#include "L1CompilerTest.h"

TEST_P(ResourceTest, FileExistsAndNotEmpty) {
  auto tc = GetParam();
  ASSERT_TRUE(fs::exists(tc.source_file_path));

  spdlog::info("Unit Test: {}",
               fs::path(tc.source_file_path).filename().string());
  auto [ret_c, path] = TestCompileSourceCode(tc.source_file_path);
  ASSERT_EQ(ret_c, tc.compiler_exit_code);

  if (tc.compiler_exit_code != 0) return;

  ASSERT_TRUE(fs::exists(path));

  auto [succ, ret_b, output] = TestRunBinary(path, tc.input);

  if (tc.expect_float_point_exception) {
    ASSERT_FALSE(succ);
    ASSERT_EQ(ret_b, -SIGFPE);
  } else if (tc.expect_segment_fault) {
    ASSERT_FALSE(succ);
    ASSERT_EQ(ret_b, -SIGSEGV);
  } else if (tc.except_abort) {
    ASSERT_FALSE(succ);
    ASSERT_EQ(ret_b, -SIGABRT);
  } else {
    ASSERT_TRUE(succ);
    ASSERT_EQ(ret_b, tc.exec_exit_code);
    if (!tc.output.empty()) ASSERT_EQ(tc.output, output);
  }
}

void PrintTo(const ResourceTestCase& tc, std::ostream* os) {
  *os << "T: " << fs::relative(tc.source_file_path, GetResourcesPath())
      << ", C: " << tc.compiler_exit_code << ", B: " << tc.exec_exit_code;
}