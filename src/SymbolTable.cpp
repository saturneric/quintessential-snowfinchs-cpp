//
// Created by Administrator on 2021/4/30.
//

#include "SymbolTable.h"

SymbolTable::SymbolTable() {
  auto *symbol = new Symbol(0, "ε", true, false);
  table.insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  cache.insert(std::pair<int, Symbol *>(0, symbol));
  line.push_back(symbol);

  symbol = new Symbol(-1, "$", true, false);
  table.insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  cache.insert(std::pair<int, Symbol *>(-1, symbol));
  line.push_back(symbol);
}

auto SymbolTable::addSymbol(const std::string &name, bool terminator) -> int {
  Symbol *symbol = nullptr;

  if (name == "ε") return 0;

  if (name[0] == '@') {
    symbol = new Symbol(index, name, terminator, true);
  } else {
    symbol = new Symbol(index, name, terminator, false);
  }

  const auto &it = table.find(name);
  if (it != table.end()) {
    return it->second->index;
  }
  table.insert(std::pair<std::string, Symbol *>(symbol->name, symbol));
  cache.insert(std::pair<int, Symbol *>(symbol->index, symbol));
  line.push_back(symbol);

  index++;

  return symbol->index;
}

auto SymbolTable::getSymbol(int symbol_index) const -> const Symbol * {
  const auto &it = cache.find(symbol_index);
  if (it != cache.end()) {
    return it->second;
  }
  throw std::runtime_error("symbol " + std::to_string(symbol_index) +
                           " NOT Found");
}

auto SymbolTable::getSymbolIndex(const std::string &name) const -> int {
  const auto &it = table.find(name);
  if (it != table.end()) {
    return it->second->index;
  }
  throw std::runtime_error("symbol NOT Found");
}

void SymbolTable::modifySymbol(int idx, const std::string &name,
                               bool terminator, bool start) {
  auto it = cache.find(idx);
  if (it != cache.end()) {
    auto *p_sym = it->second;
    p_sym->name = name;
    p_sym->terminator = terminator;
    p_sym->start = start;
  }
}

auto SymbolTable::getStartSymbol() const -> const Symbol * {
  for (const auto &symbol : getAllSymbols()) {
    if (symbol->start) {
      return symbol;
    }
  }

  throw std::runtime_error("start symbol NOT Found");
}
