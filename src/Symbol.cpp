#include "Symbol.h"

#include <utility>

Symbol::Symbol(SymbolType type, int index, ScopePtr scope, std::string name,
               std::string value)
    : type_(type),
      index_(index),
      scope_(std::move(scope)),
      name_(std::move(name)),
      value_(std::move(value)) {}

void Symbol::SetMetaData(const std::string& key, std::any value) {
  meta_data_[key] = std::move(value);
}

auto Symbol::MetaData(const std::string& key) const -> std::any {
  if (meta_data_.count(key) == 0) return {};
  return meta_data_.at(key);
}

auto Symbol::RefMetaData(const std::string& key) -> std::any& {
  return meta_data_[key];
}

void Symbol::RemoveMetaData(const std::string& key) { meta_data_.erase(key); }

void Symbol::SetName(std::string name) { name_ = std::move(name); }

auto Symbol::Type() const -> SymbolType { return type_; }

auto Symbol::Index() const -> int { return index_; }

auto Symbol::Name() const -> std::string { return name_; }

auto Symbol::Value() const -> std::string { return value_; }

auto Symbol::Scope() const -> ScopePtr { return scope_; }

void Symbol::SetScope(ScopePtr scope) { scope_ = std::move(scope); }

auto Symbol::ScopeId() const -> int {
  return scope_ != nullptr ? scope_->Id() : kNonScopeId;
}
