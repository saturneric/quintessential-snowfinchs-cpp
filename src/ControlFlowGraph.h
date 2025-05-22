#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "IRGenerator.h"

struct InstrLiveness {
  std::vector<SymbolPtr> use, def;
  std::set<SymbolPtr> in, out;
};

using InstrLivenessPtr = std::shared_ptr<InstrLiveness>;

struct BasicBlock {
  int id;
  std::string label;
  std::vector<size_t> instr_indices;  // index in ir array
  std::vector<InstrLivenessPtr> instrs;

  std::set<SymbolPtr> use;
  std::set<SymbolPtr> def;
  std::set<SymbolPtr> in;
  std::set<SymbolPtr> out;

  explicit BasicBlock(int id, std::string label = "")
      : id(id), label(std::move(label)) {}
};
using BasicBlockPtr = std::shared_ptr<BasicBlock>;

struct VertexProp {
  BasicBlockPtr bb;
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

  auto AddBlock(const BasicBlockPtr& bb) -> Vertex;

  void AddEdge(int from_id, int to_id);

  [[nodiscard]] auto Blocks() const -> std::vector<BasicBlockPtr>;

  [[nodiscard]] auto Successors(int id) const -> std::vector<BasicBlockPtr>;

  [[nodiscard]] auto Predecessors(int id) const -> std::vector<BasicBlockPtr>;

  auto Graph() -> CFGGraph&;
  [[nodiscard]] auto Graph() const -> const CFGGraph&;

 private:
  CFGGraph g_;
  std::map<int, Vertex> bb_map_;  // id -> vertex
};