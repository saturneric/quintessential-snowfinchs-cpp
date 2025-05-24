#include "Utils.h"

#include <iomanip>
#include <ostream>

#include "SymbolDefs.h"

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

auto RunOperation(const std::string& name, const Opera& f) -> bool {
  spdlog::stopwatch sw;
  auto ret = f();

  spdlog::debug("{} exec duration = {:.6f}s ret = {} ", name, sw, ret);
  return ret;
}
auto IsVariable(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->Value() == "variable";
}

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstructionPtr>& instructions) {
  for (const auto& i : instructions) {
    f << std::left << std::setw(12) << i->Op()->Name();

    if (i->DST()) {
      f << i->DST()->Name();
      if (i->SRC(0)) f << ", " << i->SRC(0)->Name();
      if (i->SRC(1)) f << ", " << i->SRC(1)->Name();
    } else if (i->SRC(0)) {
      f << i->SRC(0)->Name();
      if (i->SRC(1)) f << ", " << i->SRC(1)->Name();
    }

    f << "    // ";

    if (i->DST()) {
      f << i->DST()->Value();
      if (i->SRC(0)) f << ", " << i->SRC(0)->Value();
      if (i->SRC(1)) f << ", " << i->SRC(1)->Value();
    } else if (i->SRC(0)) {
      f << i->SRC(0)->Value();
      if (i->SRC(1)) f << ", " << i->SRC(1)->Value();
    }

    f << "\n";
    f << "% LIVE IN: ";

    for (const auto& s : i->LiveIn()) {
      assert(s != nullptr);
      if (s == nullptr) continue;
      f << s->Name() << ", ";
    }

    f << "\n";
    f << "% LIVE OUT: ";

    for (const auto& s : i->LiveOut()) {
      assert(s != nullptr);
      if (s == nullptr) continue;
      f << s->Name() << ", ";
    }

    f << "\n";
    f << "\n";
  }
}

auto SymLoc(const SymbolPtr& op) -> std::string {
  if (op == nullptr) return {};
  auto loc = op->MetaData(SymbolMetaKey::kLOCATION);
  if (!loc.has_value()) return "";
  return std::any_cast<std::string>(loc);
}
