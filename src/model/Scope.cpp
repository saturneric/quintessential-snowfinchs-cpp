#include "Scope.h"

Scope::Scope(ScopePtr parent, int id)
    : scope_id_(id), parent_(std::move(parent)) {}

void Scope::AddChild(const ScopePtr& scope) { children_.push_back(scope); }

auto Scope::Id() const -> int { return scope_id_; }

auto Scope::Parent() const -> ScopePtr { return parent_; }

auto Scope::Children() const -> const std::vector<ScopePtr>& {
  return children_;
}
