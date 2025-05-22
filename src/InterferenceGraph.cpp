#include "InterferenceGraph.h"

#include <iostream>

void InterferenceGraph::AddVariable(const SymbolPtr& symbol) {
  // bug
  assert(symbol != nullptr);
  if (symbol == nullptr) return;

  if (sym_2_vtx_.count(symbol) == 0) {
    Vertex v = boost::add_vertex(graph_);
    boost::put(boost::vertex_name, graph_, v, symbol);
    sym_2_vtx_[symbol] = v;
  }
}

void InterferenceGraph::AddEdge(const SymbolPtr& var1, const SymbolPtr& var2) {
  assert(var1 != nullptr);
  assert(var2 != nullptr);

  AddVariable(var1);
  AddVariable(var2);
  boost::add_edge(sym_2_vtx_.at(var1), sym_2_vtx_.at(var2), graph_);
}

void InterferenceGraph::RemoveEdge(const SymbolPtr& var1,
                                   const SymbolPtr& var2) {
  assert(var1 != nullptr);
  assert(var2 != nullptr);

  if ((sym_2_vtx_.count(var1) != 0U) && (sym_2_vtx_.count(var2) != 0U)) {
    boost::remove_edge(sym_2_vtx_.at(var1), sym_2_vtx_.at(var2), graph_);
  }
}

auto InterferenceGraph::Neighbors(const SymbolPtr& var) const
    -> std::unordered_set<SymbolPtr> {
  std::unordered_set<SymbolPtr> neighbors;
  auto it = sym_2_vtx_.find(var);
  if (it != sym_2_vtx_.end()) {
    Vertex v = it->second;
    auto [begin, end] = boost::adjacent_vertices(v, graph_);
    auto name_map = boost::get(boost::vertex_name, graph_);
    for (auto i = begin; i != end; ++i) {
      neighbors.insert(name_map[*i]);
    }
  }
  return neighbors;
}

auto InterferenceGraph::Variables() const -> std::vector<SymbolPtr> {
  std::vector<SymbolPtr> vars;
  vars.reserve(sym_2_vtx_.size());
  for (const auto& [name, _] : sym_2_vtx_) {
    vars.push_back(name);
  }
  return vars;
}

void InterferenceGraph::Print() const {
  auto name_map = boost::get(boost::vertex_name, graph_);
  for (const auto& [name, v] : sym_2_vtx_) {
    std::cout << name << ": { ";
    auto [begin, end] = boost::adjacent_vertices(v, graph_);
    for (auto it = begin; it != end; ++it) {
      std::cout << name_map[*it] << " ";
    }
    std::cout << "}\n";
  }
}
auto InterferenceGraph::GetGraph() const -> const Graph& { return graph_; }

auto InterferenceGraph::GetVertexBySymbol(const SymbolPtr& symbol) const
    -> std::optional<Vertex> {
  auto it = sym_2_vtx_.find(symbol);
  if (it != sym_2_vtx_.end()) return it->second;
  assert(it != sym_2_vtx_.end());
  return std::nullopt;
}
