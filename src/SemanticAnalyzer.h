

#include "AST.h"

class SemanticAnalyzer {
 public:
  explicit SemanticAnalyzer(SymbolTablePtr symbol_table);

  auto Analyze(const AST& ast) -> bool;

 private:
  enum class ExpType : uint8_t { kINT, kBOOL, kVOID };
  SymbolTablePtr symbol_table_;
  int scopes_ = 0;
  bool succ_;

  void enter_scope();

  void leave_scope();

  auto declare(const std::string& name, const std::string& type) -> bool;

  auto lookup(const std::string& name) -> SymbolPtr;

  auto visit(const ASTNodePtr& node) -> ASTNodePtr;

  auto visit_expr(const ASTNodePtr& expr) -> ExpType;

  static auto is_integer_literal(const std::string& s) -> bool;

  void error(const ASTNodePtr& /*node*/, const std::string& msg);
};