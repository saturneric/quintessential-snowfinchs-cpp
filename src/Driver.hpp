#pragma once

#undef yyFlexLexer
#include "FlexLexer.h"

//
#include "AST.h"
#include "Parser.hpp"

namespace yy {

inline void parser::error(const std::string& msg) {
  std::cerr << "Syntax error: " << msg << '\n';
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

 private:
  SymbolTablePtr symbol_table_;
  class AST ast_;
};

#define YY_DECL auto Driver::yylex(Driver& drv) -> yy::parser::symbol_type

auto MakeASTTreeNode(ASTNodeType type, const std::string& name,
                     const std::string& value, Driver& driver) -> ASTNodePtr;