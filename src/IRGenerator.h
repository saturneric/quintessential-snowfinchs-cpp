#pragma once

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
    explicit Context(ExpHandler handler);

    auto ExpRoute(const ASTNodePtr& node) -> std::string;

    auto NewTempVariable() -> std::string;

    void AppendInstruction(const IRInstruction& i);

    static auto SelectInstruction(const std::string& operation) -> std::string;

    [[nodiscard]] auto Instructions() const -> std::vector<IRInstruction>;

   private:
    int temp_variable_counter_ = 0;
    std::vector<IRInstruction> instructions_;
    ExpHandler handler_;
  };

  IRGenerator();

  auto Generate(const AST& tree) -> std::vector<IRInstructionA2>;

  void Print3Addr(const std::string& path);

  void Print2Addr(const std::string& path);

 private:
  static std::map<ASTNodeType, ExpHandler> exp_handler_resiter;
  std::vector<IRInstructionA2> instructions_2_addr_;
  std::vector<IRInstruction> instructions_ssa_;
  std::map<std::string, int> variable_version_;
  std::map<std::string, std::string> variable_name_ssa_;
  ContextPtr ctx_;

  static auto select_instruction(const std::string& operation) -> std::string;

  auto do_ir_generate(Context*, const ASTNodePtr& node) -> std::string;

  auto get_ssa_name(const std::string& var, bool is_def) -> std::string;

  void convert2_ssa();

  void convert_instructions();
};