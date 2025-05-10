#pragma once

#include <vector>

struct Production {
  const int index;
  const int left;
  const std::vector<int> right;

  Production(int index, int left, std::vector<int> right);
};

using ProductionPtr = std::shared_ptr<Production>;