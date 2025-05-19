#include <cfenv>
#include <csignal>
#include <cstdio>
#include <cstdlib>

namespace {
__attribute__((constructor)) void EnableFPETrap() {
  feenableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
}
};  // namespace