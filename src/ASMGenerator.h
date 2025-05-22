#pragma once

#include <set>
#include <string>
#include <vector>

#include "IRInstruction.h"
#include "InterferenceGraph.h"
#include "ScopedSymbolTable.h"
#include "SymbolTable.h"

class ASMGenerator {
 public:
  explicit ASMGenerator(SymbolTablePtr symbol_table, bool r32,
                        const std::vector<IRInstructionA2Ptr>& ir2);

  void Generate(const std::string& path);

  void PrintFinalIR(const std::string& path);

  void PrintIFG(const std::string& path);

 private:
  bool r32_;
  int stack_offset_dt_ = r32_ ? 4 : 8;
  int stack_offset_ = 0;
  const std::string sp_ = "%rsp";  // compiling on 64 bit platform!
  const std::string bp_ = "%rbp";  // compiling on 64 bit platform!

  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  InterferenceGraph inf_graph_;
  std::vector<IRInstructionA2Ptr> ir2_;
  std::vector<IRInstructionA2Ptr> ir_final_;
  std::vector<std::string> asm_;
  std::set<SymbolPtr> constants_;
  std::set<SymbolPtr> vars_;
  std::vector<SymbolPtr> mcs_order_;
  std::set<SymbolPtr> spilled_vars_;

  void gen_final_asm_source(const std::string& path);

  void translate(const IRInstructionA2& instr);

  void emit_binary_op(const std::string& op, const IRInstructionA2& instr);

  void emit_cmp_op(const std::string& op, const IRInstructionA2& i);

  void emit_logic_op(const std::string& op, const IRInstructionA2& i);

  void emit_unary_op(const std::string& op, const IRInstructionA2& i);

  void optimums();

  auto format_operand(const SymbolPtr& operand, bool force_use_loc = false)
      -> std::string;

  void alloc_reg();

  void build_inf_graph();

  void mcs();

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  auto map_reg(const std::string& name) -> SymbolPtr;

  void generate_gcc_asm(const std::string& path);

  void handle_spling_var();

  void alloc_stack_for_immediate(const SymbolPtr& val);

  static auto gen_data_var_immediate_label(const SymbolPtr& val) -> std::string;

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;

  void alloc_stack_memory();

  auto map_op(const std::string& name) -> SymbolPtr;
};