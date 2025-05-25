#include "InterferenceGraphAlgos.h"

auto GraphColorRegisterAlloc(const InterferenceGraph& graph,
                             const std::vector<std::string>& registers)
    -> RegisterAllocResult {
  RegisterAllocResult result;

  for (const auto& [v, color] : graph.Coloring()) {
    if (color < registers.size()) {
      result.reg_assignment[v] = registers[color];
      continue;
    }

    result.spilled.insert(v);
  }

  for (const auto& var : graph.Variables()) {
    if (result.reg_assignment.count(var) == 0) result.spilled.insert(var);
  }

  return result;
}
