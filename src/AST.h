#pragma once

#include "SymbolTable.h"

enum class ASTNodeType : char {
  kPROGRAM,
  kASSIGN,
  kDECLARE,
  kBIN_OP,
  kUN_OP,
  kRETURN,
  kTYPE,
  kVALUE,
  kIDENT,
};

class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

class ASTNode {
 public:
  ASTNode() = default;

  ASTNode(ASTNodeType type, SymbolPtr opera);

  ASTNode(ASTNodeType type, SymbolPtr opera,
          std::initializer_list<ASTNodePtr> children);

  auto Children() -> std::vector<ASTNodePtr>;

  void AddChildren(const ASTNodePtr &child);

  auto Type() -> ASTNodeType;

  auto Operation() -> SymbolPtr;

 private:
  ASTNodeType type_;
  SymbolPtr opera_;
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
