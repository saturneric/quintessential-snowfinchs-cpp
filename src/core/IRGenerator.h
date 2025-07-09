#pragma once

#include "core/AST.h"
#include "core/ScopedSymbolTable.h"

// model
#include "model/ControlFlowGraph.h"
#include "model/IRInstruction.h"
#include "model/Symbol.h"

class IRGeneratorContext;
using IRGeneratorContextPtr = std::shared_ptr<IRGeneratorContext>;

class IRGenerator;
using IRExpHandler =
    std::function<SymbolPtr(IRGeneratorContext*, const ASTNodePtr&, bool)>;

class IRGeneratorContext {
 public:
  explicit IRGeneratorContext(IRGenerator* ig, IRExpHandler handler);

  auto ExpRoute(const ASTNodePtr& node, bool is_lhs = false) -> SymbolPtr;

  auto NewTempVariable() -> SymbolPtr;

  auto NewTempAddressVariable() -> SymbolPtr;

  auto NewLabel() -> SymbolPtr;

  void AddIns(const std::string& op, SymbolPtr dst = nullptr,
              SymbolPtr src_1 = nullptr, SymbolPtr src_2 = nullptr);

  static auto SelectInstruction(const std::string& operation) -> std::string;

  [[nodiscard]] auto Instructions() const -> std::vector<FuncInstructions>;

  void EnterInsGroup(const std::string& func);

  void EnterScope();

  void LeaveScope();

  auto MapDefSym(const SymbolPtr& symbol) -> SymbolPtr;

  auto MapDefSym(ScopePtr scope, const std::string& name) -> SymbolPtr;

  auto MapOp(const std::string& name) -> SymbolPtr;

  auto MapSymbol(const SymbolPtr& symbol) -> SymbolPtr;

  auto MapSymbol(const std::string& name, const std::string& type) -> SymbolPtr;

  void AddError(const std::string& err);

  auto LookupType(const std::string& type_name) -> SymbolPtr;

  [[nodiscard]] auto Success() const -> bool;

 private:
  IRGenerator* ig_;
  IRExpHandler handler_;
  bool success_ = true;

  std::vector<FuncInstructions> ins_;
};

using IRHandlerMapping = std::unordered_map<ASTNodeType, IRExpHandler>;

class IRGenerator {
 public:
  friend IRGeneratorContext;

  explicit IRGenerator(SymbolTablePtr symbol_table, IRHandlerMapping mapping);

  auto Generate(const AST& tree) -> bool;

  void PrintAddr(const std::string& path);

  void PrintCFG(const std::string& path);

  auto ControlFlowGraph() -> std::vector<FuncCFG>&;

 private:
  IRHandlerMapping ir_handler_mapping_;
  IRGeneratorContextPtr ctx_;
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper def_symbol_helper_;
  ScopedSymbolLookUpHelper ir_symbol_helper_;
  ScopedSymbolLookUpHelper type_desc_helper_;
  int tmp_var_idx_ = 1;

  std::vector<FuncCFG> cfgs_;
  std::unordered_map<std::string, SymbolPtr> op_ins_;

  static auto select_instruction(const std::string& operation) -> std::string;

  auto do_ir_generate(IRGeneratorContext*, const ASTNodePtr& node,
                      bool is_lhs = false) -> SymbolPtr;

  auto map_ssa(const SymbolPtr& sym, bool is_def) -> SymbolPtr;

  void convert2_ssa();

  auto reg_op(const std::string& name, const std::string& type = {})
      -> SymbolPtr;

  auto map_op(const std::string& name) -> SymbolPtr;

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  auto lookup_variable(const SymbolPtr& ast_sym) -> SymbolPtr;

  auto new_phi_variable(const SymbolPtr& var, int block_id) -> SymbolPtr;

  auto new_temp_variable() -> SymbolPtr;

  auto new_label() -> SymbolPtr;

  void build_cfg();

  void block_level_liveness_analyse();

  void instruction_level_liveness_analyse();

  void block_level_def_analyse();

  void insert_phi();

  void refresh_irs_by_cfg();

  auto lookup_type(const std::string& type_name) -> SymbolPtr;

  auto lookup_variable(ScopePtr scope, const std::string& name) -> SymbolPtr;

  static auto optimums(const std::vector<IRInstructionPtr>&)
      -> std::vector<IRInstructionPtr>;
};