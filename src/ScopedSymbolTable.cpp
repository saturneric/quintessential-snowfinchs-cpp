#include "ScopedSymbolTable.h"

ScopedSymbolTable::ScopedSymbolTable(SymbolType symbol_type,
                                     SymbolTablePtr symbol_table)
    : symbol_type_(symbol_type), symbol_table_(std::move(symbol_table)) {}

void ScopedSymbolTable::EnterScope() {
  auto scope = symbol_table_->AddScope(
      scopes_stack_.size() > 0 ? scopes_stack_.top() : nullptr);
  scopes_stack_.push(scope);
}

void ScopedSymbolTable::LeaveScope() {
  if (scopes_stack_.size() > 0) scopes_stack_.pop();
}

auto ScopedSymbolTable::CurrentScope() -> ScopePtr {
  return scopes_stack_.size() > 0 ? scopes_stack_.top() : nullptr;
}

auto ScopedSymbolTable::AddSymbol(const std::string& name,
                                  const std::string& value, bool unique)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(
      symbol_type_, name, value, unique,
      CurrentScope() != nullptr ? CurrentScope()->Id() : kNonScopeId);
}

auto ScopedSymbolTable::SymbolTable() -> SymbolTablePtr {
  return symbol_table_;
}

ScopedSymbolLookUpHelper::ScopedSymbolLookUpHelper(SymbolType symbol_type,
                                                   SymbolTablePtr symbol_table)
    : symbol_type_(symbol_type), symbol_table_(std::move(symbol_table)) {}

auto ScopedSymbolLookUpHelper::LookupSymbolInScope(const ScopePtr& scope,
                                                   const std::string& name)
    -> SymbolPtr {
  if (scope == nullptr) return nullptr;
  // speed up by index
  return symbol_table_->SearchSymbol(symbol_type_, scope->Id(), name);
}

auto ScopedSymbolLookUpHelper::LookupSymbolsInScope(const ScopePtr& scope,
                                                    const std::string& name)
    -> std::vector<SymbolPtr> {
  if (scope == nullptr) return {};
  // speed up by index
  return symbol_table_->SearchSymbols(symbol_type_, scope->Id(), name);
}

auto ScopedSymbolLookUpHelper::LookupSymbol(ScopePtr scope,
                                            const std::string& name)
    -> SymbolPtr {
  // symbol has no scope
  if (scope == nullptr) return LookupSymbolWithoutScope(name);

  while (scope != nullptr) {
    auto sym = LookupSymbolInScope(scope, name);
    if (sym != nullptr) return sym;
    scope = scope->Parent();
  }
  return nullptr;
}

auto ScopedSymbolLookUpHelper::LookupSymbolWithoutScope(const std::string& name)
    -> SymbolPtr {
  return symbol_table_->SearchSymbol(symbol_type_, kNonScopeId, name);
}
