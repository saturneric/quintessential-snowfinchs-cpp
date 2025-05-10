#include "ASMGenerator.h"

#include <fstream>
#include <set>
#include <sstream>

ASMGenerator::ASMGenerator(const std::vector<IRInstructionA2>& ir) : ir_(ir) {}

void ASMGenerator::Generate(const std::string& path) {
  std::set<std::string> data_vars;

  for (const auto& instr : ir_) {
    if (is_variable(instr.arg1)) data_vars.insert(instr.arg1);
    if (is_variable(instr.arg2)) data_vars.insert(instr.arg2);
  }

  // .data
  std::vector<std::string> lines;
  lines.emplace_back("section .data");
  for (const auto& var : data_vars) {
    lines.emplace_back("  " + var + " dd 0");
  }

  // .text
  lines.emplace_back("\nsection .text");
  lines.emplace_back("global main\nmain:");

  asm_.clear();
  for (const auto& instr : ir_) translate(instr);
  lines.insert(lines.end(), asm_.begin(), asm_.end());

  std::ofstream f(path);
  for (const auto& line : lines) f << line << "\n";
  f.close();
}

void ASMGenerator::translate(const IRInstructionA2& i) {
  std::ostringstream oss;

  if (i.op == "mov") {
    oss << "mov dword [" << i.arg1 << "], " << format_operand(i.arg2);
    asm_.push_back(oss.str());
  } else if (i.op == "add") {
    emit_binary_op("add", i);
  } else if (i.op == "sub") {
    emit_binary_op("sub", i);
  } else if (i.op == "mul") {
    emit_binary_op("imul", i);
  } else if (i.op == "div") {
    asm_.push_back("mov eax, " + format_operand(i.arg1));
    asm_.push_back("cdq");
    asm_.push_back("idiv " + format_operand(i.arg2));
    asm_.push_back("mov dword [" + i.arg1 + "], eax");
  } else if (i.op == "neg") {
    asm_.push_back("mov eax, " + format_operand(i.arg1));
    asm_.push_back("neg eax");
    asm_.push_back("mov dword [" + i.arg1 + "], eax");
  } else if (i.op == "rtn") {
    asm_.push_back("mov eax, " + format_operand(i.arg1));
    asm_.push_back("ret");
  } else {
    asm_.push_back("; unsupported op: " + i.op);
  }
}

void ASMGenerator::emit_binary_op(const std::string& asm_op,
                                  const IRInstructionA2& instr) {
  asm_.push_back("mov eax, " + format_operand(instr.arg1));
  asm_.push_back(asm_op + " eax, " + format_operand(instr.arg2));
  asm_.push_back("mov dword [" + instr.arg1 + "], eax");
}

auto ASMGenerator::format_operand(const std::string& operand) -> std::string {
  if (!operand.empty() && (std::isdigit(operand[0]) != 0)) {
    return operand;
  }
  return "dword [" + operand + "]";
}

auto ASMGenerator::is_variable(const std::string& s) -> bool {
  if (s.empty()) return false;
  if (std::isdigit(s[0]) != 0) return false;
  if (s[0] == '-') return false;  // e.g., -5
  return true;
}
