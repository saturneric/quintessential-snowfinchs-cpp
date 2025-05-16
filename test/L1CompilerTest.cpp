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

  SPDLOG_INFO("read test cases from: {}", filename);

  std::ifstream infile(filename);
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    ResourceTestCase tc;
    if (!(iss >> tc.source_file_path >> tc.compiler_exit_code >>
          tc.exec_exit_code)) {
      // parse error, skip or handle as needed
      continue;
    }

    tc.source_file_path = GetResourcesPath() / tc.source_file_path;
    testcases.push_back(tc);
  }
  return testcases;
}

auto GetResourcesPath() -> fs::path { return fs::current_path() / "resources"; }
