#pragma once

#include "ControlFlowGraph.h"
#include "IRInstruction.h"
#include "ScopedSymbolTable.h"

class IR2Generator {
 public:
  explicit IR2Generator(SymbolTablePtr symbol_table,
                        std::vector<IRInstructionPtr> ir3);

  void Generate();

  void PrintAddr(const std::string& path);

  void PrintCFG(const std::string& path);

  auto ControlFlowGraph() -> ControlFlowGraphPtr;

 private:
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  ControlFlowGraphPtr cfg_;
  std::vector<IRInstructionPtr> ir3_;
  std::vector<IRInstructionA2Ptr> ir2_;

  auto map_op(const std::string& name) -> SymbolPtr;

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  void convert_ira3_2_ira2();

  void build_cfg();

  void block_level_liveness_analyse();

  void instruction_level_liveness_analyse();
};