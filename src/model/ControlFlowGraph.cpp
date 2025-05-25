#include "ControlFlowGraph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>

#include "CFGBasicBlock.h"
#include "Utils.h"

namespace {
struct VertexProp {
  CFGBasicBlockPtr bb;
};

using CFGGraph = boost::adjacency_list<boost::vecS,            // edge list
                                       boost::vecS,            // vertex list
                                       boost::bidirectionalS,  // directed
                                       VertexProp  // vertex property
                                       >;

using Vertex = boost::graph_traits<CFGGraph>::vertex_descriptor;
using Edge = boost::graph_traits<CFGGraph>::edge_descriptor;

struct KeepReachable {
  const CFGGraph* g;
  auto operator()(Vertex v) const -> bool;
};

struct KeepEdges {
  const CFGGraph* g;
  auto operator()(Edge e) const -> bool;
};

using FilteredCFGGraph =
    boost::filtered_graph<CFGGraph, KeepEdges, KeepReachable>;

using FilteredCFGGraphPtr = std::shared_ptr<FilteredCFGGraph>;
}  // namespace

struct ControlFlowGraph::Impl {
  CFGGraph g_;
  FilteredCFGGraphPtr f_g_;
  std::map<int, Vertex> bb_map_;             // id -> vertex
  std::map<int, CFGBasicBlockPtr> id_2_bb_;  // id -> block
  std::vector<Vertex> dom_tree_;
  Vertex dom_entry_;
  bool built_filtered_graph_ = false;
  bool built_dom_tree_ = false;

  void ResetCache() {
    built_filtered_graph_ = false;
    built_dom_tree_ = false;
  }
};

auto ControlFlowGraph::AddBlock(const CFGBasicBlockPtr& bb) -> BlockHandle {
  auto v = boost::add_vertex(VertexProp{bb}, impl_->g_);
  impl_->bb_map_[bb->id] = v;
  impl_->id_2_bb_[bb->id] = bb;

  impl_->ResetCache();
  return v;
}

void ControlFlowGraph::AddEdge(BlockID from_id, BlockID to_id) {
  assert(impl_->bb_map_.count(from_id) && impl_->bb_map_.count(to_id));
  boost::add_edge(impl_->bb_map_[from_id], impl_->bb_map_[to_id], impl_->g_);

  impl_->ResetCache();
}

auto ControlFlowGraph::Blocks() const -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  for (auto vp = vertices(impl_->g_); vp.first != vp.second; ++vp.first) {
    out.push_back(impl_->g_[*vp.first].bb);
  }
  return out;
}

auto ControlFlowGraph::Successors(BlockID id) const
    -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  auto it = impl_->bb_map_.find(id);
  if (it == impl_->bb_map_.end()) return out;
  Vertex v = it->second;
  for (auto ep = out_edges(v, impl_->g_); ep.first != ep.second; ++ep.first) {
    Vertex tgt = target(*ep.first, impl_->g_);
    out.push_back(impl_->g_[tgt].bb);
  }
  return out;
}

auto ControlFlowGraph::Predecessors(BlockID id) const
    -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  auto it = impl_->bb_map_.find(id);
  if (it == impl_->bb_map_.end()) return out;
  Vertex v = it->second;
  for (auto ep = in_edges(v, impl_->g_); ep.first != ep.second; ++ep.first) {
    Vertex src = source(*ep.first, impl_->g_);
    out.push_back(impl_->g_[src].bb);
  }
  return out;
}

void ControlFlowGraph::Print(std::ostream& os) const {
  auto blocks = this->Blocks();
  for (const auto& bb : blocks) {
    os << "BasicBlock " << bb->id;
    if (!bb->label.empty()) os << " (" << bb->label << ")";
    os << ":\n";
    auto succs = Successors(bb->id);
    os << "  Successors:";
    for (const auto& succ : succs) {
      os << " " << succ->id;
      if (!succ->label.empty()) os << "(" << succ->label << ")";
    }
    os << "\n";
    auto preds = Predecessors(bb->id);
    os << "  Predecessors:";
    for (const auto& pred : preds) {
      os << " " << pred->id;
      if (!pred->label.empty()) os << "(" << pred->label << ")";
    }
    os << "\n";
    os << "  LIVE IN:";
    for (const auto& in : bb->live_in) {
      os << " " << in->Name();
    }
    os << "\n";
    os << "  LIVE OUT:";
    for (const auto& out : bb->live_out) {
      os << " " << out->Name();
    }
    os << "\n";
    os << "  DEF IN:";
    for (const auto& in : bb->def_in) {
      os << " " << in->Name();
    }
    os << "\n";
    os << "  DEF OUT:";
    for (const auto& out : bb->def_out) {
      os << " " << out->Name();
    }
    os << "\n";
    os << "  LOCAL DEF:";
    for (const auto& def : bb->def) {
      os << " " << def->Name();
    }
    os << "\n";
    os << "  LOCAL USE:";
    for (const auto& use : bb->use) {
      os << " " << use->Name();
    }
    os << "\n";
    os << "  HAS RETURN: " << bb->has_return << "\n";
    os << "  WILL RETURN: " << bb->will_return << "\n";
    os << "  REACHABLE: " << bb->reachable << "\n";
    os << "  Instructions:" << "\n";

    PrintInstructions(os, bb->Instrs());

    os << "\n";
  }
}

auto ControlFlowGraph::Instructions() const -> std::vector<IRInstructionPtr> {
  std::vector<IRInstructionPtr> ret;
  for (const auto& block : Blocks()) {
    if (!block->reachable) continue;
    ret.insert(ret.end(), block->Instrs().begin(), block->Instrs().end());
  }
  return ret;
}

auto ControlFlowGraph::VertexByBlockId(BlockID block_id) const -> Vertex {
  assert(block_id < impl_->bb_map_.size());
  return impl_->bb_map_.at(block_id);
}

auto ControlFlowGraph::BlockByBlockId(BlockID block_id) const
    -> CFGBasicBlockPtr {
  assert(block_id < impl_->id_2_bb_.size());
  return impl_->id_2_bb_.at(block_id);
}

void ControlFlowGraph::BuildFilteredGraph() {
  if (impl_->built_filtered_graph_) return;

  // use filtered_graph to connect together
  auto& full = impl_->g_;
  impl_->f_g_ = std::make_shared<FilteredCFGGraph>(full, KeepEdges{&full},
                                                   KeepReachable{&full});

  impl_->built_filtered_graph_ = true;
}

auto KeepReachable::operator()(Vertex v) const -> bool {
  return g->operator[](v).bb->reachable;
}
auto KeepEdges::operator()(Edge e) const -> bool {
  auto u = source(e, *g);
  auto v = target(e, *g);
  return g->operator[](u).bb->reachable && g->operator[](v).bb->reachable;
}

void ControlFlowGraph::BuildDominatorTree() {
  BuildFilteredGraph();

  if (impl_->built_dom_tree_) return;

  auto& g = impl_->g_;
  auto filtered_graph = impl_->f_g_;
  size_t n = num_vertices(g);
  impl_->dom_tree_.assign(n, Vertex());
  impl_->dom_entry_ = VertexByBlockId(0);

  lengauer_tarjan_dominator_tree(
      *filtered_graph, impl_->dom_entry_,
      make_iterator_property_map(impl_->dom_tree_.begin(),
                                 get(boost::vertex_index, g)));

  impl_->built_dom_tree_ = true;
}

auto ControlFlowGraph::Dominates(BlockID u_block_id, BlockID v_block_id) const
    -> bool {
  auto u_vertex = VertexByBlockId(u_block_id);
  auto v_vertex = VertexByBlockId(v_block_id);

  Vertex x = u_vertex;
  while (true) {
    if (x == v_vertex) return true;
    if (x == impl_->dom_entry_) break;
    x = impl_->dom_tree_[x];
  }
  return false;
}

ControlFlowGraph::ControlFlowGraph()
    : impl_(std::make_unique<ControlFlowGraph::Impl>()) {}

ControlFlowGraph::~ControlFlowGraph() = default;
