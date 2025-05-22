#include "IRInstruction.h"

IRInstruction::IRInstruction(SymbolPtr op, SymbolPtr dst,
                             const SymbolPtr& src_1, const SymbolPtr& src_2)
    : op_(std::move(op)), dst_(std::move(dst)) {
  srcs_.push_back(src_1);
  srcs_.push_back(src_2);
}

auto IRInstruction::SRC(int i) -> SymbolPtr {
  if (srcs_.size() > i) return srcs_[i];
  return nullptr;
}

auto IRInstruction::Op() -> SymbolPtr { return op_; }

auto IRInstruction::DST() -> SymbolPtr { return dst_; }

auto IRInstruction::Use() -> std::vector<SymbolPtr>& { return srcs_; }

IRInstructionA2::IRInstructionA2(SymbolPtr op, SymbolPtr dst, SymbolPtr src,
                                 SymbolPtr src_2)
    : op(std::move(op)),
      dst(std::move(dst)),
      src(std::move(src)),
      src_2(std::move(src_2)) {}
