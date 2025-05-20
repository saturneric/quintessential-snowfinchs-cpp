#pragma once

#undef yyFlexLexer
#include "FlexLexer.h"

//
#include "AST.h"
#include "Parser.hpp"

namespace yy {

inline void parser::error(const location_type& l, const std::string& m) {
  SPDLOG_ERROR("Parser error at {}:{}-{}:{}; Message: {}", l.begin.line,
               l.begin.column, l.end.line, l.end.column, m);
}

}  // namespace yy

class Driver : public yyFlexLexer {
 public:
  // bison use yylex to get a token
  auto yylex(Driver& drv) -> yy::parser::symbol_type;

  // default yylex
  auto yylex() -> int override;

  explicit Driver(const SymbolTablePtr& symbol_table);

  auto Parse(const std::string& path) -> int;

  void SetSyntaxTreeRoot(const ASTNodePtr& root);

  auto SymbolTable() -> SymbolTablePtr;

  void Print(const std::string& path);

  auto AST() -> AST;

  void LexerError(const char* msg) override;

  auto Location() -> yy::location&;

  auto LastToken() -> yy::location&;

 private:
  class AST ast_;
  yy::location loc_;
  bool lexer_error_ = false;
  std::string lexer_error_msg_;
  SymbolTablePtr symbol_table_;
};

#define YY_DECL auto Driver::yylex(Driver& drv) -> yy::parser::symbol_type

auto MakeASTTreeNode(ASTNodeType type, const std::string& name,
                     const std::string& value, Driver& driver) -> ASTNodePtr;