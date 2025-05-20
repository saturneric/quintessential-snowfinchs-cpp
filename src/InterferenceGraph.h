#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class InterferenceGraph {
 public:
  using Graph =
      boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
                            boost::property<boost::vertex_name_t, std::string>>;
  using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

  void AddVariable(const std::string& var);

  void AddEdge(const std::string& var1, const std::string& var2);

  void RemoveEdge(const std::string& var1, const std::string& var2);

  [[nodiscard]] auto Neighbors(const std::string& var) const
      -> std::unordered_set<std::string>;

  [[nodiscard]] auto Variables() const -> std::vector<std::string>;

  void Print() const;

 private:
  Graph graph_;
  std::unordered_map<std::string, Vertex> name_to_vertex_;
};