#include "IRGenerator.h"

#include "model/ControlFlowGraphAlgos.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

namespace {

auto SSAOriginSym(const SymbolPtr& s) -> SymbolPtr {
  return MetaGet<SymbolPtr>(s, SymbolMetaKey::kSSA_ORIGIN_SYM);
}

}  // namespace

auto IRGenerator::do_ir_generate(IRGeneratorContext* ctx,
                                 const ASTNodePtr& node) -> SymbolPtr {
  if (ctx == nullptr || node == nullptr) return {};
  if (ir_handler_mapping_.count(node->Type()) == 0) return {};

  return ir_handler_mapping_[node->Type()](ctx, node);
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

auto IRGeneratorContext::NewTempVariable() -> SymbolPtr {
  return ig_->new_temp_variable();
}

auto IRGeneratorContext::Instructions() const -> std::vector<FuncInstructions> {
  return ins_;
}

void IRGenerator::PrintAddr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  for (const auto& fc : cfgs_) {
    std::ofstream f(path);
    PrintInstructions(f, fc.cfg->Instructions());
    f.close();
  }
}

auto IRGeneratorContext::ExpRoute(const ASTNodePtr& node) -> SymbolPtr {
  if (node == nullptr) return nullptr;

  // do inheritance of some meta: break_label and continue_label
  if (node->Parent() != nullptr) {
    node->Symbol()->Inheritance(node->Parent()->Symbol());
  }

  if (handler_) return handler_(this, node);
  return {};
}

auto IRGeneratorContext::MapDefSym(const SymbolPtr& symbol) -> SymbolPtr {
  auto sym = ig_->lookup_variable(symbol);

  // is an intermediate
  if (sym == nullptr) return nullptr;

  // checking
  assert(sym->Type() == SymbolType::kDEFINE);
  assert(sym->Name() == symbol->Name());
  assert(!sym->Value().empty());

  return sym;
}

IRGeneratorContext::IRGeneratorContext(IRGenerator* ig, IRExpHandler handler)
    : ig_(ig), handler_(std::move(handler)) {}

IRGenerator::IRGenerator(SymbolTablePtr symbol_table, IRHandlerMapping mapping)
    : ctx_(std::make_shared<IRGeneratorContext>(
          this,
          [this](auto&& PH1, auto&& PH2) {
            return do_ir_generate(std::forward<decltype(PH1)>(PH1),
                                  std::forward<decltype(PH2)>(PH2));
          })),
      symbol_table_(std::move(symbol_table)),
      def_symbol_helper_(SymbolType::kDEFINE, symbol_table_),
      ir_symbol_helper_(SymbolType::kIR, symbol_table_),
      ir_handler_mapping_(std::move(mapping)) {
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

  // function
  reg_op("call");
  reg_op("param");
  reg_op("arg");
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

auto IRGeneratorContext::MapSymbol(const std::string& name,
                                   const std::string& type) -> SymbolPtr {
  return ig_->map_sym(name, type);
}

auto IRGeneratorContext::MapSymbol(const SymbolPtr& symbol) -> SymbolPtr {
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

    if (sym->Value().rfind("__func_", 0) != std::string::npos) {
      sym = MapSymbol(sym->Value(), "function");
    } else {
      sym = MapSymbol(sym->Value(), "variable");
    }

    MetaSet(sym, SymbolMetaKey::kDEF_SYMBOL, o_sym);
  }

  assert(sym != nullptr);

  // should return now
  if (sym->Type() == SymbolType::kIR) return sym;

  assert(sym != nullptr);

  SPDLOG_ERROR("unknown symbol type of symbol: {}", symbol->Name());
  return nullptr;
}

auto IRGeneratorContext::MapOp(const std::string& name) -> SymbolPtr {
  auto op = ig_->map_op(name);
  assert(op != nullptr);

  if (op == nullptr) {
    SPDLOG_ERROR("Unsupported op provided by handler: {}", op->Name());
  }
  return op;
}

void IRGeneratorContext::AddIns(const std::string& op, SymbolPtr dst,
                                SymbolPtr src_1, SymbolPtr src_2) {
  auto sym_op = MapOp(op);
  if (sym_op == nullptr) {
    SPDLOG_ERROR("Unsupported Instruction Op: {}", op);
    return;
  }

  assert(!ins_.empty());
  if (!ins_.empty()) {
    ins_.back().ins.emplace_back(
        std::make_shared<IRInstructionA3>(sym_op, dst, src_1, src_2));
  }
}

auto IRGenerator::reg_op(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return map_sym(name, type.empty() ? "spec" : type);
}

auto IRGenerator::map_op(const std::string& name) -> SymbolPtr {
  return ir_symbol_helper_.LookupSymbol(nullptr, name);
}

void IRGeneratorContext::AddError(const std::string& err) {
  SPDLOG_ERROR("IR Generate Error: {}", err);
}

auto IRGeneratorContext::NewLabel() -> SymbolPtr { return ig_->new_label(); }

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
  for (const auto& ig : ctx_->Instructions()) {
    FuncCFG fc{ig.func, std::make_shared<class ControlFlowGraph>()};

    // do optimums before build cfg
    auto res = SplitToBasicBlocks(optimums(ig.ins), *fc.cfg);
    BuildControlFlowEdges(*fc.cfg, res.blocks, res.label2block);
    AnalyzeUseDefForBlocks(res.blocks);
    MarkReachableBlocks(*fc.cfg);

    cfgs_.push_back(fc);
  }
}

void IRGenerator::block_level_liveness_analyse() {
  for (const auto& fc : cfgs_) {
    BlockLevelLivenessAnalyse(*fc.cfg);
  }
}

void IRGenerator::block_level_def_analyse() {
  for (const auto& fc : cfgs_) {
    BlockLevelDefAnalyse(*fc.cfg);
  }
}

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

auto IRGenerator::ControlFlowGraph() -> std::vector<FuncCFG>& { return cfgs_; }

void IRGenerator::PrintCFG(const std::string& path) {
  std::ofstream f(path);
  for (const auto& fc : cfgs_) {
    fc.cfg->Print(f);
  }
  f.close();
}

void IRGenerator::instruction_level_liveness_analyse() {
  for (const auto& fc : cfgs_) {
    InstructionLevelLivenessAnalyse(*fc.cfg);
  }
}

auto IRGenerator::optimums(const std::vector<IRInstructionPtr>& irs)
    -> std::vector<IRInstructionPtr> {
  std::vector<IRInstructionPtr> ret;

  IRInstructionPtr l_ir = nullptr;
  for (const auto& ir : irs) {
    const auto op = ir->Op()->Name();
    const auto dst = ir->DST();
    const auto src_1 = ir->SRC(0);
    const auto src_2 = ir->SRC(1);

    // opt a += b; or a = a + b;
    if (op == "mov" && l_ir && l_ir->Op()->Value() == kBinOpType &&
        src_1 == l_ir->DST() && dst == l_ir->SRC(0)) {
      l_ir->SetDST(dst);
      continue;
    }

    ret.push_back(ir);
    l_ir = ir;
  }

  return ret;
}

void IRGeneratorContext::EnterInsGroup(const std::string& func) {
  ins_.push_back({func, {}});
}
