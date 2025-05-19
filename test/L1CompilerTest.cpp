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

auto TestRunBinary(const fs::path& path) -> std::tuple<bool, int> {
  pid_t pid = fork();
  if (pid == 0) {
    const auto fpe_trap =
        fs::current_path() / "build" / "lib" / "libfpe_trap.so";

    if (fs::exists(fpe_trap)) {
      spdlog::debug("found libfpe_trap.so: {}", fpe_trap.string());
      setenv("LD_PRELOAD", fpe_trap.c_str(), 1);
    }

    execl(path.c_str(), path.c_str(), nullptr);
    _exit(127);
  }

  int status;
  waitpid(pid, &status, 0);

  if (WIFSIGNALED(status)) {
    const auto sig = WTERMSIG(status);
    spdlog::debug("detect signal from child process: {}", sig);
    return {false, -sig};
  }

  if (WIFEXITED(status)) return {true, WEXITSTATUS(status)};
  return {false, -(1 << 16)};
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
  if (j.contains("expect_float_point_exception")) {
    j.at("expect_float_point_exception")
        .get_to(tc.expect_float_point_exception);
  } else {
    tc.expect_float_point_exception = false;
  }
  if (j.contains("expect_segment_fault")) {
    j.at("expect_segment_fault").get_to(tc.expect_segment_fault);
  } else {
    tc.expect_segment_fault = false;
  }
}
