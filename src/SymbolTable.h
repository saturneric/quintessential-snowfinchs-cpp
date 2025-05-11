#pragma once

#include <map>
#include <string>

#include "Symbol.h"

class SymbolTable {
  int index_ = 1 << 10;

  using SymbolNameTable = std::map<std::string, SymbolPtr>;
  std::map<SymbolType, SymbolNameTable> table_;
  std::map<int, SymbolPtr> cache_;
  std::map<int, SymbolNameTable> semantic_table_;

 public:
  SymbolTable();

  auto AddSymbol(SymbolType type, int index, const std::string &name,
                 const std::string &value = {}) -> SymbolPtr;

  auto AddSymbol(SymbolType type, const std::string &name,
                 const std::string &value = {}) -> SymbolPtr;

  auto AddASTSymbol(const std::string &name, const std::string &value)
      -> SymbolPtr;

  auto AddSemanticSymbol(int scope, const std::string &name,
                         const std::string &value) -> SymbolPtr;

  [[nodiscard]] auto SearchSemanticSymbol(int scope,
                                          const std::string &name) const
      -> SymbolPtr;

  [[nodiscard]] auto Symbol(int index) const -> SymbolPtr;

  [[nodiscard]] auto SymbolIndex(SymbolType type, const std::string &name) const
      -> int;

  [[nodiscard]] auto SearchSymbol(SymbolType type,
                                  const std::string &name) const -> SymbolPtr;

  void ModifySymbol(int idx, const std::string &name, bool terminator,
                    bool start);
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;