#include "ControlFlowGraph.h"

#include "Utils.h"

auto ControlFlowGraph::AddBlock(const CFGBasicBlockPtr& bb) -> Vertex {
  Vertex v = boost::add_vertex(VertexProp{bb}, g_);
  bb_map_[bb->id] = v;
  id_2_bb_[bb->id] = bb;
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

    if (bb->IR2Mode()) {
      PrintInstructionA2s(os, bb->Instr2As());
    } else {
      PrintInstructions(os, bb->Instrs());
    }

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

auto ControlFlowGraph::Instruction2As() const
    -> std::vector<IRInstructionA2Ptr> {
  std::vector<IRInstructionA2Ptr> ret;
  for (const auto& block : Blocks()) {
    if (!block->reachable) continue;
    ret.insert(ret.end(), block->Instr2As().begin(), block->Instr2As().end());
  }
  return ret;
}

auto ControlFlowGraph::VertexByBlockId(int block_id) const -> Vertex {
  assert(block_id < bb_map_.size());
  return bb_map_.at(block_id);
}

auto ControlFlowGraph::BlockByBlockId(int block_id) const -> CFGBasicBlockPtr {
  assert(block_id < id_2_bb_.size());
  return id_2_bb_.at(block_id);
}

auto ControlFlowGraph::FilteredGraph() -> FilteredCFGGraphPtr {
  if (!f_g_) {
    // use filtered_graph to connect together
    auto& full = Graph();
    f_g_ = std::make_shared<FilteredCFGGraph>(full, KeepEdges{&full},
                                              KeepReachable{&full});
  }

  return f_g_;
}
