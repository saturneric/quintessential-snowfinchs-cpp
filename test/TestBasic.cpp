#include "L1CompilerTest.h"

TEST(UtilsTest, ReadTestcasesTest) {
  auto tcs =
      ReadTestcases(fs::current_path() / "resources" / "test" / "test.json");
  ASSERT_FALSE(tcs.empty());
}