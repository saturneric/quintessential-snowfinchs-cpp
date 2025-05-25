#include "InterferenceGraph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>

namespace {
using Graph =
    boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
                          boost::property<boost::vertex_name_t, SymbolPtr>>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
}  // namespace

struct InterferenceGraph::Impl {
  Graph graph_;
  std::unordered_map<SymbolPtr, Vertex> sym_2_vtx_;
};

InterferenceGraph::InterferenceGraph()
    : impl_(std::make_unique<InterferenceGraph::Impl>()) {}

InterferenceGraph::~InterferenceGraph() = default;

void InterferenceGraph::AddVariable(const SymbolPtr& symbol) {
  // bug
  assert(symbol != nullptr);
  if (symbol == nullptr) return;

  if (impl_->sym_2_vtx_.count(symbol) == 0) {
    Vertex v = boost::add_vertex(impl_->graph_);
    boost::put(boost::vertex_name, impl_->graph_, v, symbol);
    impl_->sym_2_vtx_[symbol] = v;
  }
}

void InterferenceGraph::AddEdge(const SymbolPtr& var1, const SymbolPtr& var2) {
  assert(var1 != nullptr);
  assert(var2 != nullptr);

  AddVariable(var1);
  AddVariable(var2);
  boost::add_edge(impl_->sym_2_vtx_.at(var1), impl_->sym_2_vtx_.at(var2),
                  impl_->graph_);
}

void InterferenceGraph::RemoveEdge(const SymbolPtr& var1,
                                   const SymbolPtr& var2) {
  assert(var1 != nullptr);
  assert(var2 != nullptr);

  if ((impl_->sym_2_vtx_.count(var1) != 0U) &&
      (impl_->sym_2_vtx_.count(var2) != 0U)) {
    boost::remove_edge(impl_->sym_2_vtx_.at(var1), impl_->sym_2_vtx_.at(var2),
                       impl_->graph_);
  }
}

auto InterferenceGraph::Neighbors(const SymbolPtr& var) const
    -> std::set<SymbolPtr> {
  std::set<SymbolPtr> neighbors;
  auto it = impl_->sym_2_vtx_.find(var);
  if (it != impl_->sym_2_vtx_.end()) {
    Vertex v = it->second;
    auto [begin, end] = boost::adjacent_vertices(v, impl_->graph_);
    auto name_map = boost::get(boost::vertex_name, impl_->graph_);
    for (auto i = begin; i != end; ++i) {
      neighbors.insert(name_map[*i]);
    }
  }
  return neighbors;
}

auto InterferenceGraph::Variables() const -> std::vector<SymbolPtr> {
  std::vector<SymbolPtr> vars;
  vars.reserve(impl_->sym_2_vtx_.size());
  for (const auto& [name, _] : impl_->sym_2_vtx_) {
    vars.push_back(name);
  }
  return vars;
}

void InterferenceGraph::Print(std::ostream& os) const {
  auto name_map = boost::get(boost::vertex_name, impl_->graph_);
  for (const auto& [sym, v] : impl_->sym_2_vtx_) {
    os << sym->Name() << ": { ";
    auto [begin, end] = boost::adjacent_vertices(v, impl_->graph_);
    for (auto it = begin; it != end; ++it) {
      os << name_map[*it]->Name() << " ";
    }
    os << "}\n";
  }
}

auto InterferenceGraph::GetVertexBySymbol(const SymbolPtr& symbol) const
    -> std::optional<INFHandler> {
  auto it = impl_->sym_2_vtx_.find(symbol);
  if (it != impl_->sym_2_vtx_.end()) return it->second;
  assert(it != impl_->sym_2_vtx_.end());
  return std::nullopt;
}

auto InterferenceGraph::MCSOrder() const -> std::vector<SymbolPtr> {
  const Graph& graph = impl_->graph_;
  auto name_map = boost::get(boost::vertex_name, graph);

  std::vector<Vertex> nodes;
  for (auto [v, v_end] = boost::vertices(graph); v != v_end; ++v) {
    nodes.push_back(*v);
  }

  std::unordered_map<Vertex, int> weights;
  std::unordered_set<Vertex> visited;
  std::vector<SymbolPtr> order;

  for (auto v : nodes) {
    weights[v] = 0;
  }

  while (visited.size() < nodes.size()) {
    std::optional<Vertex> max_node;
    int max_weight = -1;

    for (auto v : nodes) {
      if (visited.count(v) == 0 && weights[v] > max_weight) {
        max_weight = weights[v];
        max_node = v;
      }
    }

    if (!max_node) break;

    visited.insert(*max_node);
    order.push_back(name_map[*max_node]);

    for (auto [adj, adj_end] = boost::adjacent_vertices(*max_node, graph);
         adj != adj_end; ++adj) {
      if (visited.count(*adj) == 0) {
        weights[*adj]++;
      }
    }
  }

  for (const auto& var : Variables()) {
    auto v_opt = GetVertexBySymbol(var);
    if (v_opt && visited.count(*v_opt) == 0) {
      order.push_back(var);
    }
  }

  return order;
}

auto InterferenceGraph::Coloring() const -> std::map<SymbolPtr, int> {
  const Graph& graph = impl_->graph_;

  std::map<Vertex, int> color_map_storage;
  auto colored_map = boost::make_assoc_property_map(color_map_storage);

  // mcs_order ->vertex
  std::vector<Vertex> vertex_order;
  for (const auto& var : MCSOrder()) {
    auto v_opt = GetVertexBySymbol(var);
    if (v_opt) vertex_order.push_back(*v_opt);
  }

  // greedy coloring using MCS order
  boost::sequential_vertex_coloring(
      graph,
      boost::make_iterator_property_map(vertex_order.begin(),
                                        boost::identity_property_map(),
                                        vertex_order.size()),
      colored_map);

  std::map<SymbolPtr, int> ret;

  auto name_map = boost::get(boost::vertex_name, graph);
  for (const auto& [v, color] : color_map_storage) {
    const auto& var = name_map[v];
    if (!var) continue;
    ret.insert({var, color});
  }

  return ret;
}
