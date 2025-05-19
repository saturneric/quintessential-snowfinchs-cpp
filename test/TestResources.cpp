#include "L1CompilerTest.h"

TEST_P(ResourceTest, FileExistsAndNotEmpty) {
  auto tc = GetParam();
  ASSERT_TRUE(fs::exists(tc.source_file_path));

  spdlog::info("Test: {}", fs::path(tc.source_file_path).filename().string());
  auto [ret, path] = TestCompileSourceCode(tc.source_file_path);
  ASSERT_EQ(ret, tc.compiler_exit_code);

  if (tc.compiler_exit_code != 0) return;

  ASSERT_TRUE(fs::exists(path));

  ret = TestRunBinary(path);
  ASSERT_EQ(ret, tc.exec_exit_code);
}

void PrintTo(const ResourceTestCase& tc, std::ostream* os) {
  *os << "T: " << fs::relative(tc.source_file_path, GetResourcesPath())
      << ", C: " << tc.compiler_exit_code << ", B: " << tc.exec_exit_code;
}