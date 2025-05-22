#include "IRGenerator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

#include "SymbolMetaTypedef.h"
#include "Utils.h"

namespace {

const std::string kBinOpType = "binop";
const std::string kUnOpType = "unop";
const std::string kCmpOpType = "cmpop";

auto IsCondJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "jmp" || n == "je" || n == "jne" || n == "jg" || n == "jl" ||
         n == "jge" || n == "jle";
}

auto IsJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "jmp" || IsCondJump(op);
}

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstruction>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(12) << i.op->Name();

    if (i.dst) {
      f << i.dst->Name();
      if (i.src_1) f << ", " << i.src_1->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    } else if (i.src_1) {
      f << i.src_1->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    }

    f << "    // ";

    if (i.dst) {
      f << i.dst->Value();
      if (i.src_1) f << ", " << i.src_1->Value();
      if (i.src_2) f << ", " << i.src_2->Value();
    } else if (i.src_1) {
      f << i.src_1->Value();
      if (i.src_2) f << ", " << i.src_2->Value();
    }

    f << "\n";
  }
}

void PrintInstructionA2s(std::ostream& f,
                         const std::vector<IRInstructionA2Ptr>& instructions) {
  for (const auto& p_i : instructions) {
    const auto& i = *p_i;
    f << std::left << std::setw(12) << i.op->Name();

    if (i.dst) {
      f << i.dst->Name();
      if (i.src) f << ", " << i.src->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    } else if (i.src) {
      f << i.src->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    }

    f << "    // ";

    if (i.dst) {
      f << i.dst->Value();
      if (i.src) f << ", " << i.src->Value();
      if (i.src_2) f << ", " << i.src_2->Value();
    } else if (i.src) {
      f << i.src->Value();
      if (i.src_2) f << ", " << i.src_2->Value();
    }

    f << "\n";
  }
}

auto ValueExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());
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
  auto sym_rhs = ctx->ExpRoute(children.back());

  assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());
  auto sym_tmp = ctx->NewTempVariable();

  auto lhs = ctx->MapSymbol(sym_lhs);
  auto rhs = ctx->MapSymbol(sym_rhs);
  auto tmp = ctx->MapSymbol(sym_tmp);

  auto opera = node->Symbol()->Name();

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

  else if (opera == "&&") {
    ctx->AddIns("andlog", tmp, lhs, rhs);
  } else if (opera == "||") {
    ctx->AddIns("orlog", tmp, lhs, rhs);
  }

  else {
    ctx->AddError("Unsupported binary operator: " + opera);
    return {};
  }

  return sym_tmp;
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
    ctx->AddIns("not", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "~") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("notb", sym_tmp1, temp);
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

  auto cond_sym = ctx->ExpRoute(children[0]);
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
  auto label_end = ctx->NewLabel();

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

  // step
  if (step) ctx->ExpRoute(step);

  ctx->AddIns("jmp", {}, label_cond);

  // end
  ctx->AddIns("label", {}, label_end);

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
};

auto IRGenerator::do_ir_generate(Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  if (ctx == nullptr || node == nullptr) return {};
  if (exp_handler_register.count(node->Type()) == 0) return {};

  return exp_handler_register[node->Type()](ctx, node);
}

auto IRGenerator::Generate(const AST& tree) -> std::vector<IRInstructionA2Ptr> {
  auto node = tree.Root();
  if (node == nullptr) return {};

  do_ir_generate(ctx_.get(), node);

  convert2_ssa();

  convert_ira3_2_ira2();

  build_cfg();

  liveness_analyse();

  return instructions_2_addr_;
}

auto IRGenerator::Context::NewTempVariable() -> SymbolPtr {
  return ig_->new_temp_variable();
}

auto IRGenerator::Context::Instructions() const -> std::vector<IRInstruction> {
  return ins_;
}

void IRGenerator::Print3Addr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  std::ofstream f(path);
  PrintInstructions(f, ins_ssa_);
  f.close();
}

void IRGenerator::Print2Addr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  std::ofstream f(path);
  PrintInstructionA2s(f, instructions_2_addr_);
  f.close();
}

auto IRGenerator::Context::ExpRoute(const ASTNodePtr& node) -> SymbolPtr {
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
  reg_op("mov");  // spec op
  reg_op("rtn");  // spec op
  reg_op("neg", kUnOpType);
  reg_op("not", kUnOpType);
  reg_op("notb", kUnOpType);

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
  reg_op("andlog", kBinOpType);
  reg_op("orlog", kBinOpType);

  reg_op("brz");
  reg_op("jmp");
  reg_op("label");

  // 2 addr
  reg_op("cmp");
  reg_op("je");
}

void IRGenerator::convert_ira3_2_ira2() {
  std::vector<IRInstructionA2> res;

  for (const auto& i : ins_ssa_) {
    const auto& op = i.op->Name();
    const auto& op_type = i.op->Value();

    if (op_type == kBinOpType) {
      if (i.dst != i.src_1) res.emplace_back(map_op("mov"), i.dst, i.src_1);
      res.emplace_back(i.op, i.dst, i.src_2);
      continue;
    }

    if (op_type == kUnOpType) {
      if (i.dst != i.src_1) res.emplace_back(map_op("mov"), i.dst, i.src_1);
      res.emplace_back(i.op, i.dst);
      continue;
    }

    if (op_type == kCmpOpType) {
      res.emplace_back(i.op, i.dst, i.src_1);
      res.back().src_2 = i.src_2;
      continue;
    }

    if (op == "mov") {
      if (i.dst != i.src_1) res.emplace_back(i.op, i.dst, i.src_1);
      continue;
    }

    if (op == "rtn") {
      res.emplace_back(i.op, nullptr, i.src_1);
      continue;
    }

    if (op == "brz") {
      if (i.src_1->Value() == "immediate") {
        if (i.src_1->Name() == "0") {
          res.emplace_back(map_op("jmp"), nullptr, i.src_2);
        }
        continue;
      }
      res.emplace_back(map_op("cmp"), i.src_1, map_sym("0", "immediate"));
      res.emplace_back(map_op("je"), nullptr, i.src_2);
      continue;
    }

    if (op == "jmp") {
      res.emplace_back(i.op, nullptr, i.src_1);
      continue;
    }

    if (op == "label") {
      res.emplace_back(i.op, nullptr, i.src_1);
      continue;
    }

    SPDLOG_ERROR("Unsupported op: {}", op);
  }

  for (const auto& i : res) {
    instructions_2_addr_.push_back(std::make_shared<IRInstructionA2>(i));
  }
}

void IRGenerator::convert2_ssa() {
  ins_ssa_.clear();

  for (auto& inst : ctx_->Instructions()) {
    auto n_inst = inst;

    n_inst.src_1 = map_ssa(inst.src_1, false);
    n_inst.src_2 = map_ssa(inst.src_2, false);
    if (inst.dst != nullptr) n_inst.dst = map_ssa(inst.dst, true);

    ins_ssa_.push_back(n_inst);
  }
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
  // TODO(eric): should set specific type
  return map_sym(tmp_var_name, "variable");
}

auto IRGenerator::new_phi_variable(const SymbolPtr& var, int block_id)
    -> SymbolPtr {
  assert(var != nullptr);
  auto tmp_var_name = "phi_ir_" + std::to_string(block_id) + "_" + var->Name();
  // TODO(eric): should set specific type
  return map_sym(tmp_var_name, "variable");
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
    sym = MapSymbol(sym->Value(), "variable");
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

  ins_.emplace_back(sym_op, dst, src_1, src_2);
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
  std::vector<CFGBasicBlockPtr> blocks;
  std::map<std::string, int> label2block;

  int current_block_id = 0;
  auto new_block = [&]() -> CFGBasicBlockPtr {
    auto bb = std::make_shared<CFGBasicBlock>(current_block_id++);
    blocks.push_back(bb);
    return bb;
  };

  bool need_new_block = true;
  CFGBasicBlockPtr bb = nullptr;
  auto ir = instructions_2_addr_;

  for (const auto& i : ir) {
    const auto& ins = i;
    auto opn = ins->op->Name();

    // last ins as jmp
    if (need_new_block && opn != "label") {
      bb = new_block();
      need_new_block = false;
    }

    // label need jmp
    if (opn == "label") {
      bb = new_block();
      // label in ins.src not ins.dst
      if (ins->src) {
        bb->label = ins->src->Name();
        label2block[bb->label] = bb->id;
      }
      need_new_block = false;
    }

    // ensure bb for ins
    if (!bb) bb = new_block();

    bb->instrs.push_back(i);

    // jmps need new block
    if (IsJump(ins->op)) need_new_block = true;
  }

  std::map<int, Vertex> bbid2vertex;

  // add all block to cfg
  for (auto& bb : blocks) {
    bbid2vertex[bb->id] = cfg_->AddBlock(bb);
  }

  // add edge
  for (size_t idx = 0; idx < blocks.size(); ++idx) {
    auto& bb = blocks[idx];
    if (bb->instrs.empty()) continue;
    const auto& last_ins = bb->instrs.back();

    auto jump_label = [&](const SymbolPtr& label) -> int {
      auto it = label2block.find(label->Name());
      if (it != label2block.end()) return it->second;
      return -1;
    };

    auto op = last_ins->op->Name();
    if (op == "jmp") {
      // jmp
      int succ_id = jump_label(last_ins->src);
      if (succ_id != -1) cfg_->AddEdge(bb->id, succ_id);
    } else if (IsCondJump(last_ins->op)) {
      // cond jmp
      int succ1 = jump_label(last_ins->src);
      if (succ1 != -1) cfg_->AddEdge(bb->id, succ1);
      // fall through
      if (idx + 1 < blocks.size()) cfg_->AddEdge(bb->id, blocks[idx + 1]->id);
    } else {
      // fall through
      if (idx + 1 < blocks.size()) cfg_->AddEdge(bb->id, blocks[idx + 1]->id);
    }
  }
}

void IRGenerator::liveness_analyse() {
  auto ir = instructions_2_addr_;
  auto block_list = cfg_->Blocks();

  for (auto& block : block_list) {
    std::set<SymbolPtr> use;
    std::set<SymbolPtr> def;
    for (const auto& ins : block->instrs) {
      // skip label
      if (ins->op->Name() == "label") continue;

      // not def yet
      for (auto& s : ins->Use()) {
        if (!IsVariable(s)) continue;
        if (def.count(s) == 0) use.insert(s);
      }
      if (ins->dst && IsVariable(ins->dst)) def.insert(ins->dst);
    }
    block->use = use;
    block->def = def;
    block->in = {};
    block->out = {};
  }

  std::unordered_map<int, CFGBasicBlockPtr> id2block;
  for (auto& bb : block_list) {
    id2block[bb->id] = bb;
  }

  bool changed = true;
  while (changed) {
    changed = false;

    // temp store
    std::map<int, std::set<SymbolPtr>> next_in;
    std::map<int, std::set<SymbolPtr>> next_out;

    // calculate all new_in/new_out
    for (auto& block : block_list) {
      auto& use = block->use;
      auto& def = block->def;

      // new_out = ⋃ IN[succ]
      std::set<SymbolPtr> new_out;
      for (auto& succ : cfg_->Successors(block->id)) {
        auto succ_bb = id2block[succ->id];
        new_out.insert(succ_bb->in.begin(), succ_bb->in.end());
      }

      // new_in = use ∪ (new_out – def)
      std::set<SymbolPtr> new_in = use;
      std::set<SymbolPtr> diff;
      std::set_difference(new_out.begin(), new_out.end(), def.begin(),
                          def.end(), std::inserter(diff, diff.end()));
      new_in.insert(diff.begin(), diff.end());

      next_in[block->id] = std::move(new_in);
      next_out[block->id] = std::move(new_out);
    }

    // write out all
    for (auto& block : block_list) {
      if (block->in != next_in[block->id] ||
          block->out != next_out[block->id]) {
        changed = true;
        block->in = next_in[block->id];
        block->out = next_out[block->id];
      }
    }
  }
}

void IRGenerator::insert_phi() {
  for (const auto& block : cfg_->Blocks()) {
    // block have more than one predecessor
    if (cfg_->Predecessors(block->id).size() > 1) {
      // find all defined vars
      std::map<SymbolPtr, std::vector<SymbolPtr>>
          phi_candidates;  // var -> ssa var
      for (const auto& pred : cfg_->Predecessors(block->id)) {
        for (const auto& v : pred->def) {
          phi_candidates[v].push_back(v);
        }
      }

      for (const auto& [var, versions] : phi_candidates) {
        if (versions.size() > 1) {
          // new phi var
          auto phi_var = new_phi_variable(var, block->id);
          auto phi_instr = IRInstructionA2{map_op("phi"), phi_var};
          phi_instr.phi_srcs = versions;

          // 5. 在block的起始插入PHI指令
          // ir_list.insert(ir_list.begin() + block->instr_indices.front(),
          //                phi_instr);
          // 6. 后续此block内的该变量，统一用phi_var
          // 实现细节：用map或在block里替换
        }
      }
    }
  }
}

auto IRGenerator::ControlFlowGraph() -> ControlFlowGraphPtr { return cfg_; }

void IRGenerator::PrintCFG(const std::string& path) {
  std::ofstream f(path);
  cfg_->Print(f);
  f.close();
}
