//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_SYMBOL_H
#define SYNTAXPARSER_SYMBOL_H

#include <string>

struct Symbol {
  const int index;
  std::string name;
  bool terminator;
  bool start;

  Symbol(int index, std::string name, bool terminator, bool start)
      : index(index),
        name(std::move(name)),
        terminator(terminator),
        start(start) {}
};

#endif  // SYNTAXPARSER_SYMBOL_H
