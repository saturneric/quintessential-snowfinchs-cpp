#pragma once

#include <map>
#include <vector>

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
  ASTNode(ASTNodeType type, std::string opera);

  ASTNode(ASTNodeType type, std::string opera,
          std::initializer_list<ASTNodePtr> children);

  auto Children() -> std::vector<ASTNodePtr>;

  void AddChildren(const ASTNodePtr &child);

  auto Type() -> ASTNodeType;

  auto Operation() -> std::string;

 private:
  ASTNodeType type_;
  std::string opera_;
  std::vector<ASTNodePtr> children_;
};

class AST {
 public:
  using RouterFunc = std::function<ASTNodePtr(ASTNodePtr, TreeNode *)>;
  using HandlerFunc =
      std::function<ASTNodePtr(ASTNodePtr, TreeNode *, const RouterFunc &)>;

  AST() = default;

  auto Build(const SyntaxTree &tree) -> bool;

  void Print(const std::string &path);

  [[nodiscard]] auto Root() const -> ASTNodePtr;

 private:
  std::shared_ptr<ASTNode> root_ = nullptr;
  static std::map<std::string, HandlerFunc> handler_registry;
  std::stack<int> tab_stack_;

  const int spaces_in_tab_ = 4;

  static auto do_build_tree(const ASTNodePtr &ast_node, TreeNode *syntax_node)
      -> std::shared_ptr<ASTNode>;

  void do_ast_node_print(const ASTNodePtr &node, std::ofstream &stream);
};
