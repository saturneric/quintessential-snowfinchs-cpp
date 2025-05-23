#pragma once

#include <any>
#include <utility>

#include "Scope.h"

enum class SymbolType : uint8_t {
  kNONE,
  kAST,
  kDEFINE,
  kIR,
};

using SymbolMetaKeySizeType = uint8_t;

enum class SymbolMetaKey : SymbolMetaKeySizeType {
  kNONE = 0,
  kSCOPE,
  kTYPE,
  kHAS_INIT,
  kIN_LOOP,
  kIS_SPILLED,
  kIN_REGISTER,
  kIN_STACK,
  kLOCATION,
  kSSA_VERSION,
  kSSA_LATEST_SYM,
  kSSA_ORIGIN_SYM,
  kCONTINUE_LABEL,
  kBREAK_LABEL,
  kDEF_SYMBOL,
  kWILL_RETURN,
  kWILL_BREAK,
  kCOUNT,
};

class Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;

class Symbol {
 public:
  Symbol(SymbolType type, int index, std::string name);

  [[nodiscard]] auto Type() const -> SymbolType;

  [[nodiscard]] auto Index() const -> int;

  [[nodiscard]] auto Name() const -> std::string;

  [[nodiscard]] auto Value() const -> std::string;

  [[nodiscard]] auto Scope() const -> ScopePtr;

  [[nodiscard]] auto ScopeId() const -> int;

  [[nodiscard]] auto MetaData(const SymbolMetaKey& key) const -> std::any;

  void SetName(std::string name);

  void SetValue(std::string value);

  void SetScope(ScopePtr scope);

  auto MetaRef(const SymbolMetaKey& key) -> std::any&;

  void SetMeta(const SymbolMetaKey& key, std::any value);

  void RemoveMeta(const SymbolMetaKey& key);

  void Inheritance(const SymbolPtr& parent);

 private:
  int index_;
  SymbolType type_;
  std::string name_;
  std::string value_;
  std::array<std::any, static_cast<size_t>(SymbolMetaKey::kCOUNT)> meta_data_;
};

template <typename T>
auto MetaGet(const SymbolPtr& s, SymbolMetaKey key, T default_value = T{})
    -> T {
  auto val = s->MetaData(key);
  if (!val.has_value()) return default_value;
  return std::any_cast<T>(val);
}

template <typename T>
void MetaSet(const SymbolPtr& s, SymbolMetaKey key, T value) {
  s->MetaRef(key) = std::move(value);
}
