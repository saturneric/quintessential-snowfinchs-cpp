#include "L1CompilerTest.h"

// Lab1
INSTANTIATE_TEST_SUITE_P(TestLab3Resources, ResourceTest,
                         ::testing::ValuesIn(ReadTestcases(
                             GetResourcesPath() / "test" / "lab3.json")));
