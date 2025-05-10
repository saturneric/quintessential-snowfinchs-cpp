#pragma once

#include <map>
#include <vector>

#include "AST.h"

struct IRInstruction {
  std::string op;
  std::string arg1;
  std::string arg2;
  std::string result;
};

const std::map<std::string, std::string> kIrInstructions = {
    {"MULT", "mul"},
    {"SUB", "sub"},
    {"SLASH", "div"},
};

class IRGenerator {
 public:
  void Generate(const AST& tree);

  void Print(const std::string& path);

 private:
  std::vector<IRInstruction> instructions_;
  int temp_counter_ = 0;

  void do_generate(const ASTNodePtr& node);

  auto do_generate_expr(const ASTNodePtr& node) -> std::string;

  auto new_temp() -> std::string;

  static auto select_instruction(const std::string& operation) -> std::string;
};