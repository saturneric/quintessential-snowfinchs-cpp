#pragma once

#include "model/IRInstruction.h"

class CFGBasicBlock;
using CFGBasicBlockPtr = std::shared_ptr<CFGBasicBlock>;

using BlockHandle = unsigned long;
using BlockID = int;

class ControlFlowGraph {
 public:
  ControlFlowGraph();

  ~ControlFlowGraph();

  auto AddBlock(const CFGBasicBlockPtr& bb) -> BlockHandle;

  void AddEdge(int from_id, int to_id);

  [[nodiscard]] auto VertexByBlockId(BlockID block_id) const -> BlockHandle;

  [[nodiscard]] auto BlockByBlockId(BlockID block_id) const -> CFGBasicBlockPtr;

  [[nodiscard]] auto Blocks() const -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Successors(BlockID id) const
      -> std::vector<CFGBasicBlockPtr>;

  [[nodiscard]] auto Predecessors(BlockID id) const
      -> std::vector<CFGBasicBlockPtr>;

  void Print(std::ostream& os) const;

  [[nodiscard]] auto Instructions() const -> std::vector<IRInstructionPtr>;

  void BuildFilteredGraph();

  void BuildDominatorTree();

  [[nodiscard]] auto Dominates(BlockID u_block_id, BlockID v_block_id) const
      -> bool;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

using ControlFlowGraphPtr = std::shared_ptr<ControlFlowGraph>;