#include "SymbolTable.h"

#include <memory>

SymbolTable::SymbolTable() = default;

auto SymbolTable::AddSymbol(SymbolType type, int index, const std::string &name,
                            const std::string &value, bool unique, int scope_id)
    -> SymbolPtr {
  auto &by_index = symbols_.get<ByIndex>();
  if (by_index.find(index) != by_index.end()) {
    return *by_index.find(index);
  }

  auto &by_typename = symbols_.get<ByTypeScopeName>();
  if (unique && by_typename.find(boost::make_tuple(type, scope_id, name)) !=
                    by_typename.end()) {
    return *by_typename.find(boost::make_tuple(type, scope_id, name));
  }

  auto scope = SearchScope(scope_id);

  // bug
  assert(scope_id == kNonScopeId || scope != nullptr);
  if (scope_id != kNonScopeId && scope == nullptr) {
    SPDLOG_ERROR("can't find scope by id: {}", scope_id);
    return nullptr;
  }

  auto symbol = std::make_shared<class Symbol>(type, index, scope, name, value);
  symbols_.insert(symbol);
  return symbol;
}

auto SymbolTable::AddSymbol(SymbolType type, const std::string &name,
                            const std::string &value, bool unique, int scope_id)
    -> SymbolPtr {
  while (symbols_.get<ByIndex>().find(next_index_) !=
         symbols_.get<ByIndex>().end()) {
    ++next_index_;
  }
  return AddSymbol(type, next_index_++, name, value, unique, scope_id);
}

auto SymbolTable::Symbol(int index) const -> SymbolPtr {
  const auto &by_index = symbols_.get<ByIndex>();
  auto it = by_index.find(index);
  return it != by_index.end() ? *it : nullptr;
}

auto SymbolTable::SearchSymbol(SymbolType type, int scope,
                               const std::string &name) const -> SymbolPtr {
  const auto &by_scopename = symbols_.get<ByTypeScopeName>();
  auto it = by_scopename.find(boost::make_tuple(type, scope, name));
  return it != by_scopename.end() ? *it : nullptr;
}

auto SymbolTable::SearchSymbols(SymbolType type, int scope,
                                const std::string &name) const
    -> std::vector<SymbolPtr> {
  const auto &by_typescopename = symbols_.get<ByTypeScopeName>();

  auto range =
      by_typescopename.equal_range(boost::make_tuple(type, scope, name));
  std::vector<SymbolPtr> ret(range.first, range.second);
  return ret;
}

auto SymbolTable::AddScope(ScopePtr parent) -> ScopePtr {
  auto scope = std::make_shared<Scope>(parent, next_scope_index_++);
  if (parent != nullptr) parent->AddChild(scope);
  scopes_.insert(scope);
  return scope;
}

auto SymbolTable::SearchScope(int scope_id) -> ScopePtr {
  if (scope_id == kNonScopeId) return nullptr;

  const auto &by_index = scopes_.get<ByScopeId>();
  auto it = by_index.find(scope_id);
  return it != by_index.end() ? *it : nullptr;
}
