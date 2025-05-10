#pragma once

#include <iostream>
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
  void Generate(const AST& tree) {
    auto node = tree.Root();
    if (node == nullptr) return;

    do_generate(node);
  }

  void Print(const std::string& path) {
    if (instructions_.empty()) return;

    std::ofstream f(path);

    for (const auto& i : instructions_) {
      f << std::left << std::setw(6) << i.op;

      if (!i.result.empty()) {
        f << i.result;
        if (!i.arg1.empty()) f << ", " << i.arg1;
        if (!i.arg2.empty()) f << ", " << i.arg2;
      } else if (!i.arg1.empty()) {
        f << i.arg1;
        if (!i.arg2.empty()) f << ", " << i.arg2;
      }

      f << "\n";
    }

    f.close();
  }

 private:
  std::vector<IRInstruction> instructions_;
  int temp_counter_ = 0;

  void do_generate(const ASTNodePtr& node) {
    if (node == nullptr) return;

    switch (node->Type()) {
      case ASTNodeType::kASSIGN: {
        if (node->Children().size() < 2) break;
        std::string rhs = do_generate_expr(node->Children()[1]);
        std::string lhs = do_generate_expr(node->Children()[0]);
        instructions_.push_back({"mov", rhs, "", lhs});
        break;
      }
      case ASTNodeType::kDECLARE: {
        if (node->Children().size() < 3) break;
        std::string rhs = do_generate_expr(node->Children()[2]);
        std::string lhs = do_generate_expr(node->Children()[1]);
        instructions_.push_back({"mov", rhs, "", lhs});
        break;
      }
      case ASTNodeType::kRETURN: {
        if (node->Children().empty()) break;
        std::string ret_val = do_generate_expr(node->Children()[0]);
        instructions_.push_back({"rtn", ret_val, "", ""});
        break;
      }
      default: {
        for (const auto& child : node->Children()) {
          if (child != nullptr) do_generate(child);
        }
        break;
      }
    }
  }

  auto do_generate_expr(const ASTNodePtr& node) -> std::string {
    if (node == nullptr) return {};

    if (node->Type() == ASTNodeType::kVALUE) {
      return node->Operation();
    }

    if (node->Type() == ASTNodeType::kUN_OP) {
      std::string operand = do_generate_expr(node->Children()[0]);
      std::string temp = new_temp();

      auto instruction = select_instruction(node->Operation());
      instructions_.push_back({instruction, operand, "", temp});
      return temp;
    }

    if (node->Type() == ASTNodeType::kBIN_OP) {
      if (node->Children().empty()) return "";

      std::string lhs = do_generate_expr(node->Children()[0]);
      std::string rhs = do_generate_expr(node->Children()[1]);
      std::string temp = new_temp();

      auto instruction = select_instruction(node->Operation());
      instructions_.push_back({instruction, lhs, rhs, temp});
      return temp;
    }

    return "";
  }

  auto new_temp() -> std::string {
    return "t" + std::to_string(temp_counter_++);
  }

  static auto select_instruction(const std::string& operation) -> std::string {
    if (kIrInstructions.count(operation) == 0) {
      std::cout << "Cannot select instruction for operation: " << operation
                << "\n";
      return {};
    }
    return kIrInstructions.at(operation);
  }
};