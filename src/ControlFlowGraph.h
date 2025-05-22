#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "IRInstruction.h"
#include "Symbol.h"

struct CFGBasicBlock {
  int id;
  std::string label;
  std::vector<IRInstructionA2Ptr> instrs;  // index in ir array

  std::set<SymbolPtr> use;
  std::set<SymbolPtr> def;
  std::set<SymbolPtr> in;
  std::set<SymbolPtr> out;

  explicit CFGBasicBlock(int id, std::string label = "")
      : id(id), label(std::move(label)) {}
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

  [[nodiscard]] auto Blocks() const -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Successors(int id) const -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Predecessors(int id) const
      -> std::vector<CFGBasicBlockPtr>;

  auto Graph() -> CFGGraph&;

  [[nodiscard]] auto Graph() const -> const CFGGraph&;

  void Print(std::ostream& os) const;

 private:
  CFGGraph g_;
  std::map<int, Vertex> bb_map_;  // id -> vertex
};

using ControlFlowGraphPtr = std::shared_ptr<ControlFlowGraph>;