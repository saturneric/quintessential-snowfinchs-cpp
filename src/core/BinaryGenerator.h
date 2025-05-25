
#pragma once

class BinaryGenerator {
 public:
  auto Generate(const std::string& asm_path, const std::string& binary_path)
      -> bool;

 private:
  static auto check_compiler_exists(const std::string& compiler) -> bool;
};