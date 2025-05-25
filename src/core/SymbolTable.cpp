#include "SymbolTable.h"

// boost
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/pool/object_pool.hpp>

// std
#include <ostream>

namespace {

struct ByIndex {};
struct ByTypeName {};
struct ByTypeScopeName {};
struct ByType {};
struct ByTypeScope {};
struct ByScopeId {};

namespace multi_index = boost::multi_index;

using SymbolMultiIndexTable = multi_index::multi_index_container<
    SymbolPtr,
    multi_index::indexed_by<
        multi_index::ordered_unique<
            multi_index::tag<ByIndex>,
            multi_index::const_mem_fun<Symbol, int, &Symbol::Index>,
            std::less<>>,

        multi_index::ordered_non_unique<
            multi_index::tag<ByTypeScopeName>,
            multi_index::composite_key<
                Symbol,
                multi_index::const_mem_fun<Symbol, SymbolType, &Symbol::Type>,
                multi_index::const_mem_fun<Symbol, int, &Symbol::ScopeId>,
                multi_index::const_mem_fun<Symbol, std::string,
                                           &Symbol::Name>>>,

        multi_index::ordered_non_unique<
            multi_index::tag<ByTypeScope>,
            multi_index::composite_key<
                Symbol,
                multi_index::const_mem_fun<Symbol, SymbolType, &Symbol::Type>,
                multi_index::const_mem_fun<Symbol, int, &Symbol::ScopeId>>>,

        multi_index::ordered_non_unique<
            multi_index::tag<ByType>,
            multi_index::const_mem_fun<Symbol, SymbolType, &Symbol::Type>>>>;

using ScopeMultiIndexTable = multi_index::multi_index_container<
    ScopePtr,
    multi_index::indexed_by<multi_index::ordered_unique<
        multi_index::tag<ByScopeId>,
        multi_index::const_mem_fun<Scope, int, &Scope::Id>, std::less<>>>>;

}  // namespace

struct SymbolTable::Impl {
  int next_index_ = 1 << 8;
  int next_scope_index_ = 1 << 4;

  SymbolMultiIndexTable symbols_;
  ScopeMultiIndexTable scopes_;
  boost::object_pool<class Symbol> symbol_pool_{1 << 8};
};

SymbolTable::SymbolTable() : impl_(std::make_unique<SymbolTable::Impl>()) {}

SymbolTable::~SymbolTable() = default;

auto SymbolTable::AddSymbol(SymbolType type, const std::string &name,
                            const std::string &value, bool unique, int scope_id)
    -> SymbolPtr {
  auto &by_typename = impl_->symbols_.get<ByTypeScopeName>();
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

  auto *s = impl_->symbol_pool_.construct(type, impl_->next_index_++, name);
  auto sym =
      std::shared_ptr<class Symbol>(s, [](class Symbol *) { /* do nothing */ });

  sym->SetValue(value);
  if (scope != nullptr) sym->SetScope(scope);

  impl_->symbols_.insert(sym);
  return sym;
}

auto SymbolTable::Symbol(int index) const -> SymbolPtr {
  const auto &by_index = impl_->symbols_.get<ByIndex>();
  auto it = by_index.find(index);
  return it != by_index.end() ? *it : nullptr;
}

auto SymbolTable::SearchSymbol(SymbolType type, int scope,
                               const std::string &name) const -> SymbolPtr {
  const auto &by_scopename = impl_->symbols_.get<ByTypeScopeName>();
  auto it = by_scopename.find(boost::make_tuple(type, scope, name));
  return it != by_scopename.end() ? *it : nullptr;
}

auto SymbolTable::SearchSymbols(SymbolType type, int scope,
                                const std::string &name) const
    -> std::vector<SymbolPtr> {
  const auto &by_typescopename = impl_->symbols_.get<ByTypeScopeName>();

  auto range =
      by_typescopename.equal_range(boost::make_tuple(type, scope, name));
  std::vector<SymbolPtr> ret(range.first, range.second);
  return ret;
}

auto SymbolTable::SearchSymbols(SymbolType type, int scope) const
    -> std::vector<SymbolPtr> {
  const auto &by_typescopename = impl_->symbols_.get<ByTypeScope>();

  auto range = by_typescopename.equal_range(boost::make_tuple(type, scope));
  std::vector<SymbolPtr> ret(range.first, range.second);
  return ret;
}

auto SymbolTable::AddScope(ScopePtr parent) -> ScopePtr {
  auto scope = std::make_shared<Scope>(parent, impl_->next_scope_index_++);
  if (parent != nullptr) parent->AddChild(scope);
  impl_->scopes_.insert(scope);
  return scope;
}

auto SymbolTable::SearchScope(int scope_id) -> ScopePtr {
  if (scope_id == kNonScopeId) return nullptr;

  const auto &by_index = impl_->scopes_.get<ByScopeId>();
  auto it = by_index.find(scope_id);
  return it != by_index.end() ? *it : nullptr;
}

void SymbolTable::PrintByType(SymbolType type, std::ostream &os) const {
  const auto &idx = impl_->symbols_.get<ByType>();
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
