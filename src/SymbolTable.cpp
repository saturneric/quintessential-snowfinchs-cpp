#include "SymbolTable.h"

SymbolTable::SymbolTable() {
  AddSymbol(kEmptySymbolId, kEmptySymbol, true, false);
  AddSymbol(kStopSymbolId, kStopSymbol, true, false);
  AddSymbol(kEOFSymbolId, kEOFSymbol, true, false);
}

auto SymbolTable::AddSymbol(int index, const std::string &name, bool terminator,
                            bool start) -> int {
  auto *symbol = new Symbol(index, name, terminator, start);
  const auto &it = table.find(name);
  if (it != table.end()) return it->second->index;

  table.insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  cache.insert(std::pair<int, Symbol *>(symbol->index, symbol));
  line.push_back(symbol);
  return symbol->index;
}

auto SymbolTable::AddSymbol(const std::string &name, bool terminator) -> int {
  Symbol *symbol = nullptr;

  const auto &it = table.find(name);
  if (it != table.end()) return it->second->index;

  return AddSymbol(index++, name, terminator, name[0] == kStartSymbolPrefix);
}

auto SymbolTable::GetSymbol(int symbol_index) const -> const Symbol * {
  const auto &it = cache.find(symbol_index);
  if (it != cache.end()) return it->second;

  throw std::runtime_error("Symbol " + std::to_string(symbol_index) +
                           " NOT Found");
}

auto SymbolTable::GetSymbolIndex(const std::string &name) const -> int {
  const auto &it = table.find(name);
  if (it != table.end()) return it->second->index;

  throw std::runtime_error("Symbol " + name + " NOT Found");
}

void SymbolTable::ModifySymbol(int idx, const std::string &name,
                               bool terminator, bool start) {
  auto it = cache.find(idx);
  if (it != cache.end()) {
    auto *p_sym = it->second;
    p_sym->name = name;
    p_sym->terminator = terminator;
    p_sym->start = start;
  }
}

auto SymbolTable::GetStartSymbol() const -> const Symbol * {
  for (const auto &symbol : GetAllSymbols()) {
    if (symbol->start) return symbol;
  }
  throw std::runtime_error("Start Symbol NOT Found");
}
auto SymbolTable::GetAllSymbols() const -> const std::vector<const Symbol *> & {
  return line;
}
