#pragma once

#include <map>
#include <utility>
#include <vector>

#include "SymbolTable.h"
#include "SyntaxTree.h"

enum class ASTNodeType : char {
  kPROGRAM,
  kASSIGN,
  kDECLARE,
  kBIN_OP,
  kUN_OP,
  kRETURN,
  kTYPE,
  kVALUE,
};

class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

class ASTNode {
 public:
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
  using RouterFunc = std::function<ASTNodePtr(ASTNodePtr, TreeNode *)>;
  using HandlerFunc = std::function<ASTNodePtr(
      ASTNodePtr, TreeNode *, const SymbolTablePtr &, const RouterFunc &)>;

  explicit AST(std::shared_ptr<SymbolTable> symbol_table);

  void LoadBinding(const std::string &path);

  auto Build(const SyntaxTree &tree) -> bool;

  void Print(const std::string &path);

  [[nodiscard]] auto Root() const -> ASTNodePtr;

 private:
  static std::map<std::string, HandlerFunc> handler_registry;
  std::shared_ptr<SymbolTable> symbol_table_;
  std::shared_ptr<ASTNode> root_ = nullptr;
  std::map<std::string, HandlerFunc> syntax_symbol_to_handler_;
  std::stack<int> tab_stack_;

  const int spaces_in_tab_ = 4;

  auto do_build_tree(const ASTNodePtr &ast_node, TreeNode *syntax_node)
      -> std::shared_ptr<ASTNode>;

  void do_ast_node_print(const ASTNodePtr &node, std::ofstream &stream);
};
