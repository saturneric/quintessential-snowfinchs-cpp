#include "IRGenerator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

namespace {

const std::string kBinOpType = "binop";
const std::string kUnOpType = "unop";

auto ValueExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());
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

  if (opera == "+") ctx->AddIns("add", tmp, lhs, rhs);
  if (opera == "*") ctx->AddIns("mul", tmp, lhs, rhs);
  if (opera == "/") ctx->AddIns("div", tmp, lhs, rhs);
  if (opera == "-") ctx->AddIns("sub", tmp, lhs, rhs);
  if (opera == "%") ctx->AddIns("mod", tmp, lhs, rhs);

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

  if (node->Symbol()->Name() == "-") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("neg", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
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

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstruction>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(6) << i.op->Name();

    if (i.dst) {
      f << i.dst->Name();
      if (i.src_1) f << ", " << i.src_1->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    } else if (i.src_1) {
      f << i.src_1->Name();
      if (i.src_2) f << ", " << i.src_2->Name();
    }

    f << "\n";
  }
}

void PrintInstructionA2s(std::ostream& f,
                         const std::vector<IRInstructionA2>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(6) << i.op->Name();

    if (i.dst) {
      f << i.dst->Name();
      if (i.src) f << ", " << i.src->Name();
    } else if (i.src) {
      f << i.src->Name();
    }

    f << "\n";
  }
}

}  // namespace

std::map<ASTNodeType, IRGenerator::ExpHandler>
    IRGenerator::exp_handler_resiter = {
        {ASTNodeType::kASSIGN, AssignExpHandler},
        {ASTNodeType::kDECLARE, MeaninglessNodeHandler},
        {ASTNodeType::kRETURN, ReturnExpHandler},
        {ASTNodeType::kUN_OP, UnOpExpHandler},
        {ASTNodeType::kBIN_OP, BinOpExpHandler},
        {ASTNodeType::kVALUE, ValueExpHandler},
        {ASTNodeType::kPROGRAM, MeaninglessNodeHandler},
        {ASTNodeType::kIDENT, ValueExpHandler},
};

auto IRGenerator::do_ir_generate(Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  if (ctx == nullptr || node == nullptr) return {};
  if (exp_handler_resiter.count(node->Type()) == 0) return {};

  return exp_handler_resiter[node->Type()](ctx, node);
}

auto IRGenerator::Generate(const AST& tree) -> std::vector<IRInstructionA2> {
  auto node = tree.Root();
  if (node == nullptr) return {};

  do_ir_generate(ctx_.get(), node);

  convert2_ssa();

  convert_ira3_2_ira2();

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
      ir_symbol_helper_(SymbolType::kIR, symbol_table_) {
  // register op
  // TODO(eric): use dymatic handler approach
  reg_op("mov");  // spec op
  reg_op("rtn");  // spec op
  reg_op("neg", kUnOpType);
  reg_op("add", kBinOpType);
  reg_op("sub", kBinOpType);
  reg_op("mul", kBinOpType);
  reg_op("div", kBinOpType);
  reg_op("mod", kBinOpType);
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

    if (op == "mov") {
      if (i.dst != i.src_1) res.emplace_back(i.op, i.dst, i.src_1);
      continue;
    }

    if (op == "rtn") {
      res.emplace_back(i.op, nullptr, i.src_1);
      continue;
    }

    SPDLOG_ERROR("Unsupported op: {}", op);
  }

  instructions_2_addr_ = res;
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
    auto& ver = sym->RefMetaData("ssa_version");
    if (!ver.has_value()) ver = 0;

    auto v = std::any_cast<int>(ver) + 1;
    ver = v;  // update ssa index

    auto sym_ssa =
        map_sym(sym->Name() + "_v" + std::to_string(v), sym->Value());
    sym_ssa->RefMetaData("origin_ir_symbol") = sym;

    // ref to ssa
    sym->RefMetaData("latest_ssa_symbol") = sym_ssa;
  }

  auto sym_ssa = sym->MetaData("latest_ssa_symbol");
  if (sym_ssa.has_value()) return std::any_cast<SymbolPtr>(sym_ssa);
  return sym;
}

auto IRGenerator::new_temp_variable() -> SymbolPtr {
  auto tmp_var_name = "tmp_ir_" + std::to_string(tmp_var_idx_++);
  // TODO(eric): should set specific type
  return map_sym(tmp_var_name, "int");
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

  // should convert to def symbol
  if (sym->Type() == SymbolType::kAST) {
    sym = MapDefSym(sym);

    // not in symbol table, it's an immediate
    if (sym == nullptr) sym = MapSymbol(symbol->Name(), "immediate");
  }

  assert(sym != nullptr);

  // should then convert to ir symbol
  if (sym->Type() == SymbolType::kDEFINE) {
    const auto type = sym->MetaData("type");
    sym =
        MapSymbol(sym->Value(),
                  type.has_value() ? std::any_cast<std::string>(type) : "void");
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
