#pragma once

#include <set>

#include "IRGenerator.h"

class ASMGenerator {
 public:
  explicit ASMGenerator(const std::vector<IRInstructionA2>& ir);

  void Generate(const std::string& path);

 private:
  std::vector<IRInstructionA2> ir_;
  std::vector<std::string> asm_;
  std::set<std::string> constants_;

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& asm_op, const IRInstructionA2& instr);

  auto is_variable(const std::string& s) -> bool;

  auto is_reg(const std::string& s) -> bool;

  static auto format_operand(const std::string& operand) -> std::string;

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;
};