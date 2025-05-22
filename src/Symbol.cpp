#include "Symbol.h"

#include <utility>

Symbol::Symbol(SymbolType type, int index, std::string name)
    : type_(type), index_(index), name_(std::move(name)) {}

void Symbol::SetMeta(const SymbolMetaKey& key, std::any value) {
  meta_data_[static_cast<SymbolMetaKeySizeType>(key)] = std::move(value);
}

auto Symbol::MetaData(const SymbolMetaKey& key) const -> std::any {
  return meta_data_.at(static_cast<SymbolMetaKeySizeType>(key));
}

auto Symbol::MetaRef(const SymbolMetaKey& key) -> std::any& {
  return meta_data_[static_cast<SymbolMetaKeySizeType>(key)];
}

void Symbol::RemoveMeta(const SymbolMetaKey& key) {
  meta_data_[static_cast<SymbolMetaKeySizeType>(key)] = {};
}

void Symbol::SetName(std::string name) { name_ = std::move(name); }

auto Symbol::Type() const -> SymbolType { return type_; }

auto Symbol::Index() const -> int { return index_; }

auto Symbol::Name() const -> std::string { return name_; }

auto Symbol::Value() const -> std::string { return value_; }

auto Symbol::Scope() const -> ScopePtr {
  auto scope = MetaData(SymbolMetaKey::kSCOPE);
  return scope.has_value() ? std::any_cast<ScopePtr>(scope) : nullptr;
}

void Symbol::SetScope(ScopePtr scope) {
  MetaRef(SymbolMetaKey::kSCOPE) = std::move(scope);
}

auto Symbol::ScopeId() const -> int {
  auto scope = Scope();
  return scope != nullptr ? scope->Id() : kNonScopeId;
}

void Symbol::SetValue(std::string value) { value_ = std::move(value); }

void Symbol::Inheritance(const SymbolPtr& parent) {
  if (parent == nullptr) return;
  this->SetMeta(SymbolMetaKey::kCONTINUE_LABEL,
                parent->MetaData(SymbolMetaKey::kCONTINUE_LABEL));
  this->SetMeta(SymbolMetaKey::kBREAK_LABEL,
                parent->MetaData(SymbolMetaKey::kBREAK_LABEL));
}
