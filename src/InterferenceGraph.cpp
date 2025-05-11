#include "InterferenceGraph.h"

#include <iostream>

void InterferenceGraph::AddVariable(const std::string& var) { adj_list_[var]; }

void InterferenceGraph::AddEdge(const std::string& var1,
                                const std::string& var2) {
  if (var1 == var2) return;
  adj_list_[var1].insert(var2);
  adj_list_[var2].insert(var1);
}

void InterferenceGraph::RemoveEdge(const std::string& var1,
                                   const std::string& var2) {
  if (var1 == var2) return;
  adj_list_[var1].erase(var2);
  adj_list_[var2].erase(var1);
}

auto InterferenceGraph::Neighbors(const std::string& var) const
    -> std::unordered_set<std::string> {
  auto it = adj_list_.find(var);
  return it != adj_list_.end() ? it->second : std::unordered_set<std::string>{};
}

auto InterferenceGraph::Variables() const -> std::vector<std::string> {
  std::vector<std::string> vars;
  vars.reserve(adj_list_.size());
  for (const auto& [key, _] : adj_list_) {
    vars.push_back(key);
  }
  return vars;
}

void InterferenceGraph::Print() const {
  for (const auto& [var, neighbors] : adj_list_) {
    std::cout << var << " inf: ";
    for (const auto& n : neighbors) std::cout << n << " ";
    std::cout << "\n";
  }
}