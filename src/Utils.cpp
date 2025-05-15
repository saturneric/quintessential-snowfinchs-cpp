#include "Utils.h"

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

auto SaftAtoi(const std::string& str) -> int {
  const char* ptr = str.c_str();
  int result = 0;

  while (std::isspace(static_cast<unsigned char>(*ptr)) != 0) {
    ++ptr;
  }

  char sign = *ptr;
  if (*ptr == '-' || *ptr == '+') {
    ++ptr;
  }

  int ch;
  while ((ch = static_cast<unsigned char>(*ptr)) != 0) {
    if (std::isdigit(ch) == 0) break;

    ch -= '0';

    if ((result < INT_MIN / 10) ||
        (result == INT_MIN / 10 && ch > -(INT_MIN % 10))) {
      throw std::out_of_range("Integer overflow");
    }

    result *= 10;
    result -= ch;
    ++ptr;
  }

  if (sign != '-') {
    if (result < -INT_MAX) {
      throw std::out_of_range("Integer overflow");
    }
    result = -result;
  }

  return result;
}
}  // namespace

auto SafeParseInt(const std::string& text, int& result) -> bool {
  if (text.empty()) return false;

  // assert: text is not negative

  size_t pos = 0;

  int base = 10;
  if (pos + 2 <= text.size() && text[pos] == '0' &&
      (text[pos + 1] == 'x' || text[pos + 1] == 'X')) {
    base = 16;
    pos += 2;
  } else if (pos + 1 < text.size() && text[pos] == '0') {
    base = 8;
    pos += 1;
  }

  std::string digits = text.substr(pos);
  if (digits.empty()) return false;

  size_t idx = 0;
  unsigned long long u = 0;
  try {
    u = std::stoull(digits, &idx, base);
  } catch (...) {
    return false;
  }
  if (idx != digits.size()) return false;

  if (base == 16) {
    if (u > 0xFFFFFFFFULL) return false;
    auto v32 = static_cast<uint32_t>(u);
    int32_t sv = *reinterpret_cast<int32_t*>(&v32);
    result = sv;
    return true;
  }

  if (base == 10) {
    if (u > 0x80000000ULL) return false;  // Allow up to 2^31 = 2147483648
    result = static_cast<int>(u);  // 2^31 == -2^31 when interpreted as int
    return true;
  }

  // Octal or others, fit into int range
  if (u > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
    return false;
  }
  result = static_cast<int>(u);
  return true;
}