#pragma once

class Scope;
using ScopePtr = std::shared_ptr<Scope>;

constexpr int kNonScopeId = 0;

class Scope {
 public:
  Scope(ScopePtr parent, int id);

  void AddChild(const ScopePtr& scope);

  [[nodiscard]] auto Id() const -> int;

  [[nodiscard]] auto Parent() const -> ScopePtr;

  [[nodiscard]] auto Children() const -> const std::vector<ScopePtr>&;

 private:
  int scope_id_;
  ScopePtr parent_;

  std::vector<ScopePtr> children_;
};
