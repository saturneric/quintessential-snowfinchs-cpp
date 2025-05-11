#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class InterferenceGraph {
 public:
  void AddVariable(const std::string& var);

  void AddEdge(const std::string& var1, const std::string& var2);

  void RemoveEdge(const std::string& var1, const std::string& var2);

  [[nodiscard]] auto Neighbors(const std::string& var) const
      -> std::unordered_set<std::string>;

  [[nodiscard]] auto Variables() const -> std::vector<std::string>;

  void Print() const;

 private:
  std::unordered_map<std::string, std::unordered_set<std::string>> adj_list_;
};