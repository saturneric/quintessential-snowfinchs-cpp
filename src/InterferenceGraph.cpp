#include "InterferenceGraph.h"

#include <iostream>

void InterferenceGraph::AddVariable(const std::string& var) {
  if (name_to_vertex_.count(var) == 0) {
    Vertex v = boost::add_vertex(graph_);
    boost::put(boost::vertex_name, graph_, v, var);
    name_to_vertex_[var] = v;
  }
}

void InterferenceGraph::AddEdge(const std::string& var1,
                                const std::string& var2) {
  AddVariable(var1);
  AddVariable(var2);
  boost::add_edge(name_to_vertex_.at(var1), name_to_vertex_.at(var2), graph_);
}

void InterferenceGraph::RemoveEdge(const std::string& var1,
                                   const std::string& var2) {
  if ((name_to_vertex_.count(var1) != 0U) &&
      (name_to_vertex_.count(var2) != 0U)) {
    boost::remove_edge(name_to_vertex_.at(var1), name_to_vertex_.at(var2),
                       graph_);
  }
}

auto InterferenceGraph::Neighbors(const std::string& var) const
    -> std::unordered_set<std::string> {
  std::unordered_set<std::string> neighbors;
  auto it = name_to_vertex_.find(var);
  if (it != name_to_vertex_.end()) {
    Vertex v = it->second;
    auto [begin, end] = boost::adjacent_vertices(v, graph_);
    auto name_map = boost::get(boost::vertex_name, graph_);
    for (auto i = begin; i != end; ++i) {
      neighbors.insert(name_map[*i]);
    }
  }
  return neighbors;
}

auto InterferenceGraph::Variables() const -> std::vector<std::string> {
  std::vector<std::string> vars;
  vars.reserve(name_to_vertex_.size());
  for (const auto& [name, _] : name_to_vertex_) {
    vars.push_back(name);
  }
  return vars;
}

void InterferenceGraph::Print() const {
  auto name_map = boost::get(boost::vertex_name, graph_);
  for (const auto& [name, v] : name_to_vertex_) {
    std::cout << name << ": { ";
    auto [begin, end] = boost::adjacent_vertices(v, graph_);
    for (auto it = begin; it != end; ++it) {
      std::cout << name_map[*it] << " ";
    }
    std::cout << "}\n";
  }
}
auto InterferenceGraph::GetGraph() const -> const Graph& { return graph_; }

auto InterferenceGraph::GetVertexByName(const std::string& name) const
    -> std::optional<Vertex> {
  auto it = name_to_vertex_.find(name);
  if (it != name_to_vertex_.end()) return it->second;
  return std::nullopt;
}
