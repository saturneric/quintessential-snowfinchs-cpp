#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Symbol.h"

class InterferenceGraph {
 public:
  using Graph =
      boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
                            boost::property<boost::vertex_name_t, SymbolPtr>>;
  using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

  void AddVariable(const SymbolPtr& symbol);

  void AddEdge(const SymbolPtr& var1, const SymbolPtr& var2);

  void RemoveEdge(const SymbolPtr& var1, const SymbolPtr& var2);

  [[nodiscard]] auto Neighbors(const SymbolPtr& var) const
      -> std::unordered_set<SymbolPtr>;

  [[nodiscard]] auto Variables() const -> std::vector<SymbolPtr>;

  auto GetGraph() const -> const Graph&;

  auto GetVertexBySymbol(const SymbolPtr& symbol) const
      -> std::optional<Vertex>;

  void Print() const;

 private:
  Graph graph_;
  std::unordered_map<SymbolPtr, Vertex> sym_2_vtx_;
};