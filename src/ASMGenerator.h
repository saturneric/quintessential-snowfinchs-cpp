#pragma once

#include "IRGenerator.h"

class ASMGenerator {
 public:
  explicit ASMGenerator(const std::vector<IRInstructionA2>& ir);

  void Generate(const std::string& path);

 private:
  std::vector<IRInstructionA2> ir_;
  std::vector<std::string> asm_;

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& asm_op, const IRInstructionA2& instr);

  auto is_variable(const std::string& s) -> bool;

  static auto format_operand(const std::string& operand) -> std::string;
};