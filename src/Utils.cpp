#include "Utils.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

auto SafeParseInt(const std::string& text, int& result, int base) -> bool {
  try {
    size_t idx = 0;
    unsigned long long uvalue = std::stoull(text, &idx, base);
    if (idx != text.size()) return false;

    if (base == 16 && uvalue > 0xFFFFFFFFULL) return false;

    if (uvalue >
        static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
      auto val = static_cast<int32_t>(uvalue);
      result = val;
      return true;
    }

    result = static_cast<int>(uvalue);
    return true;

  } catch (...) {
    return false;
  }
}