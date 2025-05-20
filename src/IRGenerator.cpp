#include "IRGenerator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

namespace {

auto ValueExpHandler(IRGenerator::Context*, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);
  return node->Symbol();
}

auto BinOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (children.size() != 2) return {};

  auto sym_lhs = ctx->ExpRoute(children.front());
  auto sym_rhs = ctx->ExpRoute(children.back());

  assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());
  auto sym_tmp = ctx->NewTempVariable(sym_lhs->Scope());

  auto lhs = ctx->InnSymName(sym_lhs);
  auto rhs = ctx->InnSymName(sym_rhs);
  auto temp = ctx->InnSymName(sym_tmp);

  auto opera = node->Symbol()->Name();

  if (opera == "+") ctx->AppendInstruction({"add", lhs, rhs, temp});
  if (opera == "*") ctx->AppendInstruction({"mul", lhs, rhs, temp});
  if (opera == "/") ctx->AppendInstruction({"div", lhs, rhs, temp});
  if (opera == "-") ctx->AppendInstruction({"sub", lhs, rhs, temp});
  if (opera == "%") ctx->AppendInstruction({"mod", lhs, rhs, temp});

  return sym_tmp;
}

auto UnOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (children.empty()) return {};

  auto sym_operand = ctx->ExpRoute(children.front());
  auto sym_tmp = ctx->NewTempVariable(sym_operand->Scope());

  auto operand = ctx->InnSymName(sym_operand);
  auto temp = ctx->InnSymName(sym_tmp);

  ctx->AppendInstruction({"mov", operand, {}, temp});

  if (node->Symbol()->Name() == "-") {
    auto sym_tmp1 = ctx->NewTempVariable(sym_tmp->Scope());
    ctx->AppendInstruction({"neg", temp, {}, ctx->InnSymName(sym_tmp1)});
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

  ctx->AppendInstruction({"rtn", ctx->InnSymName(sym_val)});
  return {};
}

auto AssignExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().size() != 2) return {};

  auto sym_rhs = ctx->ExpRoute(node->Children().back());
  auto sym_lhs = ctx->ExpRoute(node->Children().front());

  auto rhs = ctx->InnSymName(sym_rhs);
  auto lhs = ctx->InnSymName(sym_lhs);

  ctx->AppendInstruction({"mov", rhs, "", lhs});

  return {};
}

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstruction>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(6) << i.op;

    if (!i.result.empty()) {
      f << i.result;
      if (!i.arg1.empty()) f << ", " << i.arg1;
      if (!i.arg2.empty()) f << ", " << i.arg2;
    } else if (!i.arg1.empty()) {
      f << i.arg1;
      if (!i.arg2.empty()) f << ", " << i.arg2;
    }

    f << "\n";
  }
}

void PrintInstructionA2s(std::ostream& f,
                         const std::vector<IRInstructionA2>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(6) << i.op;

    if (!i.arg1.empty()) {
      f << i.arg1;
      if (!i.arg2.empty()) f << ", " << i.arg2;
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

  convert_instructions();

  return instructions_2_addr_;
}

auto IRGenerator::Context::NewTempVariable(const ScopePtr& scope) -> SymbolPtr {
  assert(scope != nullptr);
  return ig_->new_temp_variable(scope);
}

auto IRGenerator::Context::Instructions() const -> std::vector<IRInstruction> {
  return instructions_;
}

void IRGenerator::Print3Addr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  std::ofstream f(path);
  PrintInstructions(f, instructions_ssa_);
  f.close();
}

void IRGenerator::Print2Addr(const std::string& path) {
  if (ctx_->Instructions().empty()) return;

  std::ofstream f(path);
  PrintInstructionA2s(f, instructions_2_addr_);
  f.close();
}

void IRGenerator::Context::AppendInstruction(const IRInstruction& i) {
  instructions_.push_back(i);
}

auto IRGenerator::Context::ExpRoute(const ASTNodePtr& node) -> SymbolPtr {
  if (handler_) return handler_(this, node);
  return {};
}

auto IRGenerator::Context::InnSymName(const SymbolPtr& symbol) -> std::string {
  assert(symbol != nullptr);

  auto sym = ig_->lookup_variable(symbol);

  // is an intermediate
  if (sym == nullptr) return symbol->Name();

  // checking
  assert(sym->Name() == symbol->Name());
  assert(!sym->Value().empty());

  return sym->Value();
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
      def_symbol_helper_(SymbolType::kDEFINE, symbol_table_) {}

void IRGenerator::convert_instructions() {
  std::vector<IRInstructionA2> result;

  for (const auto& i : instructions_ssa_) {
    if (i.op == "add" || i.op == "sub" || i.op == "mul" || i.op == "div" ||
        i.op == "mod") {
      if (i.result != i.arg1) {
        result.push_back({"mov", i.result, i.arg1});
      }
      result.push_back({i.op, i.result, i.arg2});
    } else if (i.op == "neg") {
      result.push_back({"mov", i.result, i.arg1});
      result.push_back({i.op, i.result});
    } else if (i.op == "mov") {
      result.push_back({i.op, i.result, i.arg1});
    } else if (i.op == "rtn") {
      result.push_back({i.op, i.arg1});
    } else {
      std::cout << "Unsupported op: " << i.op << '\n';
    }
  }

  instructions_2_addr_ = result;
}

void IRGenerator::convert2_ssa() {
  instructions_ssa_.clear();

  for (auto& inst : ctx_->Instructions()) {
    IRInstruction n_inst = inst;

    n_inst.arg1 = get_ssa_name(inst.arg1, false);
    n_inst.arg2 = get_ssa_name(inst.arg2, false);

    if (inst.op != "rtn") {
      n_inst.result = get_ssa_name(inst.result, true);
    }

    instructions_ssa_.push_back(n_inst);
  }
}

auto IRGenerator::get_ssa_name(const std::string& var, bool is_def)
    -> std::string {
  if (var.empty()) return "";

  if (is_def) {
    int& v = variable_version_[var];
    ++v;
    std::string ssa = var + std::to_string(v);
    variable_name_ssa_[var] = ssa;
    return ssa;
  }

  if (variable_name_ssa_.count(var) != 0U) return variable_name_ssa_[var];
  return var;
}

auto IRGenerator::new_temp_variable(const ScopePtr& scope) -> SymbolPtr {
  assert(scope != nullptr);

  if (scope == nullptr) return nullptr;

  auto tmp_var_name =
      "t_" + std::to_string(scope->Id()) + "_" + std::to_string(tmp_var_idx_++);

  auto sym = symbol_table_->AddSymbol(SymbolType::kDEFINE, tmp_var_name,
                                      tmp_var_name, true, scope->Id());

  assert(sym != nullptr);
  return sym;
}

auto IRGenerator::lookup_variable(const SymbolPtr& ast_sym) -> SymbolPtr {
  return def_symbol_helper_.LookupSymbol(ast_sym->Scope(), ast_sym->Name());
}
