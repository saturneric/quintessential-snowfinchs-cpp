#include "Utils.h"

auto RunOperation(const std::string& name, const Opera& f) -> bool {
  spdlog::stopwatch sw;
  auto ret = f();

  spdlog::debug("{} exec duration = {:.6f}s ret = {} ", name, sw, ret);
  return ret;
}