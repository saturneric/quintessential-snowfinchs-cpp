#include "L1CompilerTest.h"

// Lab4
INSTANTIATE_TEST_SUITE_P(TestLab4Resources, ResourceTest,
                         ::testing::ValuesIn(ReadTestcases(
                             GetResourcesPath() / "test" / "lab4.json")));
