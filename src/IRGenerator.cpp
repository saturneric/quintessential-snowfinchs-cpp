#include "IRGenerator.h"

#include <fstream>
#include <iostream>

namespace {

auto ValueExpHandler(IRGenerator::Context*, const ASTNodePtr& node)
    -> std::string {
  return node->Operation()->Value();
}

auto BinOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> std::string {
  auto children = node->Children();

  if (children.size() != 2) return {};

  auto lhs = ctx->ExpRoute(children.front());
  auto rhs = ctx->ExpRoute(children.back());
  auto temp = ctx->NewTempVariable();

  auto opera = node->Operation()->Value();

  if (opera == "+") ctx->AppendInstruction({"add", lhs, rhs, temp});
  if (opera == "*") ctx->AppendInstruction({"mul", lhs, rhs, temp});
  if (opera == "/") ctx->AppendInstruction({"div", lhs, rhs, temp});
  if (opera == "-") ctx->AppendInstruction({"sub", lhs, rhs, temp});
  if (opera == "%") ctx->AppendInstruction({"mod", lhs, rhs, temp});
  return temp;
}

auto UnOpExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> std::string {
  auto children = node->Children();

  if (children.empty()) return {};

  auto operand = ctx->ExpRoute(children.front());
  auto temp = ctx->NewTempVariable();

  ctx->AppendInstruction({"mov", operand, {}, temp});

  if (node->Operation()->Value() == "-") {
    auto temp1 = ctx->NewTempVariable();
    ctx->AppendInstruction({"neg", temp, {}, temp1});
    temp = temp1;
  }

  return temp;
}

auto MeaninglessNodeHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> std::string {
  for (const auto& child : node->Children()) {
    ctx->ExpRoute(child);
  }

  return {};
}

auto ReturnExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> std::string {
  auto children = node->Children();

  if (node->Children().empty()) return "";

  auto val = ctx->ExpRoute(children.front());

  ctx->AppendInstruction({"rtn", val});
  return "";
}

auto AssignExpHandler(IRGenerator::Context* ctx, const ASTNodePtr& node)
    -> std::string {
  auto children = node->Children();

  if (node->Children().size() != 2) return "";

  auto rhs = ctx->ExpRoute(node->Children().back());
  auto lhs = ctx->ExpRoute(node->Children().front());

  ctx->AppendInstruction({"mov", rhs, "", lhs});

  return "";
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
    -> std::string {
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

auto IRGenerator::Context::NewTempVariable() -> std::string {
  return "t" + std::to_string(temp_variable_counter_++);
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

auto IRGenerator::Context::ExpRoute(const ASTNodePtr& node) -> std::string {
  if (handler_) return handler_(this, node);
  return {};
}

IRGenerator::Context::Context(ExpHandler handler)
    : handler_(std::move(handler)) {}

IRGenerator::IRGenerator()
    : ctx_(std::make_shared<Context>([this](auto&& PH1, auto&& PH2) {
        return do_ir_generate(std::forward<decltype(PH1)>(PH1),
                              std::forward<decltype(PH2)>(PH2));
      })) {}

void IRGenerator::convert_instructions() {
  std::vector<IRInstructionA2> result;

  for (const auto& instr : instructions_ssa_) {
    if (instr.op == "add" || instr.op == "sub" || instr.op == "mul" ||
        instr.op == "div") {
      if (instr.result != instr.arg1) {
        result.push_back({"mov", instr.result, instr.arg1});
      }
      result.push_back({instr.op, instr.result, instr.arg2});
    } else if (instr.op == "neg") {
      result.push_back({"mov", instr.result, instr.arg1});
      result.push_back({instr.op, instr.result});
    } else if (instr.op == "mov") {
      result.push_back({instr.op, instr.result, instr.arg1});
    } else if (instr.op == "rtn") {
      result.push_back({instr.op, instr.arg1});
    } else {
      std::cout << "Unsupported op: " << instr.op << '\n';
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
