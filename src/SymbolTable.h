#pragma once

#include <map>
#include <string>
#include <vector>

#include "Symbol.h"

const std::string kEmptySymbol = "ε";
const std::string kStopSymbol = "$";
const std::string kEOFSymbol = "EOF";

const int kEmptySymbolId = 0;
const int kStopSymbolId = -1;
const int kEOFSymbolId = 1;

const char kStartSymbolPrefix = '@';

class SymbolTable {
  int index_ = 1 << 10;

  using SymbolNameTable = std::map<std::string, SymbolPtr>;
  std::map<SymbolType, SymbolNameTable> table_;
  std::map<int, SymbolPtr> cache_;

 public:
  SymbolTable();

  auto AddSymbol(SymbolType type, int index, const std::string &name,
                 const std::string &value = {}) -> SymbolPtr;

  auto AddSymbol(SymbolType type, const std::string &name,
                 const std::string &value = {}) -> SymbolPtr;

  [[nodiscard]] auto Symbol(int index) const -> SymbolPtr;

  [[nodiscard]] auto SymbolIndex(SymbolType type, const std::string &name) const
      -> int;

  [[nodiscard]] auto SearchSymbol(SymbolType type,
                                  const std::string &name) const -> SymbolPtr;

  void ModifySymbol(int idx, const std::string &name, bool terminator,
                    bool start);

  [[nodiscard]] auto GetAllSyntaxSymbols() const -> std::vector<SymbolPtr>;

  [[nodiscard]] auto GetSyntaxStartSymbol() const -> SymbolPtr;
};
