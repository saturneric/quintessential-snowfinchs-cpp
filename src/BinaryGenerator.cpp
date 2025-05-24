#include "BinaryGenerator.h"

#include <filesystem>

auto BinaryGenerator::Generate(const std::string& asm_path,
                               const std::string& binary_path) -> bool {
  namespace fs = std::filesystem;

  if (!fs::exists(asm_path)) {
    SPDLOG_ERROR("cannot find ASM source file: {}", asm_path);
    return false;
  }

  std::string compiler;
  if (check_compiler_exists("gcc")) {
    compiler = "gcc";
  } else if (check_compiler_exists("clang")) {
    compiler = "clang";
  } else {
    SPDLOG_ERROR("cannot find suitable compiler: gcc or clang");
    return false;
  }

  std::string cmd =
      compiler + " -no-pie -o \"" + binary_path + "\" \"" + asm_path + "\"";
  spdlog::debug("build command: {}", cmd);

  if (std::system(cmd.c_str()) != 0) {
    spdlog::error("compiler build failed, please chack source file.");
    return false;
  }

  return true;
}
