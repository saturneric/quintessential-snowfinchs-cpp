#include "L1CompilerTest.h"

TEST(UtilsTest, ReadTestcasesTest) {
  auto tcs =
      ReadTestcases(fs::current_path() / "resources" / "test" / "lab1.txt");
  ASSERT_FALSE(tcs.empty());
}