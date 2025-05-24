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
                        const std::vector<IRInstructionPtr>& ir2);

  void Generate(const std::string& path);

  void PrintFinalIR(const std::string& path);

  void PrintIFG(const std::string& path);

 private:
  bool r32_;
  int stack_offset_dt_ = r32_ ? 4 : 8;
  int stack_offset_ = 0;
  std::string suffix_ = r32_ ? "l" : "q";
  std::string acc_reg_ = r32_ ? "%eax" : "%rax";
  std::string acc_reg_low_ = "%al";

  std::string count_reg_64_ = "%rcx";
  std::string count_reg_ = r32_ ? "%ecx" : "%rcx";
  std::string count_reg_low_ = "%cl";

  std::string rem_reg_ = r32_ ? "%edx" : "%rdx";

  const std::string op_mov_ = "mov" + suffix_;
  const std::string op_push_ = "push";
  const std::string op_pop_ = "pop";

  const std::string sp_ = "%rsp";  // compiling on 64 bit platform!
  const std::string bp_ = "%rbp";  // compiling on 64 bit platform!

  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  InterferenceGraph inf_graph_;
  std::vector<IRInstructionPtr> ir2_;
  std::vector<IRInstructionPtr> ir_final_;
  std::set<SymbolPtr> constants_;
  std::set<SymbolPtr> vars_;
  std::set<SymbolPtr> spilled_vars_;

  void gen_final_asm_source(const std::string& path);

  auto translate(const std::vector<IRInstructionPtr>& ir)
      -> std::vector<std::string>;

  void emit_mov_op(std::vector<std::string>& fins, const std::string& op,
                   const IRInstruction& i);

  void emit_binary_op(std::vector<std::string>& fins, const std::string& op,
                      const IRInstruction& instr);

  void emit_cmp_op(std::vector<std::string>& fins, const std::string& op,
                   const IRInstruction& i);

  void emit_logic_op(std::vector<std::string>& fins, const std::string& op,
                     const IRInstruction& i);

  void emit_unary_op(std::vector<std::string>& fins, const std::string& op,
                     const IRInstruction& i);

  void emit_spz_op(std::vector<std::string>& fins, const std::string& op,
                   const IRInstruction& i);

  void emit_jmp_op(std::vector<std::string>& fins, const std::string& op,
                   const IRInstruction& i);

  void optimums();

  auto format_operand(const SymbolPtr& operand, bool force_use_loc = false)
      -> std::string;

  void alloc_reg();

  void build_inf_graph();

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  auto map_reg(const std::string& name) -> SymbolPtr;

  void generate_gcc_asm(const std::string& path,
                        const std::vector<std::string>& ins);

  void handle_spling_var();

  auto alloc_stack_for_immediate(const SymbolPtr& val) -> std::string;

  static auto gen_data_var_immediate_label(const SymbolPtr& val) -> std::string;

  [[nodiscard]] auto generate_data_segment() const -> std::vector<std::string>;

  auto alloc_stack_memory() -> std::vector<std::string>;

  auto map_op(const std::string& name) -> SymbolPtr;
};