#pragma once

#include <vector>

enum class ASTNodeType : char {
  kASSIGN,
  kDECLARE,
  kBINARY_OPERA,
  kUNARY_OPERA,
  kRETURN,
};

class ASTNode {
  int type_;
  std::string opera_;
  std::vector<ASTNode *> children_;
};