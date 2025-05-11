#include "SyntaxTree.h"

#include <ostream>

void SyntaxTree::do_tree_node_print(TreeNode *node, std::ostream &stream) {
  tab_stack_.push(tab_stack_.top() + 1);

  for (int i = tab_stack_.top() * spaces_in_tab_; i > 0; i--) {
    if (i % spaces_in_tab_ == 0) {
      stream << '|';
    }
    stream << ' ';
  }

  stream << node->Syntax()->Name();

  int count = 0;
  for (const auto &token : node->Tokens()) {
    if (count++ != 0) {
      stream << ", ";
    }
    stream << "<" << token->Value() << ", " << token->Name() << ">";
  }

  stream << '\n';

  if (!node->GetChildren().empty()) {
    for (const auto &node : node->GetChildren()) {
      do_tree_node_print(node, stream);
    }
  }

  tab_stack_.pop();
}

void SyntaxTree::Print(std::ostream &stream) {
  if (this->root_ == nullptr) return;
  tab_stack_.push(-1);
  do_tree_node_print(root_, stream);
  stream.flush();
}

auto MakeSyntaxTreeNode(const std::string &name) -> std::shared_ptr<TreeNode> {
  return std::make_shared<TreeNode>(nullptr);
}