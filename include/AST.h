#pragma once

#include <iostream>
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
  ASTNode(ASTNodeType type, std::string opera)
      : type_(type), opera_(std::move(opera)) {}

  ASTNode(ASTNodeType type, std::string opera,
          std::initializer_list<ASTNodePtr> children)
      : type_(type), opera_(std::move(opera)), children_(children) {}

  auto Children() -> std::vector<ASTNodePtr> { return children_; }

  void AddChildren(const ASTNodePtr &child) {
    if (child != nullptr) children_.push_back(child);
  }

  auto Type() -> ASTNodeType { return type_; }

  auto Operation() -> std::string { return opera_; }

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

  auto Build(SyntaxTree *tree) -> bool {
    if (tree->Root() == nullptr) return false;

    root_ = do_build_tree(nullptr, tree->Root());
    return root_ == nullptr;
  }

  void Print();

  auto Root() -> ASTNodePtr { return root_; }

 private:
  std::shared_ptr<ASTNode> root_ = nullptr;
  static std::map<std::string, HandlerFunc> handler_registry;
  std::stack<int> tab_stack_;

  const int spaces_in_tab_ = 4;

  static auto do_build_tree(const ASTNodePtr &ast_node, TreeNode *syntax_node)
      -> std::shared_ptr<ASTNode> {
    auto node_type = syntax_node->NodeType();

    if (handler_registry.count(node_type) == 0U) return nullptr;
    if (handler_registry.at(node_type) == nullptr) return nullptr;

    return handler_registry[node_type](ast_node, syntax_node, do_build_tree);
  }

  void do_ast_node_print(const ASTNodePtr &node, std::ofstream &stream);
};
