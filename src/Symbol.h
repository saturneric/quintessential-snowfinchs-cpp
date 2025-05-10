#pragma once

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
