//
// Created by Administrator on 2021/5/12.
//

#include "SyntaxTree.h"

void SyntaxTree::do_tree_node_print(TreeNode *thisNode, std::ofstream &stream) {
  tabStack.push(tabStack.top() + 1);

  for (int i = tabStack.top() * spacesInTab; i > 0; i--) {
    if (i % spacesInTab == 0) {
      stream << '|';
    }
    stream << ' ';
  }

  int count = 0;
  for (const auto &info : thisNode->GetInfoVec()) {
    if (count++ != 0) {
      if (count != 2) {
        stream << ", ";
      }
      stream << info;
    } else {
      stream << info << '<';
    }
  }

  stream << '>';

  stream << "[";

  count = 0;
  for (const auto &value : thisNode->GetValueVec()) {
    if (count++ != 0) {
      stream << ", ";
    }
    stream << value;
  }

  stream << ']';

  stream << '\n';

  if (!thisNode->GetChildren().empty()) {
    for (const auto &node : thisNode->GetChildren()) {
      do_tree_node_print(node, stream);
    }
  }

  tabStack.pop();
}

void SyntaxTree::print(std::ofstream &stream) {
  if (this->root == nullptr) return;

  tabStack.push(-1);

  do_tree_node_print(root, stream);
}
