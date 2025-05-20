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

  // enter global naming scope
  ScopedSymbolTable()->EnterScope();

  auto ret = parse.parse();

  // leave global naming scope
  ScopedSymbolTable()->LeaveScope();

  if (lexer_error_) {
    SPDLOG_ERROR("Lexer Error: {}", lexer_error_msg_);
    return -1;
  }

  return ret;
}
void Driver::SetSyntaxTreeRoot(const ASTNodePtr& root) { ast_.SetRoot(root); }

auto Driver::SymbolTable() -> SymbolTablePtr {
  return symbol_table_->SymbolTable();
}

Driver::Driver(const SymbolTablePtr& symbol_table)
    : symbol_table_(std::make_shared<class ScopedSymbolTable>(SymbolType::kAST,
                                                              symbol_table)),
      ast_(symbol_table) {}

auto MakeASTTreeNode(ASTNodeType type, const std::string& node,
                     const std::string& symbol, Driver& driver) -> ASTNodePtr {
  return std::make_shared<ASTNode>(
      type, driver.ScopedSymbolTable()->AddSymbol(symbol, node, false));
}

void Driver::Print(const std::string& path) { ast_.Print(path); }

auto Driver::yylex() -> int { return 0; }

auto yyFlexLexer::yylex() -> int { return 0; }

auto Driver::AST() -> class AST {
  return ast_;
}

void Driver::LexerError(const char* msg) {
  lexer_error_ = true;
  lexer_error_msg_ = msg;
}

auto Driver::Location() -> yy::location& { return loc_; }

auto Driver::ScopedSymbolTable() -> ScopedSymbolTablePtr {
  return symbol_table_;
}

void EnterScope(Driver& driver) { driver.ScopedSymbolTable()->EnterScope(); }

void LeaveScope(Driver& driver) { driver.ScopedSymbolTable()->LeaveScope(); }