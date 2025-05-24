#include "SymbolTable.h"

#include <memory>

SymbolTable::SymbolTable() = default;

auto SymbolTable::AddSymbol(SymbolType type, const std::string &name,
                            const std::string &value, bool unique, int scope_id)
    -> SymbolPtr {
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

  auto *s = symbol_pool_.construct(type, next_index_++, name);
  auto sym =
      std::shared_ptr<class Symbol>(s, [](class Symbol *) { /* do nothing */ });

  sym->SetValue(value);
  if (scope != nullptr) sym->SetScope(scope);

  symbols_.insert(sym);
  return sym;
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

auto SymbolTable::SearchSymbols(SymbolType type, int scope) const
    -> std::vector<SymbolPtr> {
  const auto &by_typescopename = symbols_.get<ByTypeScope>();

  auto range = by_typescopename.equal_range(boost::make_tuple(type, scope));
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

void SymbolTable::PrintByType(SymbolType type, std::ostream &os) const {
  const auto &idx = symbols_.get<ByType>();
  for (auto it = idx.lower_bound(type), end = idx.upper_bound(type); it != end;
       ++it) {
    if (*it) {
      const auto &sym = *it;
      os << "Index: " << sym->Index() << " Name: " << sym->Name()
         << " Value: " << sym->Value() << " ScopeID: " << sym->ScopeId()
         << "\n";
    }
  }
}
