#include "ASMGenerator.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <set>

ASMGenerator::ASMGenerator(const std::vector<IRInstructionA2>& ir) : ir_(ir) {}

void ASMGenerator::Generate(const std::string& path) {
  asm_.clear();
  for (const auto& instr : ir_) translate(instr);

  std::set<std::string> data_vars;

  for (const auto& instr : ir_) {
    if (is_variable(instr.arg1)) data_vars.insert(instr.arg1);
    if (is_variable(instr.arg2)) data_vars.insert(instr.arg2);
  }

  // .data
  std::vector<std::string> lines;
  lines.emplace_back(".data");
  for (const auto& var : data_vars) {
    lines.emplace_back(var + ":" + " .quad 0");
  }
  for (const auto& line : generate_data_segment()) {
    lines.emplace_back(line);
  }

  // .text
  lines.emplace_back("\n.text");
  lines.emplace_back(".globl main\nmain:");

  for (const auto& line : asm_) {
    lines.emplace_back("    " + line);
  }

  std::ofstream f(path);
  for (const auto& line : lines) f << line << "\n";
  f.close();
}

static auto is_immediate(const std::string& s) -> bool {
  static const std::regex kImmRegex(R"(^\$\-?(0x[0-9a-fA-F]+|\d+)$)");
  return std::regex_match(s, kImmRegex);
}

void ASMGenerator::translate(const IRInstructionA2& i) {
  if (i.op == "mov") {
    const bool lhs_is_mem = !is_reg(i.arg1) && !is_immediate(i.arg1);
    const bool rhs_is_mem = !is_reg(i.arg2) && !is_immediate(i.arg2);

    if (lhs_is_mem && rhs_is_mem) {
      // memory to memory -> use temp register
      asm_.push_back("movq " + format_operand(i.arg2) + ", %rax");
      asm_.push_back("movq %rax, " + format_operand(i.arg1));
    } else {
      // valid combination
      asm_.push_back("movq " + format_operand(i.arg2) + ", " +
                     format_operand(i.arg1));
    }
  } else if (i.op == "add") {
    emit_binary_op("addq", i);
  } else if (i.op == "sub") {
    emit_binary_op("subq", i);
  } else if (i.op == "mul") {
    emit_binary_op("imulq", i);
  } else if (i.op == "div") {
    asm_.push_back("movq " + format_operand(i.arg1) + ", %rax");
    asm_.push_back("cqto");
    if (!is_variable(i.arg2)) {
      std::string const_label = "const_" + i.arg2;
      constants_.insert(i.arg2);
      asm_.push_back("idivq " + format_operand(const_label));
    } else {
      asm_.push_back("idivq " + format_operand(i.arg2));
    }
    asm_.push_back("movq %rax, " + format_operand(i.arg1));
  } else if (i.op == "neg") {
    asm_.push_back("movq " + format_operand(i.arg1) + ", %rax");
    asm_.push_back("negq %rax");
    asm_.push_back("movq %rax, " + format_operand(i.arg1));
  } else if (i.op == "rtn") {
    asm_.push_back("movq " + format_operand(i.arg1) + ", %rax");
    asm_.push_back("ret");
  } else {
    asm_.push_back("# unsupported op: " + i.op);
  }
}

void ASMGenerator::emit_binary_op(const std::string& asm_op,
                                  const IRInstructionA2& instr) {
  asm_.push_back("movq " + format_operand(instr.arg1) + ", %rax");
  asm_.push_back(asm_op + " " + format_operand(instr.arg2) + ", %rax");
  asm_.push_back("movq %rax, " + format_operand(instr.arg1));
}

auto ASMGenerator::format_operand(const std::string& operand) -> std::string {
  if (operand.empty()) return "";
  if (std::isdigit(operand[0]) || operand[0] == '-') {
    return "$" + operand;
  }
  return operand + "(%rip)";
}

auto ASMGenerator::is_variable(const std::string& s) -> bool {
  if (s.empty()) return false;
  if (std::isdigit(s[0]) || s[0] == '-') return false;
  return true;
}

auto ASMGenerator::is_reg(const std::string& s) -> bool {
  if (s.empty()) return false;
  if (s[0] == '%') return true;
  return false;
}

auto ASMGenerator::generate_data_segment() const -> std::vector<std::string> {
  std::vector<std::string> ret;
  for (const auto& c : constants_) {
    ret.emplace_back("const_" + c + ": .quad " + c);
  }
  return ret;
}
