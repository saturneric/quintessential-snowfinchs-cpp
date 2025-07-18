#pragma once

#include "core/AST.h"
#include "core/ScopedSymbolTable.h"

class SemanticAnalyzer;

using SMNodeRouter = std::function<ASTNodePtr(const ASTNodePtr&)>;
using SMNodeHandler = std::function<ASTNodePtr(
    SemanticAnalyzer*, const SMNodeRouter&, const ASTNodePtr&)>;
using SMHandlerMapping = std::unordered_map<ASTNodeType, SMNodeHandler>;

class SemanticAnalyzer {
 public:
  explicit SemanticAnalyzer(SymbolTablePtr symbol_table,
                            SMHandlerMapping mapping);

  auto Analyze(const AST& ast) -> bool;

  auto RecordSymbol(const SymbolPtr& symbol) -> std::tuple<bool, SymbolPtr>;

  auto RecordRealSymbol(const SymbolPtr& symbol) -> std::tuple<bool, SymbolPtr>;

  auto LookupSymbol(const SymbolPtr& symbol) -> SymbolPtr;

  auto LookupSymbol(const ScopePtr& scope, const std::string& name)
      -> SymbolPtr;

  auto VisibleDefineSymbols(const ScopePtr& scope) -> std::vector<SymbolPtr>;

  void Error(const ASTNodePtr& node, const std::string& msg);

  void SetMeta(const std::string& key, std::any value);

  auto Meta(const std::string& key) -> std::any&;

  void PrintSymbolTable(const std::string& path);

  auto MapSymbol(int scope_id, const std::string& name,
                 const std::string& value) -> SymbolPtr;

  auto MapFunction(int scope_id, const std::string& name,
                   const std::string& value) -> SymbolPtr;

  auto MapStruct(int scope_id, const std::string& name,
                 const std::string& value) -> SymbolPtr;

  auto MapType(const std::string& id, const std::string& extra) -> SymbolPtr;

  auto GetRootScopeId() const -> int;

  auto GetRootScope() const -> ScopePtr;

  auto LookupType(const std::string& type_name) -> SymbolPtr;

 private:
  SMHandlerMapping node_handler_register_;
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  ScopedSymbolLookUpHelper def_sym_helper_;
  ScopedSymbolLookUpHelper type_desc_sym_helper_;
  std::map<std::string, std::any> meta_data_;
  ASTNodePtr ast_root_;

  bool succ_;
  int inner_var_index_ = 0;

  auto visit(const ASTNodePtr& node) -> ASTNodePtr;

  static auto is_integer_literal(const std::string& s) -> bool;
};