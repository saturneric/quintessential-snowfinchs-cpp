#pragma once

#include "InterferenceGraph.h"
#include "Symbol.h"

struct RegisterAllocResult {
  std::map<SymbolPtr, std::string> reg_assignment;
  std::set<SymbolPtr> spilled;
};

auto GraphColorRegisterAlloc(const InterferenceGraph& graph,
                             const std::vector<std::string>& registers)
    -> RegisterAllocResult;
