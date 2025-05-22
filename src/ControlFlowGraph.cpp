#include "ControlFlowGraph.h"

auto ControlFlowGraph::AddBlock(const CFGBasicBlockPtr& bb) -> Vertex {
  Vertex v = boost::add_vertex(VertexProp{bb}, g_);
  bb_map_[bb->id] = v;
  return v;
}

void ControlFlowGraph::AddEdge(int from_id, int to_id) {
  assert(bb_map_.count(from_id) && bb_map_.count(to_id));
  boost::add_edge(bb_map_[from_id], bb_map_[to_id], g_);
}

auto ControlFlowGraph::Blocks() const -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  for (auto vp = vertices(g_); vp.first != vp.second; ++vp.first) {
    out.push_back(g_[*vp.first].bb);
  }
  return out;
}

auto ControlFlowGraph::Successors(int id) const
    -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  auto it = bb_map_.find(id);
  if (it == bb_map_.end()) return out;
  Vertex v = it->second;
  for (auto ep = out_edges(v, g_); ep.first != ep.second; ++ep.first) {
    Vertex tgt = target(*ep.first, g_);
    out.push_back(g_[tgt].bb);
  }
  return out;
}

auto ControlFlowGraph::Predecessors(int id) const
    -> std::vector<CFGBasicBlockPtr> {
  std::vector<CFGBasicBlockPtr> out;
  auto it = bb_map_.find(id);
  if (it == bb_map_.end()) return out;
  Vertex v = it->second;
  for (auto ep = in_edges(v, g_); ep.first != ep.second; ++ep.first) {
    Vertex src = source(*ep.first, g_);
    out.push_back(g_[src].bb);
  }
  return out;
}

auto ControlFlowGraph::Graph() -> CFGGraph& { return g_; }

auto ControlFlowGraph::Graph() const -> const CFGGraph& { return g_; }

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
  }
}
