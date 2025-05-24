#pragma once

#include "Scope.h"
#include "Symbol.h"

class SymbolTable {
 public:
  SymbolTable();

  ~SymbolTable();

  auto AddScope(ScopePtr parent) -> ScopePtr;

  auto SearchScope(int scope_id) -> ScopePtr;

  auto AddSymbol(SymbolType type, const std::string &name,
                 const std::string &value = {}, bool unique = true,
                 int scope_id = kNonScopeId) -> SymbolPtr;

  [[nodiscard]] auto Symbol(int index) const -> SymbolPtr;

  [[nodiscard]] auto SearchSymbols(SymbolType type, int scope) const
      -> std::vector<SymbolPtr>;

  [[nodiscard]] auto SearchSymbol(SymbolType type, int scope,
                                  const std::string &name) const -> SymbolPtr;

  [[nodiscard]] auto SearchSymbols(SymbolType type, int scope,
                                   const std::string &name) const
      -> std::vector<SymbolPtr>;

  void PrintByType(SymbolType type, std::ostream &os) const;

 private:
  struct Impl;  // PIMPL
  std::unique_ptr<Impl> impl_;
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;