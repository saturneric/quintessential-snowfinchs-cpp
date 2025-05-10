#pragma once

#include <algorithm>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

class TreeNode {
  const int nodeType_;
  TreeNode *father_ = nullptr;
  std::vector<std::string> info_vec_;
  std::vector<std::string> value_vec_;
  std::vector<TreeNode *> children_;

 public:
  explicit TreeNode(int nodeType) : nodeType_(nodeType) {}

  void AddInfo(const std::string &info) { info_vec_.push_back(info); }

  void AddValue(const std::string &info) { value_vec_.push_back(info); }

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

  auto NodeType() -> std::string {
    if (info_vec_.empty()) return {};
    return info_vec_.front();
  }

  auto GetChildren() -> const std::vector<TreeNode *> & { return children_; }

  auto GetInfoVec() -> const std::vector<std::string> & { return info_vec_; }

  auto GetValueVec() -> const std::vector<std::string> & { return value_vec_; }
};

class SyntaxTree {
 public:
  std::stack<int> tabStack;

  const int spacesInTab = 4;

  [[nodiscard]] auto Root() const -> TreeNode * { return root; }

  void setRoot(TreeNode *node) { this->root = node; }

  void print(std::ostream &stream);

 private:
  TreeNode *root = nullptr;

  void do_tree_node_print(TreeNode *thisNode, std::ostream &stream);
};
