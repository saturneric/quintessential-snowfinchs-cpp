//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_PRODUCTION_H
#define SYNTAXPARSER_PRODUCTION_H

#include <vector>

struct Production {
  const int index;
  const int left;
  const std::vector<int> right;

  Production(int index, int left, std::vector<int> right)
      : index(index), left(left), right(std::move(right)) {}
};

#endif  // SYNTAXPARSER_PRODUCTION_H
