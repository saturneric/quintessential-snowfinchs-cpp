#include "Utils.h"

#include <limits>
#include <stdexcept>

auto SafeParseInt(const std::string& text, int& result, int base) -> bool {
  try {
    size_t idx;
    long long value = std::stoll(text, &idx, base);

    if (idx != text.size()) return false;
    if (value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max()) {
      return false;
    }

    result = static_cast<int>(value);
    return true;
  } catch (const std::invalid_argument&) {
    return false;
  } catch (const std::out_of_range&) {
    return false;
  }
}