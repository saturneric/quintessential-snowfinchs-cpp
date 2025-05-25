#include "IRInstruction.h"

auto IsCondJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "je" || n == "jne" || n == "jg" || n == "jl" || n == "jge" ||
         n == "jle" || n == "jnz" || n == "brz" || n == "brnz";
}

auto IsJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "jmp" || IsCondJump(op);
}

IRInstructionA3::IRInstructionA3(SymbolPtr op, SymbolPtr dst,
                                 const SymbolPtr& src_1, const SymbolPtr& src_2)
    : op_(std::move(op)), dst_(std::move(dst)) {
  srcs_.push_back(src_1);
  srcs_.push_back(src_2);
}

auto IRInstructionA3::SRC(int i) const -> SymbolPtr {
  if (srcs_.size() > i) return srcs_[i];
  return nullptr;
}

auto IRInstructionA3::Op() const -> SymbolPtr { return op_; }

auto IRInstructionA3::DST() const -> SymbolPtr { return dst_; }

auto IRInstructionA3::Use() const -> std::vector<SymbolPtr> { return srcs_; }

IRInstructionA2::IRInstructionA2(SymbolPtr op, SymbolPtr dst, SymbolPtr src,
                                 SymbolPtr src_2)
    : op(std::move(op)),
      dst(std::move(dst)),
      src(std::move(src)),
      src_2(std::move(src_2)) {}

auto IRInstructionA3::Type() const -> IRInstructionType {
  return IRInstructionType::kIR_INSTRUCTION_A3;
}

auto IRInstructionA3::LiveIn() -> std::set<SymbolPtr>& { return live_in_; }

auto IRInstructionA3::LiveOut() -> std::set<SymbolPtr>& { return live_out_; }

void IRInstructionA3::RenameVariable(SymbolPtr v, SymbolPtr n) {
  if (dst_ == v) dst_ = n;

  for (auto& src : srcs_) {
    if (src == v) src = n;
  }
}

auto IRInstructionA2::Type() const -> IRInstructionType {
  return IRInstructionType::kIR_INSTRUCTION_A2;
}

auto IRInstructionA2::Op() const -> SymbolPtr { return op; }

auto IRInstructionA2::DST() const -> SymbolPtr { return dst; }

auto IRInstructionA2::SRC(int i) const -> SymbolPtr {
  if (i == 0) return src;
  if (i == 1) return src_2;
  return nullptr;
}

auto IRInstructionA2::LiveIn() -> std::set<SymbolPtr>& { return live_in; }

auto IRInstructionA2::LiveOut() -> std::set<SymbolPtr>& { return live_out; }

void IRInstructionA2::RenameVariable(SymbolPtr v, SymbolPtr n) {
  if (dst == v) dst = n;
  if (src == v) src = n;
  if (src_2 == v) src_2 = n;
}
