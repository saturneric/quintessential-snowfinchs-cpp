#include "Driver.hpp"

#include <fstream>

auto Driver::Parse(const std::string& path) -> int {
  std::ifstream input(path);
  if (!input.is_open()) {
    std::cerr << "Cannot open file: " << path << '\n';
    return 1;
  }

  switch_streams(&input, nullptr);

  yy::parser parse(*this);
  // parse.set_debug_level(1);
  return parse.parse();
}
void Driver::SetSyntaxTreeRoot(const ASTNodePtr& root) { ast_.SetRoot(root); }

auto Driver::SymbolTable() -> SymbolTablePtr { return symbol_table_; }

Driver::Driver(const SymbolTablePtr& symbol_table)
    : symbol_table_(symbol_table), ast_(symbol_table) {}

auto MakeASTTreeNode(ASTNodeType type, const std::string& name,
                     const std::string& value, Driver& driver) -> ASTNodePtr {
  return std::make_shared<ASTNode>(
      type, driver.SymbolTable()->AddASTSymbol(name, value));
}

void Driver::Print(const std::string& path) { ast_.Print(path); }

auto Driver::yylex() -> int { return 0; }

auto yyFlexLexer::yylex() -> int { return 0; }

auto Driver::AST() -> class AST {
  return ast_;
}
