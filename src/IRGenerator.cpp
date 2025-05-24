#include "IRGenerator.h"

#include <fstream>

#include "ControlFlowGraphAlgos.h"
#include "SymbolDefs.h"
#include "SymbolMetaTypedef.h"
#include "Utils.h"

namespace {

auto SSAOriginSym(const SymbolPtr& s) -> SymbolPtr {
  return MetaGet<SymbolPtr>(s, SymbolMetaKey::kSSA_ORIGIN_SYM);
}

auto DeclareHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());
  assert(sym != nullptr);

  ctx->AddIns("dcl", sym);
  return sym;
}

auto ValueExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());

  // ident
  if (node->Type() == ASTNodeType::kIDENT) return sym;

  // immediate
  auto type = MetaGet<SymbolMetaType>(node->Symbol(), SymbolMetaKey::kTYPE);

  if (type == SymbolMetaType::kBOOL) {
    if (sym->Name() == "true") {
      return ctx->MapSymbol("1", "immediate");
    }
    return ctx->MapSymbol("0", "immediate");
  }

  return sym;
}

auto BinOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (children.size() != 2) return {};

  auto sym_lhs = ctx->ExpRoute(children.front());
  auto lhs = ctx->MapSymbol(sym_lhs);

  auto opera = node->Symbol()->Name();
  auto tmp = ctx->MapSymbol(ctx->NewTempVariable());

  if (opera == "&&") {
    auto end = ctx->NewLabel();

    // tmp = 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("0", "immediate"));

    // if (!lhs) goto false_label
    ctx->AddIns("brz", nullptr, lhs, end);

    auto sym_rhs = ctx->ExpRoute(children.back());
    auto rhs = ctx->MapSymbol(sym_rhs);
    assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

    // if (!rhs) goto false_label
    ctx->AddIns("brz", nullptr, rhs, end);

    // all not 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("1", "immediate"));

    // end:
    ctx->AddIns("label", nullptr, end);

    // avoid next operations
    return tmp;
  }

  if (opera == "||") {
    auto end = ctx->NewLabel();

    // tmp = 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("1", "immediate"));

    // if (!lhs) goto false_label
    ctx->AddIns("brnz", nullptr, lhs, end);

    auto sym_rhs = ctx->ExpRoute(children.back());
    auto rhs = ctx->MapSymbol(sym_rhs);
    assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

    // if (!rhs) goto false_label
    ctx->AddIns("brnz", nullptr, rhs, end);

    // all not 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("0", "immediate"));

    // end:
    ctx->AddIns("label", nullptr, end);

    // avoid next operations
    return tmp;
  }

  auto sym_rhs = ctx->ExpRoute(children.back());
  auto rhs = ctx->MapSymbol(sym_rhs);
  assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

  if (opera == "+") {
    ctx->AddIns("add", tmp, lhs, rhs);
  } else if (opera == "-") {
    ctx->AddIns("sub", tmp, lhs, rhs);
  } else if (opera == "*") {
    ctx->AddIns("mul", tmp, lhs, rhs);
  } else if (opera == "/") {
    ctx->AddIns("div", tmp, lhs, rhs);
  } else if (opera == "%") {
    ctx->AddIns("mod", tmp, lhs, rhs);
  }

  else if (opera == "==") {
    ctx->AddIns("eq", tmp, lhs, rhs);
  } else if (opera == "!=") {
    ctx->AddIns("neq", tmp, lhs, rhs);
  } else if (opera == "<") {
    ctx->AddIns("lt", tmp, lhs, rhs);
  } else if (opera == "<=") {
    ctx->AddIns("le", tmp, lhs, rhs);
  } else if (opera == ">") {
    ctx->AddIns("gt", tmp, lhs, rhs);
  } else if (opera == ">=") {
    ctx->AddIns("ge", tmp, lhs, rhs);
  }

  else if (opera == "&") {
    ctx->AddIns("band", tmp, lhs, rhs);
  } else if (opera == "|") {
    ctx->AddIns("bor", tmp, lhs, rhs);
  } else if (opera == "^") {
    ctx->AddIns("bxor", tmp, lhs, rhs);
  }

  else if (opera == "<<") {
    auto tmp1 = ctx->NewTempVariable();
    ctx->AddIns("band", tmp1, rhs, ctx->MapSymbol("31", "immediate"));
    ctx->AddIns("sal", tmp, lhs, tmp1);
  } else if (opera == ">>") {
    auto tmp1 = ctx->NewTempVariable();
    ctx->AddIns("band", tmp1, rhs, ctx->MapSymbol("31", "immediate"));
    ctx->AddIns("sar", tmp, lhs, tmp1);
  }

  else {
    ctx->AddError("Unsupported binary operator: " + opera);
    return {};
  }

  return tmp;
}

auto UnOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (children.empty()) return {};

  auto sym_operand = ctx->ExpRoute(children.front());
  auto sym_tmp = ctx->NewTempVariable();

  auto operand = ctx->MapSymbol(sym_operand);
  auto temp = ctx->MapSymbol(sym_tmp);

  ctx->AddIns("mov", temp, operand);

  auto op = node->Symbol()->Name();
  if (op == "-") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("neg", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "!") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("lnot", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "~") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("bnot", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else {
    ctx->AddError("Unsupported unary operator: " + op);
    return {};
  }

  return sym_tmp;
}

auto MeaninglessNodeHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  for (const auto& child : node->Children()) {
    ctx->ExpRoute(child);
  }
  return {};
}

auto ReturnExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().empty()) return {};

  auto sym_val = ctx->ExpRoute(children.front());

  ctx->AddIns("rtn", {}, sym_val);
  return {};
}

auto AssignExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().size() != 2) return {};

  auto sym_rhs = ctx->ExpRoute(node->Children().back());
  auto sym_lhs = ctx->ExpRoute(node->Children().front());

  auto rhs = ctx->MapSymbol(sym_rhs);
  auto lhs = ctx->MapSymbol(sym_lhs);

  ctx->AddIns("mov", lhs, rhs);
  return {};
}

auto IfHandler(IRGenerator::Context* ctx, const ASTNodePtr& node) -> SymbolPtr {
  auto children = node->Children();

  auto cond_sym = ctx->ExpRoute(children[0]);
  auto cond = ctx->MapSymbol(cond_sym);

  auto label_then = ctx->NewLabel();
  auto label_else = children.size() > 2 ? ctx->NewLabel() : nullptr;
  auto label_end = ctx->NewLabel();

  // cond jump
  if (label_else) {
    ctx->AddIns("brz", {}, cond, label_else);  // brz: if cond == 0 jump else
    ctx->AddIns("label", {}, label_then);
    ctx->ExpRoute(children[1]);  // then
    ctx->AddIns("jmp", {}, label_end);
    ctx->AddIns("label", {}, label_else);
    ctx->ExpRoute(children.back());  // else
    ctx->AddIns("label", {}, label_end);
  } else {
    ctx->AddIns("brz", {}, cond, label_end);  // brz: if cond == 0 jump end
    ctx->AddIns("label", {}, label_then);
    ctx->ExpRoute(children[1]);  // then
    ctx->AddIns("label", {}, label_end);
  }

  return nullptr;
}

auto CondExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  auto cond_sym = ctx->ExpRoute(children.front());
  auto cond = ctx->MapSymbol(cond_sym);

  auto label_then = ctx->NewLabel();
  auto label_else = children.size() > 2 ? ctx->NewLabel() : nullptr;
  auto label_end = ctx->NewLabel();

  auto tmp = ctx->NewTempVariable();

  // cond jump
  ctx->AddIns("brz", {}, cond, label_else);  // brz: if cond == 0 jump else
  ctx->AddIns("label", {}, label_then);
  auto lhs = ctx->ExpRoute(children[1]);  // then value
  ctx->AddIns("mov", tmp, lhs);
  ctx->AddIns("jmp", {}, label_end);
  ctx->AddIns("label", {}, label_else);
  ctx->ExpRoute(children.back());             // else
  auto rhs = ctx->ExpRoute(children.back());  // else value
  ctx->AddIns("mov", tmp, rhs);
  ctx->AddIns("label", {}, label_end);

  return tmp;
}

auto WhileHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  const auto& children = node->Children();

  ASTNodePtr init;
  ASTNodePtr cond;
  ASTNodePtr step;
  ASTNodePtr body;

  for (const auto& child : children) {
    switch (child->Tag()) {
      case ASTNodeTag::kINIT:
        init = child;
        break;
      case ASTNodeTag::kCOND:
        cond = child;
        break;
      case ASTNodeTag::kSTEP:
        step = child;
        break;
      case ASTNodeTag::kBODY:
        body = child;
        break;
      default:
        break;
    }
  }

  auto label_cond = ctx->NewLabel();
  auto label_body = ctx->NewLabel();
  auto label_step = ctx->NewLabel();
  auto label_end = ctx->NewLabel();

  // set continue and break label
  auto symbol = node->Symbol();
  MetaSet(symbol, SymbolMetaKey::kCONTINUE_LABEL, label_step);
  MetaSet(symbol, SymbolMetaKey::kBREAK_LABEL, label_end);

  // 1. init
  if (init) ctx->ExpRoute(init);

  // cond
  ctx->AddIns("label", {}, label_cond);
  if (!cond) {
    // endless loop
    ctx->AddIns("jmp", {}, label_body);
  } else {
    auto cond_sym = ctx->ExpRoute(cond);
    auto cond_ir = ctx->MapSymbol(cond_sym);
    ctx->AddIns("brz", {}, cond_ir, label_end);  // if cond==0 jump
  }

  // body
  ctx->AddIns("label", {}, label_body);
  if (body) ctx->ExpRoute(body);

  ctx->AddIns("label", {}, label_step);

  // step
  if (step) ctx->ExpRoute(step);

  ctx->AddIns("jmp", {}, label_cond);

  // end
  ctx->AddIns("label", {}, label_end);

  return nullptr;
}

auto ContinueBreakHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto p_sym = node->Symbol();
  assert(p_sym != nullptr);

  if (node->Type() == ASTNodeType::kCONTINUE) {
    auto continue_label =
        MetaGet<SymbolPtr>(p_sym, SymbolMetaKey::kCONTINUE_LABEL);
    if (continue_label == nullptr) return nullptr;

    ctx->AddIns("jmp", {}, continue_label);
  }

  if (node->Type() == ASTNodeType::kBREAK) {
    auto break_label = MetaGet<SymbolPtr>(p_sym, SymbolMetaKey::kBREAK_LABEL);
    if (break_label == nullptr) return nullptr;

    ctx->AddIns("jmp", {}, break_label);
  }

  return nullptr;
}

}  // namespace

std::map<ASTNodeType, IRGenerator::ExpHandler>
    IRGenerator::exp_handler_register = {
        {ASTNodeType::kASSIGN, AssignExpHandler},
        {ASTNodeType::kDECLARE, MeaninglessNodeHandler},
        {ASTNodeType::kRETURN, ReturnExpHandler},
        {ASTNodeType::kUN_OP, UnOpExpHandler},
        {ASTNodeType::kBIN_OP, BinOpExpHandler},
        {ASTNodeType::kVALUE, ValueExpHandler},
        {ASTNodeType::kPROGRAM, MeaninglessNodeHandler},
        {ASTNodeType::kIDENT, ValueExpHandler},
        {ASTNodeType::kBLOCK, MeaninglessNodeHandler},
        {ASTNodeType::kIF, IfHandler},
        {ASTNodeType::kCOND_EXP, CondExpHandler},
        {ASTNodeType::kWHILE, WhileHandler},
        {ASTNodeType::kCONTINUE, ContinueBreakHandler},
        {ASTNodeType::kBREAK, ContinueBreakHandler},
};

auto IRGenerator::do_ir_generate(Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  if (ctx == nullptr || node == nullptr) return {};
  if (exp_handler_register.count(node->Type()) == 0) return {};

  return exp_handler_register[node->Type()](ctx, node);
}

void IRGenerator::Generate(const AST& tree) {
  auto node = tree.Root();
  if (node == nullptr) return;

  do_ir_generate(ctx_.get(), node);

  build_cfg();

  block_level_liveness_analyse();

  instruction_level_liveness_analyse();

  block_level_def_analyse();

  insert_phi();

  // convert2_ssa();
}

auto IRGenerator::Context::NewTempVariable() -> SymbolPtr {
  return ig_->new_temp_variable();
}

auto IRGenerator::Context::Instructions() const
    -> std::vector<IRInstructionPtr> {
  return ins_;
}

void IRGenerator::PrintAddr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  std::ofstream f(path);
  PrintInstructions(f, cfg_->Instructions());
  f.close();
}

auto IRGenerator::Context::ExpRoute(const ASTNodePtr& node) -> SymbolPtr {
  if (node == nullptr) return nullptr;

  // do inheritance of some meta: break_label and continue_label
  if (node->Parent() != nullptr) {
    node->Symbol()->Inheritance(node->Parent()->Symbol());
  }

  if (handler_) return handler_(this, node);
  return {};
}

auto IRGenerator::Context::MapDefSym(const SymbolPtr& symbol) -> SymbolPtr {
  auto sym = ig_->lookup_variable(symbol);

  // is an intermediate
  if (sym == nullptr) return nullptr;

  // checking
  assert(sym->Type() == SymbolType::kDEFINE);
  assert(sym->Name() == symbol->Name());
  assert(!sym->Value().empty());

  return sym;
}

IRGenerator::Context::Context(IRGenerator* ig, ExpHandler handler)
    : ig_(ig), handler_(std::move(handler)) {}

IRGenerator::IRGenerator(SymbolTablePtr symbol_table)
    : ctx_(std::make_shared<Context>(this,
                                     [this](auto&& PH1, auto&& PH2) {
                                       return do_ir_generate(
                                           std::forward<decltype(PH1)>(PH1),
                                           std::forward<decltype(PH2)>(PH2));
                                     })),
      symbol_table_(std::move(symbol_table)),
      def_symbol_helper_(SymbolType::kDEFINE, symbol_table_),
      ir_symbol_helper_(SymbolType::kIR, symbol_table_),
      cfg_(std::make_shared<class ControlFlowGraph>()) {
  // register op
  // TODO(eric): use dymatic handler approach

  reg_op("neg", kUnOpType);
  reg_op("lnot", kUnOpType);
  reg_op("bnot", kUnOpType);

  reg_op("add", kBinOpType);
  reg_op("sub", kBinOpType);
  reg_op("mul", kBinOpType);
  reg_op("div", kBinOpType);
  reg_op("mod", kBinOpType);
  reg_op("eq", kCmpOpType);
  reg_op("neq", kCmpOpType);
  reg_op("lt", kCmpOpType);
  reg_op("le", kCmpOpType);
  reg_op("gt", kCmpOpType);
  reg_op("ge", kCmpOpType);
  reg_op("land", kBinOpType);
  reg_op("lor", kBinOpType);
  reg_op("band", kBinOpType);
  reg_op("bor", kBinOpType);
  reg_op("bxor", kBinOpType);
  reg_op("shl", kBinOpType);
  reg_op("shr", kBinOpType);
  reg_op("sal", kBinOpType);
  reg_op("sar", kBinOpType);
  reg_op("test", kBinOpType);

  reg_op("brz");
  reg_op("brnz");
  reg_op("label");

  reg_op("cmp");
  reg_op("jmp");
  reg_op("je");
  reg_op("jne");
  reg_op("jge");
  reg_op("jg");
  reg_op("jle");
  reg_op("jl");
  reg_op("jnz");

  reg_op("mov");  // spec op
  reg_op("rtn");  // spec op

  // live control
  reg_op("dcl");
  reg_op("phi");
}

void IRGenerator::convert2_ssa() {
  // ins_ssa_.clear();

  // for (auto& inst : ctx_->Instructions()) {
  //   auto n_inst = inst;

  //   n_inst.src_1 = map_ssa(inst.src_1, false);
  //   n_inst.src_2 = map_ssa(inst.src_2, false);
  //   if (inst.dst != nullptr) n_inst.dst = map_ssa(inst.dst, true);

  //   ins_ssa_.push_back(n_inst);
  // }

  // auto graph = cfg_->Graph();
  // const auto blocks = cfg_->Blocks();
  // std::map<SymbolPtr, std::set<int>> def_blocks;
  // for (auto& block : cfg_->Blocks()) {
  //   for (const auto& v : block->def) {
  //     def_blocks[v].insert(block->id);
  //   }
  // }

  // std::vector<Vertex> dom_tree(num_vertices(graph));
  // auto entry = cfg_->VertexByBlockId(0);

  // lengauer_tarjan_dominator_tree(
  //     graph, entry,
  //     make_iterator_property_map(dom_tree.begin(),
  //                                get(boost::vertex_index, graph)));

  // std::vector<std::set<Vertex>> dom_frontier(num_vertices(graph));
  // for (Vertex v = 0; v < num_vertices(graph); ++v) {
  //   if (in_degree(v, graph) >= 2) {  // more than one pre block
  //     for (auto in_edge : make_iterator_range(in_edges(v, graph))) {
  //       Vertex u = source(in_edge, graph);
  //       Vertex runner = u;
  //       while (runner != dom_tree[v] && runner != entry) {
  //         dom_frontier[runner].insert(v);
  //         runner = dom_tree[runner];
  //       }
  //     }
  //   }
  // }

  // std::map<SymbolPtr, std::set<int>> phi_blocks;

  // for (const auto& [var, blocks] : def_blocks) {
  //   std::set<int> has_already;
  //   std::queue<int> worklist;
  //   for (auto b : blocks) worklist.push(b);

  //   while (!worklist.empty()) {
  //     auto x = worklist.front();
  //     worklist.pop();
  //     for (auto y : dom_frontier[x]) {
  //       if (phi_blocks[var].count(y) == 0) {
  //         phi_blocks[var].insert(y);
  //         cfg_->VertexByBlockId(y)->insert_phi(var);  //
  //         if (def_blocks[var].count(y) == 0) worklist.push(y);
  //       }
  //     }
  //   }
  // }
}

auto IRGenerator::map_ssa(const SymbolPtr& sym, bool is_def) -> SymbolPtr {
  if (sym == nullptr) return nullptr;

  if (is_def) {
    int ver = MetaGet<int>(sym, SymbolMetaKey::kSSA_VERSION, 0) + 1;
    MetaSet(sym, SymbolMetaKey::kSSA_VERSION, ver);

    auto sym_ssa =
        map_sym(sym->Name() + "_v" + std::to_string(ver), sym->Value());
    MetaSet(sym_ssa, SymbolMetaKey::kSSA_ORIGIN_SYM, sym);
    MetaSet(sym, SymbolMetaKey::kSSA_LATEST_SYM, sym_ssa);
  }

  auto sym_ssa = sym->MetaData(SymbolMetaKey::kSSA_LATEST_SYM);
  if (sym_ssa.has_value()) return std::any_cast<SymbolPtr>(sym_ssa);
  return sym;
}

auto IRGenerator::new_temp_variable() -> SymbolPtr {
  auto tmp_var_name = "tmp_ir_" + std::to_string(tmp_var_idx_++);
  return map_sym(tmp_var_name, "variable");
}

auto IRGenerator::new_phi_variable(const SymbolPtr& var, int block_id)
    -> SymbolPtr {
  assert(var != nullptr);
  auto tmp_var_name = "phi_ir_" + std::to_string(block_id) + "_" + var->Name();

  auto sym = map_sym(tmp_var_name, "variable");
  MetaSet(sym, SymbolMetaKey::kSSA_ORIGIN_SYM, var);
  return sym;
}

auto IRGenerator::lookup_variable(const SymbolPtr& ast_sym) -> SymbolPtr {
  return def_symbol_helper_.LookupSymbol(ast_sym->Scope(), ast_sym->Name());
}

auto IRGenerator::map_sym(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kIR, name, type, true);
}

auto IRGenerator::Context::MapSymbol(const std::string& name,
                                     const std::string& type) -> SymbolPtr {
  return ig_->map_sym(name, type);
}

auto IRGenerator::Context::MapSymbol(const SymbolPtr& symbol) -> SymbolPtr {
  auto sym = symbol;
  assert(sym != nullptr);

  // should convert to def symbol
  if (sym->Type() == SymbolType::kAST) {
    sym = MapDefSym(sym);

    // not in symbol table, it's an immediate
    if (sym == nullptr) sym = MapSymbol(symbol->Name(), "immediate");
  }

  assert(sym != nullptr);

  // should then convert to ir symbol
  if (sym->Type() == SymbolType::kDEFINE) {
    const auto type = sym->MetaData(SymbolMetaKey::kTYPE);
    auto o_sym = sym;
    sym = MapSymbol(sym->Value(), "variable");
    MetaSet(sym, SymbolMetaKey::kDEF_SYMBOL, o_sym);
  }

  assert(sym != nullptr);

  // should return now
  if (sym->Type() == SymbolType::kIR) return sym;

  assert(sym != nullptr);

  SPDLOG_ERROR("unknown symbol type of symbol: {}", symbol->Name());
  return nullptr;
}

auto IRGenerator::Context::MapOp(const std::string& name) -> SymbolPtr {
  auto op = ig_->map_op(name);
  assert(op != nullptr);

  if (op == nullptr) {
    SPDLOG_ERROR("Unsupported op provided by handler: {}", op->Name());
  }
  return op;
}

void IRGenerator::Context::AddIns(const std::string& op, SymbolPtr dst,
                                  SymbolPtr src_1, SymbolPtr src_2) {
  auto sym_op = MapOp(op);
  if (sym_op == nullptr) {
    SPDLOG_ERROR("Unsupported Instruction Op: {}", op);
    return;
  }

  ins_.emplace_back(
      std::make_shared<IRInstructionA3>(sym_op, dst, src_1, src_2));
}

auto IRGenerator::reg_op(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return map_sym(name, type.empty() ? "spec" : type);
}

auto IRGenerator::map_op(const std::string& name) -> SymbolPtr {
  return ir_symbol_helper_.LookupSymbol(nullptr, name);
}

void IRGenerator::Context::AddError(const std::string& err) {
  SPDLOG_ERROR("IR Generate Error: {}", err);
}

auto IRGenerator::Context::NewLabel() -> SymbolPtr { return ig_->new_label(); }

auto IRGenerator::new_label() -> SymbolPtr {
  auto tmp_var_name = "label_ir_" + std::to_string(tmp_var_idx_++);
  return map_sym(tmp_var_name, "label");
}

auto IRInstructionA2::Use() const -> std::vector<SymbolPtr> {
  std::vector<SymbolPtr> ret;
  if (dst) ret.emplace_back(dst);
  if (src) ret.emplace_back(src);
  if (src_2) ret.emplace_back(src_2);
  return ret;
}

void IRGenerator::build_cfg() {
  auto res = SplitToBasicBlocks(ctx_->Instructions(), *cfg_);
  BuildControlFlowEdges(*cfg_, res.blocks, res.label2block);
  AnalyzeUseDefForBlocks(res.blocks);
  MarkReachableBlocks(*cfg_);
}

void IRGenerator::block_level_liveness_analyse() {
  BlockLevelLivenessAnalyse(*cfg_);
}

void IRGenerator::block_level_def_analyse() { BlockLevelDefAnalyse(*cfg_); }

void IRGenerator::insert_phi() {
  // for (const auto& block : cfg_->Blocks()) {
  //   // block have more than one predecessor
  //   if (cfg_->Predecessors(block->id).size() > 1) {
  //     // find all defined vars
  //     std::map<SymbolPtr, std::vector<SymbolPtr>>
  //         phi_candidates;  // var -> ssa var
  //     for (const auto& pred : cfg_->Predecessors(block->id)) {
  //       for (const auto& v : pred->def) {
  //         phi_candidates[v].push_back(v);
  //       }
  //     }

  //     for (const auto& [var, versions] : phi_candidates) {
  //       if (versions.size() > 1) {
  //         // new phi var
  //         auto phi_var = new_phi_variable(var, block->id);
  //         auto phi_instr =
  //             std::make_shared<IRInstruction>(map_op("phi"), phi_var);
  //         phi_instr->phi_srcs = versions;

  //         for (auto& ins : block->instrs) {
  //           if (ins->Op()->Name() == "phi") continue;
  //           // if we have a new ssa version, stop here
  //           if (ins->DST() && ins->DST() == var) break;

  //           for (auto& sym : ins->Use()) {
  //             if (!sym) continue;
  //             sym = phi_var;
  //           }
  //         }

  //         block->instrs.insert(block->instrs.begin(), phi_instr);
  //       }
  //     }
  //   }
  // }
}

auto IRGenerator::ControlFlowGraph() -> ControlFlowGraphPtr { return cfg_; }

void IRGenerator::PrintCFG(const std::string& path) {
  std::ofstream f(path);
  cfg_->Print(f);
  f.close();
}

void IRGenerator::instruction_level_liveness_analyse() {
  InstructionLevelLivenessAnalyse(*cfg_);
}
