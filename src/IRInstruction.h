#pragma once

#include "Symbol.h"

struct IRInstructionA2 {
  SymbolPtr op;
  SymbolPtr dst;
  SymbolPtr src;

  // for le, gt...
  SymbolPtr src_2;

  std::set<SymbolPtr> LiveIn;

  std::set<SymbolPtr> LiveOut;

  explicit IRInstructionA2(SymbolPtr op, SymbolPtr dst = nullptr,
                           SymbolPtr src = nullptr, SymbolPtr src_2 = nullptr);

  [[nodiscard]] auto Use() const -> std::vector<SymbolPtr>;
};

using IRInstructionA2Ptr = std::shared_ptr<IRInstructionA2>;

struct IRInstruction {
  explicit IRInstruction(SymbolPtr op, SymbolPtr dst = nullptr,
                         const SymbolPtr& src_1 = nullptr,
                         const SymbolPtr& src_2 = nullptr);

  auto Op() -> SymbolPtr;

  auto DST() -> SymbolPtr;

  auto SRC(int i) -> SymbolPtr;

  auto Use() -> std::vector<SymbolPtr>&;

  std::set<SymbolPtr> LiveIn;

  std::set<SymbolPtr> LiveOut;

 private:
  SymbolPtr op_;
  SymbolPtr dst_;
  std::vector<SymbolPtr> srcs_;
};

using IRInstructionPtr = std::shared_ptr<IRInstruction>;

const std::string kBinOpType = "binop";
const std::string kUnOpType = "unop";
const std::string kCmpOpType = "cmpop";

inline auto IsCondJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "jmp" || n == "je" || n == "jne" || n == "jg" || n == "jl" ||
         n == "jge" || n == "jle";
}

inline auto IsJump(const SymbolPtr& op) -> bool {
  auto n = op->Name();
  return n == "jmp" || IsCondJump(op);
}