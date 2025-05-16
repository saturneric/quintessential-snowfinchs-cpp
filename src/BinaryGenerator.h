
#pragma once

#include "pch.h"

class BinaryGenerator {
 public:
  auto Generate(const std::string& asm_path, const std::string& binary_path)
      -> bool;

 private:
  static auto check_compiler_exists(const std::string& compiler) -> bool {
    std::string cmd = "which " + compiler + " > /dev/null 2>&1";
    return std::system(cmd.c_str()) == 0;
  }
};