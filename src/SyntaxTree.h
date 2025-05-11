#pragma once

#include <algorithm>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "Symbol.h"

class TreeNode {
 public:
  explicit TreeNode(SymbolPtr syntax_symbol)
      : syntax_symbol_(std::move(syntax_symbol)) {}

  void AddToken(const SymbolPtr &symbol) { tokens_symbols_.push_back(symbol); }

  void SetSyntax(const SymbolPtr &symbol) { syntax_symbol_ = symbol; }

  void SetFather(TreeNode *fatherNode) {
    if (fatherNode == this) {
      throw std::runtime_error("Illegal Tree Structure");
    }
    if (std::find(fatherNode->children_.begin(), fatherNode->children_.end(),
                  this) == fatherNode->children_.end()) {
      fatherNode->children_.push_back(this);
    }
    this->father_ = fatherNode;
  }

  auto NodeType() -> std::string { return syntax_symbol_->Name(); }

  auto GetChildren() -> const std::vector<TreeNode *> & { return children_; }

  auto Tokens() -> std::vector<SymbolPtr> { return tokens_symbols_; }

  auto Syntax() -> SymbolPtr { return syntax_symbol_; }

 private:
  TreeNode *father_ = nullptr;
  std::vector<TreeNode *> children_;

  SymbolPtr syntax_symbol_;
  std::vector<SymbolPtr> tokens_symbols_;
};

using TreeNodePtr = std::shared_ptr<TreeNode>;

class SyntaxTree {
 public:
  [[nodiscard]] auto Root() const -> TreeNode * { return root_; }

  void SetRoot(TreeNode *node) { this->root_ = node; }

  void Print(std::ostream &stream);

 private:
  std::stack<int> tab_stack_;
  const int spaces_in_tab_ = 4;
  TreeNode *root_ = nullptr;

  void do_tree_node_print(TreeNode *thisNode, std::ostream &stream);
};

auto MakeSyntaxTreeNode(const std::string &name) -> std::shared_ptr<TreeNode>;