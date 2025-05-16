#include "L1CompilerTest.h"

class ResourceTest : public ::testing::TestWithParam<ResourceTestCase> {};

TEST_P(ResourceTest, FileExistsAndNotEmpty) {
  ResourceTestCase tc = GetParam();
  ASSERT_TRUE(fs::exists(tc.source_file_path));

  SPDLOG_INFO("TC: SP: {}, CC: {}, BC: {}",
              fs::relative(tc.source_file_path, GetResourcesPath()).string(),
              tc.compiler_exit_code, tc.exec_exit_code);

  auto [ret, path] = TestCompileSourceCode(tc.source_file_path);
  ASSERT_EQ(ret, tc.compiler_exit_code);

  if (tc.compiler_exit_code != 0) return;

  ASSERT_TRUE(fs::exists(path));

  ret = TestRunBinary(path);
  SPDLOG_INFO("TC BP: {}, Ret: {}", path.string(), ret);
  ASSERT_EQ(ret, tc.exec_exit_code);
}

inline void PrintTo(const ResourceTestCase& tc, std::ostream* os) {
  *os << "T: " << fs::relative(tc.source_file_path, GetResourcesPath())
      << ", C: " << tc.compiler_exit_code << ", B: " << tc.exec_exit_code;
}

// Lab1
INSTANTIATE_TEST_SUITE_P(TestLab1Resources, ResourceTest,
                         ::testing::ValuesIn(ReadTestcases(
                             GetResourcesPath() / "test" / "lab1.txt")));