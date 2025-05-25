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
  if (check_compiler_exists("clang")) {
    compiler = "clang";
  } else if (check_compiler_exists("gcc")) {
    compiler = "gcc";
  } else {
    SPDLOG_ERROR("cannot find suitable compiler: gcc or clang");
    return false;
  }

  const auto cmd =
      compiler + " -static -o \"" + binary_path + "\" \"" + asm_path + "\"";
  spdlog::debug("build command: {}", cmd);

  if (std::system(cmd.c_str()) != 0) {
    spdlog::error("compiler build failed, please check source file.");
    return false;
  }

  return true;
}

auto BinaryGenerator::check_compiler_exists(const std::string& compiler)
    -> bool {
  std::string cmd = "which " + compiler + " > /dev/null 2>&1";
  return std::system(cmd.c_str()) == 0;
}
