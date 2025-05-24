#pragma once

#include "Symbol.h"

enum class IRInstructionType : uint8_t {
  kNONE,
  kIR_INSTRUCTION_A2,
  kIR_INSTRUCTION_A3,
};

class IRInstruction {
 public:
  [[nodiscard]] virtual auto Type() const -> IRInstructionType = 0;

  [[nodiscard]] virtual auto Op() const -> SymbolPtr = 0;

  [[nodiscard]] virtual auto DST() const -> SymbolPtr = 0;

  [[nodiscard]] virtual auto SRC(int i) const -> SymbolPtr = 0;

  [[nodiscard]] virtual auto Use() const -> std::vector<SymbolPtr> = 0;

  virtual auto LiveIn() -> std::set<SymbolPtr>& = 0;

  virtual auto LiveOut() -> std::set<SymbolPtr>& = 0;
};

using IRInstructionPtr = std::shared_ptr<IRInstruction>;

struct IRInstructionA2 : public IRInstruction {
  SymbolPtr op;
  SymbolPtr dst;
  SymbolPtr src;

  // for le, gt...
  SymbolPtr src_2;

  std::set<SymbolPtr> live_in;
  std::set<SymbolPtr> live_out;

  explicit IRInstructionA2(SymbolPtr op, SymbolPtr dst = nullptr,
                           SymbolPtr src = nullptr, SymbolPtr src_2 = nullptr);

  [[nodiscard]] auto Type() const -> IRInstructionType override;

  [[nodiscard]] auto Op() const -> SymbolPtr override;

  [[nodiscard]] auto DST() const -> SymbolPtr override;

  [[nodiscard]] auto SRC(int i) const -> SymbolPtr override;

  [[nodiscard]] auto Use() const -> std::vector<SymbolPtr> override;

  auto LiveIn() -> std::set<SymbolPtr>& override;

  auto LiveOut() -> std::set<SymbolPtr>& override;
};

using IRInstructionA2Ptr = std::shared_ptr<IRInstructionA2>;

class IRInstructionA3 : public IRInstruction {
 public:
  explicit IRInstructionA3(SymbolPtr op, SymbolPtr dst = nullptr,
                           const SymbolPtr& src_1 = nullptr,
                           const SymbolPtr& src_2 = nullptr);

  [[nodiscard]] auto Type() const -> IRInstructionType override;

  [[nodiscard]] auto Op() const -> SymbolPtr override;

  [[nodiscard]] auto DST() const -> SymbolPtr override;

  [[nodiscard]] auto SRC(int i) const -> SymbolPtr override;

  [[nodiscard]] auto Use() const -> std::vector<SymbolPtr> override;

  auto LiveIn() -> std::set<SymbolPtr>& override;

  auto LiveOut() -> std::set<SymbolPtr>& override;

 private:
  SymbolPtr op_;
  SymbolPtr dst_;
  std::vector<SymbolPtr> srcs_;
  std::set<SymbolPtr> live_in_;
  std::set<SymbolPtr> live_out_;
};

using IRInstructionA3Ptr = std::shared_ptr<IRInstructionA3>;

const std::string kBinOpType = "binop";
const std::string kUnOpType = "unop";
const std::string kCmpOpType = "cmpop";

auto IsCondJump(const SymbolPtr& op) -> bool;

auto IsJump(const SymbolPtr& op) -> bool;