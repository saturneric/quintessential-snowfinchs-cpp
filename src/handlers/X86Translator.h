#pragma once

#include "core/ASMGenerator.h"

class X86Translator : public Translator {
 public:
  explicit X86Translator(bool r32);

  auto Optimums(const std::vector<IRInstructionPtr>& ir)
      -> std::vector<IRInstructionPtr> override;

  void HandleVariables(const std::set<SymbolPtr>& vars) override;

  auto GenerateTextSection(const std::vector<IRInstructionPtr>& ir)
      -> std::vector<std::string> override;

  auto GenerateDataSegment() -> std::vector<std::string> override;

  void Reset() override;

  [[nodiscard]] auto AvailableRegisters() const
      -> std::vector<std::string> override;

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

  std::vector<SymbolPtr> in_data_sec_vars_;
  std::set<SymbolPtr> vars_;

  auto translate(const std::vector<IRInstructionPtr>& ir)
      -> std::vector<std::string>;

  auto format_operand(const SymbolPtr& operand, bool force_use_loc = false)
      -> std::string;

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

  void emit_func_op(std::vector<std::string>& out, const std::string& op,
                    const IRInstruction& i);

  auto alloc_stack_for_immediate(const SymbolPtr& val) -> std::string;

  static auto gen_data_var_immediate_label(const SymbolPtr& val) -> std::string;

  auto alloc_stack_memory() -> std::vector<std::string>;
};
