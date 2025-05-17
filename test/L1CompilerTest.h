#pragma once

namespace fs = std::filesystem;

struct ResourceTestCase {
  std::string source_file_path;
  int compiler_exit_code;
  int exec_exit_code;
};

auto TestCompileSourceCode(const fs::path& path) -> std::tuple<int, fs::path>;

auto TestRunBinary(const fs::path& path) -> int;

auto ListTestSourcefiles(const fs::path& dir, const std::set<std::string>& exts)
    -> std::vector<std::string>;

auto ReadTestcases(const std::string& filename)
    -> std::vector<ResourceTestCase>;

auto GetResourcesPath() -> fs::path;
