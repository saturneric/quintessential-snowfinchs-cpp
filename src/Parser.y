%require "3.2"
%language "c++"
%defines "Parser.hpp"
%define api.token.prefix {TOKEN_}
%define api.value.type variant
%define api.token.constructor
%param { Driver& drv }
%debug

%code requires {
  #include "AST.h"
  class Driver;
}

%code {
    #define yylex drv.yylex
}

%{
#include <memory>

#include "Driver.hpp"
%}


%type <std::string> assign_operator
%type <std::vector<ASTNodePtr>> statements
%type <ASTNodePtr> program statement
%type <ASTNodePtr> simple_statement return_statement declarator left_value
%type <ASTNodePtr> expression additive multiplicative unary primary

%token <int> VALUE_INTEGER
%token <std::string> VALUE_ID
%token EQUAL PLUS_EQUAL SUB_EQUAL MULT_EQUAL SLASH_EQUAL PERCENT_EQUAL
%token PLUS SUB MULT SLASH PERCENT
%token SEMICOLON LEFT_BRACKET RIGHT_BRACKET OPENING_BRACE CLOSING_BRACE
%token LEFT_SHIFT RIGHT_SHIFT AND INSERT DELIMITER COMMA TILDE
%token STRUCT IF ELSE WHILE FOR CONTINUE BREAK RETURN 
%token ASSERT PRINT READ ALLOC ALLOC_ARRAY
%token TRUE FALSE STRING BOOLEAN VOID CHAR INT
%token NONE 

%left PLUS SUB
%left MULT SLASH PERCENT
%right UMINUS
%start program

%%
program:
    INT VALUE_ID LEFT_BRACKET RIGHT_BRACKET OPENING_BRACE statements CLOSING_BRACE
    {
      $$ = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", $2, drv);
      drv.SetSyntaxTreeRoot($$);
      for (const auto& child : $6) {
        $$->AddChildren(child);
      }
    }
;

statements:
    /* empty */
    {
      $$ = {};
    }
    | statement statements
    {
      $2.insert($2.begin(), $1);
      $$ = $2;
    }
;

statement:
    declarator SEMICOLON
    {
      $$ = $1;
    }
    | simple_statement SEMICOLON
    {
      $$ = $1;
    }
    | return_statement SEMICOLON
    {
      $$ = $1;
    }
;

simple_statement:
    left_value assign_operator expression
    {
      if ($2 == "=") {
        $$ = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", $2, drv);
        $$->AddChildren($1);
        $$->AddChildren($3);
      } else {

        auto op = $2;
        if (op.size() >= 2 && op.back() == '=') {
            op = op.substr(0, 1);
        }

        auto bin_op = MakeASTTreeNode(ASTNodeType::kBIN_OP, "simple_statement", op, drv);
        bin_op->AddChildren($1);
        bin_op->AddChildren($3);

        $$ = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", "=", drv);
        $$->AddChildren($1);
        $$->AddChildren(bin_op);
      }
    }
;

return_statement:
    RETURN expression
    {
      $$ = MakeASTTreeNode(ASTNodeType::kRETURN, "return_statement", "", drv);
      $$->AddChildren($2);
    }
;

declarator:
    INT VALUE_ID
    {
      $$ = MakeASTTreeNode(ASTNodeType::kDECLARE, "declarator", $2, drv);
    }
    | INT VALUE_ID EQUAL expression
    {
      $$ = MakeASTTreeNode(ASTNodeType::kDECLARE, "declarator", $2, drv);
      auto assign_node = MakeASTTreeNode(ASTNodeType::kASSIGN, "declarator", "=", drv);
      
      assign_node->AddChildren(
        MakeASTTreeNode(ASTNodeType::kIDENT, "declarator", $2, drv));
      assign_node->AddChildren($4);  

      $$->AddChildren(assign_node);
    }
;

left_value:
    VALUE_ID
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIDENT, "left_value", $1, drv);
    }
    | LEFT_BRACKET left_value RIGHT_BRACKET
    {
      $$ = $2;
    }
;

expression: additive;

additive:
    additive PLUS multiplicative
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "additive", "+", drv);
      $$->AddChildren($1);
      $$->AddChildren($3);
    }
    | additive SUB multiplicative
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "additive", "-", drv);
      $$->AddChildren($1);
      $$->AddChildren($3);
    }
    | multiplicative
    {
      $$ = $1;
    }
;

multiplicative:
    multiplicative MULT unary
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "*", drv);
      $$->AddChildren($1);
      $$->AddChildren($3);
    }
    | multiplicative SLASH unary
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "/", drv);
      $$->AddChildren($1);
      $$->AddChildren($3);
    }
    | multiplicative PERCENT unary
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "multiplicative", "%", drv);
      $$->AddChildren($1);
      $$->AddChildren($3);
    }
    | unary
    {
      $$ = $1;
    }
;

unary:
    SUB unary %prec UMINUS
    {
      $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "unary", "-", drv);
      $$->AddChildren($2);
    }
    | primary
    {
      $$ = $1;
    }
;

primary:
    LEFT_BRACKET expression RIGHT_BRACKET
    {
      $$ = $2;
    }
    | VALUE_INTEGER
    {
      $$ = MakeASTTreeNode(ASTNodeType::kVALUE, "primary", std::to_string($1), drv);
    }
    | VALUE_ID
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIDENT, "primary", $1, drv);
    }
;

assign_operator:
      EQUAL           { $$ = "="; }
    | PLUS_EQUAL      { $$ = "+="; }
    | SUB_EQUAL       { $$ = "-="; }
    | MULT_EQUAL      { $$ = "*="; }
    | SLASH_EQUAL     { $$ = "/="; }
    | PERCENT_EQUAL   { $$ = "%="; }
;

%%