#pragma once

namespace fs = std::filesystem;

struct ResourceTestCase {
  std::string source_file_path;
  int compiler_exit_code;
  int exec_exit_code;
  bool expect_float_point_exception = false;
  bool expect_segment_fault = false;
  bool except_abort = false;
  std::string input;
  std::string output;
};

// for json to ResourceTestCase using by json library
void from_json(const nlohmann::json& j, ResourceTestCase& tc);  // NOLINT

class ResourceTest : public ::testing::TestWithParam<ResourceTestCase> {};

void PrintTo(const ResourceTestCase& tc, std::ostream* os);

// Developer defined functions

auto TestCompileSourceCode(const fs::path& path) -> std::tuple<int, fs::path>;

auto TestRunBinary(const fs::path& path, const std::string& input)
    -> std::tuple<bool, int, std::string>;

auto ListTestSourcefiles(const fs::path& dir, const std::set<std::string>& exts)
    -> std::vector<std::string>;

auto ReadTestcases(const std::string& filename)
    -> std::vector<ResourceTestCase>;

auto GetResourcesPath() -> fs::path;
