#include "Utils.h"

#include <iomanip>

#include "model/SymbolDefs.h"

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

auto IsImmediate(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->Value() == "immediate";
}

auto Trim(const std::string& str) -> std::string {
  auto start = str.find_first_not_of(" \t\n\r");
  if (start == std::string::npos) return "";
  auto end = str.find_last_not_of(" \t\n\r");
  return str.substr(start, end - start + 1);
}
auto EndsWith(const std::string& str, const std::string& suffix) -> bool {
  if (str.size() < suffix.size()) return false;
  return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
