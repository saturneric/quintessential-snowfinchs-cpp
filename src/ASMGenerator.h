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
  int stack_offset_dt_ = r32_ ? 4 : 8;
  int stack_offset_ = 0;
  const std::string sp_ = "%rsp";  // compiling on 64 bit platform!
  const std::string bp_ = "%rbp";  // compiling on 64 bit platform!

  InterferenceGraph inf_graph_;
  std::vector<IRInstructionA2> ir_;
  std::vector<IRInstructionA2> ir_opt_;
  std::vector<std::string> asm_;
  std::set<std::string> constants_;
  std::set<std::string> vars_;
  std::vector<std::string> mcs_order_;
  std::unordered_map<std::string, std::string> reg_alloc_;
  std::unordered_map<std::string, int> const_stack_slots_;

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& asm_op, const IRInstructionA2& instr);

  void optimums();

  auto format_operand(const std::string& operand) -> std::string;

  void alloc_reg();

  void build_inf_graph();

  void mcs();

  void generate_gcc_asm(const std::string& path);

  auto bounded_reg(const std::string& operand) -> std::string;

  auto handle_spling_var(const std::set<std::string>& spilled_vars)
      -> std::vector<IRInstructionA2>;

  auto alloc_stack_for_immediate(const std::string& val) -> std::string;

  static auto gen_data_var_immediate_label(const std::string& val)
      -> std::string;

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;

  void alloc_stack_memory();
};