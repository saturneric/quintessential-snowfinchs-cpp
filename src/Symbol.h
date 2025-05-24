#pragma once

#include <any>

#include "Scope.h"

enum class SymbolType : uint8_t;
enum class SymbolMetaKey : uint8_t;

class Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;

class Symbol {
 public:
  Symbol(SymbolType type, int index, std::string name);

  ~Symbol();

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
  struct Impl;  // PIMPL
  std::unique_ptr<Impl> impl_;
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
