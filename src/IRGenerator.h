#pragma once

#include <functional>
#include <map>
#include <vector>

#include "AST.h"

struct IRInstructionA2 {
  std::string op;
  std::string arg1;
  std::string arg2;
};

struct IRInstruction {
  std::string op;
  std::string arg1;
  std::string arg2;
  std::string result;
};

class IRGenerator {
 public:
  class Context;
  using ContextPtr = std::shared_ptr<Context>;
  using ExpHandler = std::function<std::string(Context*, const ASTNodePtr&)>;

  class Context {
   public:
    explicit Context(IRGenerator* ig, ExpHandler handler);

    auto ExpRoute(const ASTNodePtr& node) -> std::string;

    auto MappingInnerVariable(const std::string& v = {}) -> std::string;

    void AppendInstruction(const IRInstruction& i);

    static auto SelectInstruction(const std::string& operation) -> std::string;

    [[nodiscard]] auto Instructions() const -> std::vector<IRInstruction>;

    void EnterScope();

    void LeaveScope();

   private:
    IRGenerator* ig_;
    ExpHandler handler_;

    int scope_ = 0;
    std::stack<int> s_t_var_idx_;
    int temp_variable_idx_ = 0;
    std::vector<IRInstruction> instructions_;
  };

  explicit IRGenerator(SymbolTablePtr symbol_table);

  auto Generate(const AST& tree) -> std::vector<IRInstructionA2>;

  void Print3Addr(const std::string& path);

  void Print2Addr(const std::string& path);

 private:
  static std::map<ASTNodeType, ExpHandler> exp_handler_resiter;
  ContextPtr ctx_;
  SymbolTablePtr symbol_table_;
  int in_var_idx_ = 1;

  std::vector<IRInstructionA2> instructions_2_addr_;
  std::vector<IRInstruction> instructions_ssa_;
  std::map<std::string, int> variable_version_;
  std::map<std::string, std::string> variable_name_ssa_;

  static auto select_instruction(const std::string& operation) -> std::string;

  auto do_ir_generate(Context*, const ASTNodePtr& node) -> std::string;

  auto get_ssa_name(const std::string& var, bool is_def) -> std::string;

  void convert2_ssa();

  void convert_instructions();

  auto mapping_inner_variable(int scope, const std::string& v) -> std::string;
};