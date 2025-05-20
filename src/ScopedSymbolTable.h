#pragma once

#include <utility>

#include "SymbolTable.h"

class ScopedSymbolTable {
 public:
  ScopedSymbolTable(SymbolType symbol_type, SymbolTablePtr symbol_table);

  void EnterScope();

  void LeaveScope();

  auto CurrentScope() -> ScopePtr;

  auto AddSymbol(const std::string& name, const std::string& value, bool unique)
      -> SymbolPtr;

  auto SymbolTable() -> SymbolTablePtr;

 private:
  SymbolType symbol_type_;
  SymbolTablePtr symbol_table_;
  std::stack<ScopePtr> scopes_stack_;
};

using ScopedSymbolTablePtr = std::shared_ptr<ScopedSymbolTable>;

class ScopedSymbolLookUpHelper {
 public:
  ScopedSymbolLookUpHelper(SymbolType symbol_type, SymbolTablePtr symbol_table);

  auto LookupSymbolInScope(const ScopePtr& scope, const std::string& name)
      -> SymbolPtr;

  auto LookupSymbolsInScope(const ScopePtr& scope, const std::string& name)
      -> std::vector<SymbolPtr>;

  auto LookupSymbol(ScopePtr scope, const std::string& name) -> SymbolPtr;

 private:
  SymbolType symbol_type_;
  SymbolTablePtr symbol_table_;
};

using ScopedSymbolTablePtr = std::shared_ptr<ScopedSymbolTable>;
