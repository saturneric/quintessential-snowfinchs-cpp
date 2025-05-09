//
// Created by Administrator on 2021/4/30.
//

#include "LR1Generator.h"

void LR1Generator::getProductions() {
  std::string temp_line;

  while (getline(input, temp_line)) {
    if (temp_line.size() > 2 && temp_line[0] != '#') {
      pool->parse_production_string_line(temp_line);
    }
  }
}
