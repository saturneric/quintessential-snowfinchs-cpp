#include "Symbol.h"

#include "SymbolDefs.h"

struct Symbol::Impl {
  int index_;
  SymbolType type_;
  std::string name_;
  std::string value_;
  std::array<std::any, static_cast<size_t>(SymbolMetaKey::kCOUNT)> meta_data_;
};

Symbol::Symbol(SymbolType type, int index, std::string name)
    : impl_(std::make_unique<Impl>()) {
  impl_->type_ = type;
  impl_->index_ = index;
  impl_->name_ = std::move(name);
}

Symbol::~Symbol() = default;

void Symbol::SetMeta(const SymbolMetaKey& key, std::any value) {
  impl_->meta_data_[static_cast<SymbolMetaKeySizeType>(key)] = std::move(value);
}

auto Symbol::MetaData(const SymbolMetaKey& key) const -> std::any {
  return impl_->meta_data_.at(static_cast<SymbolMetaKeySizeType>(key));
}

auto Symbol::MetaRef(const SymbolMetaKey& key) -> std::any& {
  return impl_->meta_data_[static_cast<SymbolMetaKeySizeType>(key)];
}

void Symbol::RemoveMeta(const SymbolMetaKey& key) {
  impl_->meta_data_[static_cast<SymbolMetaKeySizeType>(key)] = {};
}

void Symbol::SetName(std::string name) { impl_->name_ = std::move(name); }

auto Symbol::Type() const -> SymbolType { return impl_->type_; }

auto Symbol::Index() const -> int { return impl_->index_; }

auto Symbol::Name() const -> std::string { return impl_->name_; }

auto Symbol::Value() const -> std::string { return impl_->value_; }

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

void Symbol::SetValue(std::string value) { impl_->value_ = std::move(value); }

void Symbol::Inheritance(const SymbolPtr& parent) {
  if (parent == nullptr) return;
  this->SetMeta(SymbolMetaKey::kCONTINUE_LABEL,
                parent->MetaData(SymbolMetaKey::kCONTINUE_LABEL));
  this->SetMeta(SymbolMetaKey::kBREAK_LABEL,
                parent->MetaData(SymbolMetaKey::kBREAK_LABEL));
  this->SetMeta(SymbolMetaKey::kIN_LOOP,
                parent->MetaData(SymbolMetaKey::kIN_LOOP));
}
