#include "L1CompilerTest.h"

// Lab1
INSTANTIATE_TEST_SUITE_P(TestLab2Resources, ResourceTest,
                         ::testing::ValuesIn(ReadTestcases(
                             GetResourcesPath() / "test" / "lab2.json")));
