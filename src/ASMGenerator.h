#pragma once

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "IRGenerator.h"

class InterferenceGraph {
 public:
  void AddVariable(const std::string& var);

  void AddEdge(const std::string& var1, const std::string& var2);

  void RemoveEdge(const std::string& var1, const std::string& var2);

  [[nodiscard]] auto Neighbors(const std::string& var) const
      -> std::unordered_set<std::string>;

  [[nodiscard]] auto Variables() const -> std::vector<std::string>;

  void Print() const;

 private:
  std::unordered_map<std::string, std::unordered_set<std::string>> adj_list_;
};

class ASMGenerator {
 public:
  explicit ASMGenerator(const std::vector<IRInstructionA2>& ir);

  void Generate(const std::string& path);

  void PrintIR(const std::string& path);

 private:
  std::vector<IRInstructionA2> ir_;
  std::vector<IRInstructionA2> ir_opt_;
  std::vector<std::string> asm_;
  std::set<std::string> constants_;
  InterferenceGraph inf_graph_;
  std::set<std::string> vars_;
  std::vector<std::string> mcs_order_;
  std::unordered_map<std::string, std::string> reg_alloc_;

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& asm_op, const IRInstructionA2& instr);

  auto is_variable(const std::string& s) -> bool;

  void optimums();

  static auto is_reg(const std::string& s) -> bool;

  static auto format_operand(const std::string& operand) -> std::string;

  void alloc_reg();

  void build_inf_graph();

  void mcs();

  void all_vars();

  void generate_gcc_asm(const std::string& path);

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;
};