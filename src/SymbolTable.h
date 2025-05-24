#pragma once

// boost
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/pool/object_pool.hpp>

// std
#include <functional>

#include "Scope.h"
#include "Symbol.h"

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

class SymbolTable {
 public:
  SymbolTable();

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
  int next_index_ = 1 << 8;
  int next_scope_index_ = 1 << 4;

  SymbolMultiIndexTable symbols_;
  ScopeMultiIndexTable scopes_;
  boost::object_pool<class Symbol> symbol_pool_{1 << 8};
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;