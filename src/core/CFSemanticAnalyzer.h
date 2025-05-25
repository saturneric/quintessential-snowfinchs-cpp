#pragma once

#include "model/ControlFlowGraph.h"

class CFSemanticAnalyzer {
 public:
  explicit CFSemanticAnalyzer(ControlFlowGraphPtr cfg);

  auto Analyse() -> bool;

  void PrintCFG(const std::string& path);

 private:
  bool succ_ = true;
  ControlFlowGraphPtr cfg_;

  void fail();

  void check_all_paths_return();

  void check_definite_assignment();
};
