#pragma once

#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "AST.h"
#include "ScopedSymbolTable.h"

struct IRInstructionA2 {
  SymbolPtr op;
  SymbolPtr dst;
  SymbolPtr src;

  explicit IRInstructionA2(SymbolPtr op, SymbolPtr dst = nullptr,
                           SymbolPtr src = nullptr)
      : op(std::move(op)), dst(std::move(dst)), src(std::move(src)) {}
};

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

class IRGenerator {
 public:
  class Context;
  using ContextPtr = std::shared_ptr<Context>;
  using ExpHandler = std::function<SymbolPtr(Context*, const ASTNodePtr&)>;

  class Context {
   public:
    explicit Context(IRGenerator* ig, ExpHandler handler);

    auto ExpRoute(const ASTNodePtr& node) -> SymbolPtr;

    auto NewTempVariable() -> SymbolPtr;

    auto NewLabel() -> SymbolPtr;

    void AddIns(const std::string& op, SymbolPtr dst = nullptr,
                SymbolPtr src_1 = nullptr, SymbolPtr src_2 = nullptr);

    static auto SelectInstruction(const std::string& operation) -> std::string;

    [[nodiscard]] auto Instructions() const -> std::vector<IRInstruction>;

    void EnterScope();

    void LeaveScope();

    auto MapDefSym(const SymbolPtr& symbol) -> SymbolPtr;

    auto MapOp(const std::string& name) -> SymbolPtr;

    auto MapSymbol(const SymbolPtr& symbol) -> SymbolPtr;

    auto MapSymbol(const std::string& name, const std::string& type)
        -> SymbolPtr;

    void AddError(const std::string& err);

   private:
    IRGenerator* ig_;
    ExpHandler handler_;

    std::vector<IRInstruction> ins_;
  };

  explicit IRGenerator(SymbolTablePtr symbol_table);

  auto Generate(const AST& tree) -> std::vector<IRInstructionA2>;

  void Print3Addr(const std::string& path);

  void Print2Addr(const std::string& path);

 private:
  static std::map<ASTNodeType, ExpHandler> exp_handler_register;
  ContextPtr ctx_;
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper def_symbol_helper_;
  ScopedSymbolLookUpHelper ir_symbol_helper_;
  int tmp_var_idx_ = 1;

  std::vector<IRInstructionA2> instructions_2_addr_;
  std::vector<IRInstruction> ins_ssa_;
  std::unordered_map<std::string, SymbolPtr> op_ins_;

  static auto select_instruction(const std::string& operation) -> std::string;

  auto do_ir_generate(Context*, const ASTNodePtr& node) -> SymbolPtr;

  auto map_ssa(const SymbolPtr& sym, bool is_def) -> SymbolPtr;

  void convert2_ssa();

  void convert_ira3_2_ira2();

  auto reg_op(const std::string& name, const std::string& type = {})
      -> SymbolPtr;

  auto map_op(const std::string& name) -> SymbolPtr;

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  auto lookup_variable(const SymbolPtr& ast_sym) -> SymbolPtr;

  auto new_temp_variable() -> SymbolPtr;

  auto new_label() -> SymbolPtr;
};