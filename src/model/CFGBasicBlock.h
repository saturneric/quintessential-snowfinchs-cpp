#pragma once

#include "model/IRInstruction.h"
#include "model/Symbol.h"

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

  explicit CFGBasicBlock(int id, std::string label = "");

  auto Instrs() -> std::vector<IRInstructionPtr>&;

 private:
  std::vector<IRInstructionPtr> instrs_;  // index in ir array
};

using CFGBasicBlockPtr = std::shared_ptr<CFGBasicBlock>;