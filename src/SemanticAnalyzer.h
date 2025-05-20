

#include "AST.h"
#include "ScopedSymbolTable.h"

enum class ExpType { kINT, kBOOL, kVOID };

class SemanticAnalyzer {
 public:
  explicit SemanticAnalyzer(SymbolTablePtr symbol_table);

  auto Analyze(const AST& ast) -> bool;

 private:
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  ScopedSymbolLookUpHelper def_sym_helper_;

  bool succ_;
  int inner_var_index_ = 0;

  auto record_symbol(const SymbolPtr& symbol) -> bool;

  auto lookup_symbol(const SymbolPtr& symbol) -> SymbolPtr;

  auto visit(const ASTNodePtr& node) -> ASTNodePtr;

  auto visit_expr(const ASTNodePtr& expr) -> ExpType;

  static auto is_integer_literal(const std::string& s) -> bool;

  void error(const ASTNodePtr& /*node*/, const std::string& msg);
};