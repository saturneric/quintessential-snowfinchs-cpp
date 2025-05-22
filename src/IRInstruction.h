#pragma once

#include "Symbol.h"

struct IRInstructionA2 {
  SymbolPtr op;
  SymbolPtr dst;
  SymbolPtr src;

  // for le, gt...
  SymbolPtr src_2;

  // for phi only
  std::vector<SymbolPtr> phi_srcs;

  explicit IRInstructionA2(SymbolPtr op, SymbolPtr dst = nullptr,
                           SymbolPtr src = nullptr)
      : op(std::move(op)), dst(std::move(dst)), src(std::move(src)) {}

  [[nodiscard]] auto Use() const -> std::vector<SymbolPtr>;
};

using IRInstructionA2Ptr = std::shared_ptr<IRInstructionA2>;

struct IRInstruction {
  SymbolPtr op;
  SymbolPtr dst;
  SymbolPtr src_1;
  SymbolPtr src_2;

  explicit IRInstruction(SymbolPtr op, SymbolPtr dst = nullptr,
                         SymbolPtr src_1 = nullptr, SymbolPtr src_2 = nullptr)
      : op(std::move(op)),
        dst(std::move(dst)),
        src_1(std::move(src_1)),
        src_2(std::move(src_2)) {}
};
