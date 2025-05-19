#include "L1CompilerTest.h"

// Lab1
INSTANTIATE_TEST_SUITE_P(TestLab1Resources, ResourceTest,
                         ::testing::ValuesIn(ReadTestcases(
                             GetResourcesPath() / "test" / "lab1.json")));

// Demonstrate some basic assertions.
TEST(Lab1Test, BasicAssertions) {}
