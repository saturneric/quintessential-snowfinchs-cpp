

#include "AST.h"
#include "ScopedSymbolTable.h"

class SemanticAnalyzer {
 public:
  using NodeRouter = std::function<ASTNodePtr(const ASTNodePtr&)>;
  using NodeHandler = std::function<ASTNodePtr(
      SemanticAnalyzer*, const NodeRouter&, const ASTNodePtr&)>;

  explicit SemanticAnalyzer(SymbolTablePtr symbol_table);

  auto Analyze(const AST& ast) -> bool;

  auto RecordSymbol(const SymbolPtr& symbol) -> std::tuple<bool, SymbolPtr>;

  auto LookupSymbol(const SymbolPtr& symbol) -> SymbolPtr;

  auto VisibleDefineSymbols(const ScopePtr& scope) -> std::vector<SymbolPtr>;

  void Error(const ASTNodePtr& node, const std::string& msg);

  void SetMeta(const std::string& key, std::any value);

  auto Meta(const std::string& key) -> std::any&;

  void PrintSymbolTable(const std::string& path);

 private:
  static std::map<ASTNodeType, NodeHandler> node_handler_register;
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  ScopedSymbolLookUpHelper def_sym_helper_;
  std::map<std::string, std::any> meta_data_;

  bool succ_;
  int inner_var_index_ = 0;

  auto visit(const ASTNodePtr& node) -> ASTNodePtr;

  static auto is_integer_literal(const std::string& s) -> bool;
};