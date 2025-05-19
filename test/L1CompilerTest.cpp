#include "L1CompilerTest.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "API.h"

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

auto TestCompileSourceCode(const fs::path& path) -> std::tuple<int, fs::path> {
  auto [ret, runtime_path] = MakeRuntimeDirectory();
  if (!ret) return {-1, {}};

  const auto output = runtime_path / "foo";
  int exit_code = CompileSourceCode(runtime_path, path, output, true, false);
  return {exit_code, output};
}

auto TestRunBinary(const fs::path& path) -> int {
  pid_t pid = fork();
  if (pid == 0) {
    execl(path.c_str(), path.c_str(), nullptr);
    _exit(127);
  }

  int status;
  waitpid(pid, &status, 0);
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  return -1;
}

auto ListTestSourcefiles(const fs::path& dir, const std::set<std::string>& exts)
    -> std::vector<std::string> {
  std::vector<std::string> files;
  if (!fs::exists(dir) || !fs::is_directory(dir)) {
    SPDLOG_ERROR("directory does not exist: {}", dir.string());
    return files;
  }
  for (const auto& entry : fs::directory_iterator(dir)) {
    if (!entry.is_regular_file()) continue;

    auto ext = entry.path().extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (exts.count(ext) != 0U) {
      files.push_back(entry.path().string());
    }
  }
  return files;
}

auto ReadTestcases(const std::string& filename)
    -> std::vector<ResourceTestCase> {
  std::vector<ResourceTestCase> testcases;

  std::ifstream infile(filename);
  if (!infile.is_open()) {
    SPDLOG_ERROR("Failed to open file: {}", filename);
    return testcases;
  }

  nlohmann::json j;
  infile >> j;

  try {
    testcases = j.get<std::vector<ResourceTestCase>>();
    for (auto& tc : testcases) {
      tc.source_file_path = (GetResourcesPath() / tc.source_file_path).string();
    }
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_ERROR("JSON parse error: {}", e.what());
  }

  return testcases;
}

auto GetResourcesPath() -> fs::path { return fs::current_path() / "resources"; }

void from_json(const nlohmann::json& j, ResourceTestCase& tc) {
  j.at("source_file_path").get_to(tc.source_file_path);
  j.at("compiler_exit_code").get_to(tc.compiler_exit_code);
  if (j.contains("exec_exit_code")) {
    j.at("exec_exit_code").get_to(tc.exec_exit_code);
  } else {
    tc.exec_exit_code = 0;
  }
  if (j.contains("expect_divide_by_zero")) {
    j.at("expect_divide_by_zero").get_to(tc.expect_divide_by_zero);
  } else {
    tc.expect_divide_by_zero = false;
  }
  if (j.contains("expect_overflow")) {
    j.at("expect_overflow").get_to(tc.expect_overflow);
  } else {
    tc.expect_overflow = false;
  }
}
