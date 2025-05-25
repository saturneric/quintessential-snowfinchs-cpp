#include "CFGBasicBlock.h"

CFGBasicBlock::CFGBasicBlock(int id, std::string label)
    : id(id), label(std::move(label)) {}

auto CFGBasicBlock::Instrs() -> std::vector<IRInstructionPtr>& {
  return instrs_;
}
