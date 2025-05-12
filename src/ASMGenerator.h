#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "IRGenerator.h"
#include "InterferenceGraph.h"

class ASMGenerator {
 public:
  explicit ASMGenerator(bool r32, const std::vector<IRInstructionA2>& ir);

  void Generate(const std::string& path);

  void PrintIR(const std::string& path);

 private:
  bool r32_;
  InterferenceGraph inf_graph_;
  std::vector<IRInstructionA2> ir_;
  std::vector<IRInstructionA2> ir_opt_;
  std::vector<std::string> asm_;
  std::set<std::string> constants_;
  std::set<std::string> vars_;
  std::vector<std::string> mcs_order_;
  std::unordered_map<std::string, std::string> reg_alloc_;

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& asm_op, const IRInstructionA2& instr);

  auto is_variable(const std::string& s) const -> bool;

  void optimums();

  static auto is_reg(const std::string& s) -> bool;

  auto format_operand(const std::string& operand) -> std::string;

  void alloc_reg();

  void build_inf_graph();

  void mcs();

  void all_vars();

  void generate_gcc_asm(const std::string& path);

  auto bounded_reg(const std::string& operand) -> std::string;

  auto handle_spling_var(const std::set<std::string>& spilled_vars)
      -> std::vector<IRInstructionA2>;

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;
};