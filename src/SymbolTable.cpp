#include "SymbolTable.h"

#include <memory>
#include <vector>

SymbolTable::SymbolTable() {
  auto empty = AddSymbol(SymbolType::kSYNTAX, kEmptySymbolId, kEmptySymbol);
  empty->SetTerminator(false);

  auto stop = AddSymbol(SymbolType::kSYNTAX, kStopSymbolId, kStopSymbol);
  stop->SetTerminator(true);

  auto eof = AddSymbol(SymbolType::kSYNTAX, kEOFSymbolId, kEOFSymbol);
  eof->SetTerminator(true);
}

auto SymbolTable::AddSymbol(SymbolType type, int index, const std::string &name,
                            const std::string &value) -> SymbolPtr {
  if (table_.count(type) == 0) table_[type] = {};

  const auto &it = table_[type].find(name);
  if (it != table_[type].end()) return it->second;

  auto symbol = std::make_shared<class Symbol>(type, index, name, value);
  table_[type].insert({symbol->Name(), symbol});

  cache_.insert(std::pair<int, SymbolPtr>(symbol->Index(), symbol));
  return symbol;
}

auto SymbolTable::AddSymbol(SymbolType type, const std::string &name,
                            const std::string &value) -> SymbolPtr {
  while (cache_.count(index_) != 0) index_++;
  return AddSymbol(type, index_++, name, value);
}

auto SymbolTable::AddASTSymbol(const std::string &name,
                               const std::string &value) -> SymbolPtr {
  if (table_.count(SymbolType::kAST) == 0) table_[SymbolType::kAST] = {};

  while (cache_.count(index_) != 0) index_++;
  auto symbol =
      std::make_shared<class Symbol>(SymbolType::kAST, index_++, name, value);
  table_[SymbolType::kAST].insert({symbol->Name(), symbol});

  cache_.insert(std::pair<int, SymbolPtr>(symbol->Index(), symbol));
  return symbol;
}

auto SymbolTable::Symbol(int symbol_index) const -> SymbolPtr {
  const auto &it = cache_.find(symbol_index);
  if (it != cache_.end()) return it->second;

  throw std::runtime_error("Symbol by Index" + std::to_string(symbol_index) +
                           " NOT Found");
}

auto SymbolTable::SymbolIndex(SymbolType type, const std::string &name) const
    -> int {
  const auto type_table = table_.at(type);
  const auto &it = type_table.find(name);

  if (it == type_table.end()) {
    throw std::runtime_error("Symbol by Name " + name + " NOT Found");
  }

  return it->second->Index();
}

auto SymbolTable::SearchSymbol(SymbolType type, const std::string &name) const
    -> SymbolPtr {
  const auto type_table = table_.at(type);
  const auto &it = type_table.find(name);

  if (it == type_table.end()) return nullptr;

  return it->second;
}

void SymbolTable::ModifySymbol(int idx, const std::string &name,
                               bool terminator, bool start) {
  if (cache_.count(idx) == 0) return;

  auto p_sym = cache_[idx];

  p_sym->SetName(name);
  p_sym->SetTerminator(terminator);
  p_sym->SetStartSymbol(start);
}

auto SymbolTable::GetSyntaxStartSymbol() const -> SymbolPtr {
  for (const auto &symbol : GetAllSyntaxSymbols()) {
    if (symbol->IsSyntaxStartSymbol()) return symbol;
  }

  throw std::runtime_error("Start Symbol NOT Found");
}
auto SymbolTable::GetAllSyntaxSymbols() const -> std::vector<SymbolPtr> {
  const auto type_table = table_.at(SymbolType::kSYNTAX);
  std::vector<SymbolPtr> symbols;

  for (auto const &i : type_table) symbols.push_back(i.second);
  return symbols;
}
