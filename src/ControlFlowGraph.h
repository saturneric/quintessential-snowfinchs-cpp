#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "IRInstruction.h"
#include "Symbol.h"

struct CFGBasicBlock {
  int id;
  std::string label;

  std::set<SymbolPtr> use;
  std::set<SymbolPtr> def;
  std::set<SymbolPtr> def_in;
  std::set<SymbolPtr> def_out;

  std::set<SymbolPtr> live_in;
  std::set<SymbolPtr> live_out;

  bool has_return = false;
  bool will_return = false;
  bool reachable = false;

  explicit CFGBasicBlock(int id, std::string label = "", bool ir2_mode = false)
      : id(id), label(std::move(label)), ir2_mode_(ir2_mode) {}

  [[nodiscard]] auto IR2Mode() const -> bool { return ir2_mode_; }

  auto Instrs() -> std::vector<IRInstructionPtr>& {
    assert(ir2_mode_ == false);
    return instrs_;
  }

  auto Instr2As() -> std::vector<IRInstructionA2Ptr>& {
    assert(ir2_mode_ == true);
    return instr_a2s_;
  }

 private:
  bool ir2_mode_;
  std::vector<IRInstructionPtr> instrs_;       // index in ir array
  std::vector<IRInstructionA2Ptr> instr_a2s_;  // index in ir array (2 addr)
};
using CFGBasicBlockPtr = std::shared_ptr<CFGBasicBlock>;

struct VertexProp {
  CFGBasicBlockPtr bb;
};

using CFGGraph = boost::adjacency_list<boost::vecS,            // edge list
                                       boost::vecS,            // vertex list
                                       boost::bidirectionalS,  // directed
                                       VertexProp  // vertex property
                                       //,boost::no_property // edge property,
                                       >;

using Vertex = boost::graph_traits<CFGGraph>::vertex_descriptor;
using Edge = boost::graph_traits<CFGGraph>::edge_descriptor;

class ControlFlowGraph {
 public:
  ControlFlowGraph() = default;

  auto AddBlock(const CFGBasicBlockPtr& bb) -> Vertex;

  void AddEdge(int from_id, int to_id);

  [[nodiscard]] auto VertexByBlockId(int block_id) const -> Vertex;

  [[nodiscard]] auto BlockByBlockId(int block_id) const -> CFGBasicBlockPtr;

  [[nodiscard]] auto Blocks() const -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Successors(int id) const -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Predecessors(int id) const
      -> std::vector<CFGBasicBlockPtr>;

  auto Graph() -> CFGGraph&;

  [[nodiscard]] auto Graph() const -> const CFGGraph&;

  void Print(std::ostream& os) const;

  [[nodiscard]] auto Instructions() const -> std::vector<IRInstructionPtr>;

  [[nodiscard]] auto Instruction2As() const -> std::vector<IRInstructionA2Ptr>;

 private:
  CFGGraph g_;
  std::map<int, Vertex> bb_map_;             // id -> vertex
  std::map<int, CFGBasicBlockPtr> id_2_bb_;  // id -> block
};

using ControlFlowGraphPtr = std::shared_ptr<ControlFlowGraph>;