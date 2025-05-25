#pragma once

#include <utility>

#include "core/SymbolTable.h"
#include "model/ControlFlowGraph.h"
#include "model/IRInstruction.h"
#include "model/Symbol.h"

struct VariableLiveRange {
  int start = std::numeric_limits<int>::max();
  int end = std::numeric_limits<int>::min();
};

class IRVariableOptimizer {
 public:
  explicit IRVariableOptimizer(SymbolTablePtr symbol_table,
                               ControlFlowGraphPtr cfg);

  void AnalyzeLiveRanges();

  void AllocateSlots();

  auto GetSlotVariable(const SymbolPtr& var) -> SymbolPtr;

  void RewriteInstructions();

  void PrintSlotMap(const std::string& path) const;

  void PrintLiveRanges(const std::string& path) const;

 private:
  SymbolTablePtr symbol_table_;
  ControlFlowGraphPtr cfg_;

  // var liveness range
  std::unordered_map<SymbolPtr, VariableLiveRange> live_ranges_;

  // var -> slot
  std::unordered_map<SymbolPtr, SymbolPtr> var_to_slot_;

  // slot分配池
  struct Slot {
    SymbolPtr sym;
    int end;
  };

  std::vector<Slot> slot_pool_;

  auto map_slot(const std::string& name) -> SymbolPtr;
};