#pragma once

#include <utility>

#include "SymbolTable.h"

enum class ASTNodeType : char {
  kPROGRAM,
  kBLOCK,
  kASSIGN,
  kDECLARE,
  kBIN_OP,
  kUN_OP,
  kRETURN,
  kTYPE,
  kVALUE,
  kIDENT,
  kIF,
  kCOND_EXP,
  kWHILE,
  kBREAK,
  kCONTINUE,
};

enum class ASTNodeTag : uint8_t {
  kNONE = 0,
  kINIT,
  kCOND,
  kSTEP,
  kBODY,
};

class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

class ASTNode {
 public:
  ASTNode() = default;

  ASTNode(ASTNodeType type, SymbolPtr symbol);

  ASTNode(ASTNodeType type, SymbolPtr symbol,
          std::initializer_list<ASTNodePtr> children);

  auto Parent() -> ASTNode *;

  auto Children() -> std::vector<ASTNodePtr>;

  void AddChild(const ASTNodePtr &child, ASTNodeTag tag = ASTNodeTag::kNONE);

  auto Type() -> ASTNodeType;

  auto Tag() -> ASTNodeTag;

  auto Symbol() -> SymbolPtr;

 private:
  ASTNodeType type_;
  ASTNodeTag tag_;
  SymbolPtr symbol_;
  ASTNode *parent_ = nullptr;
  std::vector<ASTNodePtr> children_;
};

class AST {
 public:
  explicit AST(std::shared_ptr<SymbolTable> symbol_table);

  void SetRoot(const ASTNodePtr &root);

  void Print(const std::string &path);

  [[nodiscard]] auto Root() const -> ASTNodePtr;

 private:
  std::shared_ptr<SymbolTable> symbol_table_;
  std::shared_ptr<ASTNode> root_ = nullptr;

  std::stack<int> tab_stack_;

  const int spaces_in_tab_ = 4;
  void do_ast_node_print(const ASTNodePtr &node, std::ofstream &stream);
};
