%require "3.2"
%language "c++"
%defines "Parser.hpp"
%define api.token.prefix {TOKEN_}
%define api.value.type variant
%define api.token.constructor
%param { Driver& drv }
%locations
%define api.location.file "Location.hpp"
%define parse.error verbose
%debug

%code requires {
  #include "core/AST.h"
  class Driver;
}

%code {
    #define yylex drv.yylex
}

%{
  #include "Driver.hpp"
%}

%type <std::string> type assign_operator
%type <std::vector<ASTNodePtr>> statements
%type <ASTNodePtr> simple_optional statement control block
%type <ASTNodePtr> simple_statement declarator left_value
%type <ASTNodePtr> expression
%type <ASTNodePtr> function
%type <ASTNodePtr> program
%type <std::vector<ASTNodePtr>> functions
%type <std::vector<ASTNodePtr>> param_list_follow
%type <ASTNodePtr> param_list param
%type <std::vector<ASTNodePtr>>  arg_list_follow
%type <ASTNodePtr> arg_list call

// id 123
%token <std::string> VALUE_ID VALUE_INTEGER
// = += -= *= /= %=
%token ASSIGN PLUS_ASSIGN SUB_ASSIGN MULT_ASSIGN SLASH_ASSIGN PERCENT_ASSIGN
// &= ^= |= <<= >>=
%token BIT_AND_ASSIGN BIT_EX_OR_ASSIGN BIT_OR_ASSIGN LEFT_SHIFT_ASSIGN RIGHT_SHIFT_ASSIGN 
// + - * / %
%token PLUS SUB MULT SLASH PERCENT
// ! ~
%token LOGIC_NOT BIT_NOT
// ; ( ) { } ,
%token SEMICOLON LEFT_BRACKET RIGHT_BRACKET OPENING_BRACE CLOSING_BRACE COMMA
// << >> & ^ |
%token LEFT_SHIFT RIGHT_SHIFT BIT_AND BIT_EX_OR BIT_OR
// && ||
%token LOGIC_AND LOGIC_OR
// ? :
%token QUESTION COLON
// < <= > >= == !=
%token LT LT_EQ GT GT_EQ EQ NOT_EQ
// function
%token PRINT READ FLUSH

%token STRUCT IF ELSE WHILE FOR CONTINUE BREAK RETURN 
%token ASSERT ALLOC ALLOC_ARRAY
%token TRUE FALSE KW_NULL
%token STRING BOOLEAN VOID CHAR INT
%token NONE 

// solve if-else reduce problem
%right IFX
%nonassoc ELSE

%right ASSIGN PLUS_ASSIGN SUB_ASSIGN MULT_ASSIGN SLASH_ASSIGN PERCENT_ASSIGN BIT_AND_ASSIGN BIT_EX_OR_ASSIGN BIT_OR_ASSIGN LEFT_SHIFT_ASSIGN RIGHT_SHIFT_ASSIGN 
%right QUESTION COLON CON_EXP // ?:

%left LOGIC_OR
%left LOGIC_AND
%left BIT_OR
%left BIT_EX_OR
%left BIT_AND
%left EQ NOT_EQ
%left LT LT_EQ GT GT_EQ 
%left LEFT_SHIFT RIGHT_SHIFT
%left PLUS SUB
%left MULT SLASH PERCENT

%right LOGIC_NOT BIT_NOT UMINUS   // ! ~ -

%start program

%%
program:
  functions    
  { 
    $$ = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", {}, drv);
    drv.SetSyntaxTreeRoot($$);
    for (auto p : $1) $$->AddChild(p);
  }
;

functions:
    /* empty */ 
    { $$ = std::vector<ASTNodePtr>(); }
  | function functions
    {
      $2.insert($2.begin(), $1);
      $$ = $2;
    }
;

function:
    type VALUE_ID { EnterScope(drv); } LEFT_BRACKET param_list RIGHT_BRACKET block
    {
      $$ = MakeASTTreeNode(ASTNodeType::kFUNCTION, $1, std::string("__func_") + $2, drv);
      if ($5) $$->AddChild($5, ASTNodeTag::kPARAMS);
      $$->AddChild($7, ASTNodeTag::kBODY);
      LeaveScope(drv);
    }
;

param_list:
    /* empty */
    { $$ = nullptr; }
  | param param_list_follow
    {
      auto list = MakeASTTreeNode(ASTNodeType::kPARAM_LIST, "param_list", {}, drv);
      list->AddChild($1);
      for (auto p : $2) list->AddChild(p);
      $$ = list;
    }
;

param_list_follow:
    /* empty */
    { $$ = std::vector<ASTNodePtr>(); }
  | COMMA param param_list_follow
    {
      auto v = $3;
      v.insert(v.begin(), $2);
      $$ = std::vector<ASTNodePtr>(v);
    }
;

param:
    type VALUE_ID
    {
      $$ = MakeASTTreeNode(ASTNodeType::kPARAM, $1, $2, drv);
    }
;

block:
    OPENING_BRACE { EnterScope(drv); } statements CLOSING_BRACE 
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBLOCK, "block", {}, drv);
      for (const auto& child : $3) {
        $$->AddChild(child);
      }
      LeaveScope(drv);
    }

type:
      INT     { $$ = "int"; }
    | BOOLEAN { $$ = "bool"; }
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
    simple_statement SEMICOLON
    {
      $$ = $1;
    }
    | control
    {
      $$ = $1;
    }
    | block
    {
      $$ = $1;
    }
;

simple_statement:
    left_value assign_operator expression
    {
      if ($2 == "=") {
        $$ = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", $2, drv);
        $$->AddChild($1);
        $$->AddChild($3);
      } else {

        auto op = $2;
        if (!op.empty() && op.back() == '=') {
          op.pop_back();
        }

        auto bin_op = MakeASTTreeNode(ASTNodeType::kBIN_OP, "simple_statement", op, drv);
        bin_op->AddChild($1);
        bin_op->AddChild($3);

        $$ = MakeASTTreeNode(ASTNodeType::kASSIGN, "simple_statement", "=", drv);
        $$->AddChild($1);
        $$->AddChild(bin_op);
      }
    }
    | declarator 
    { $$ = $1; }
    | call
    { $$ = $1; }
;

simple_optional:
    /* empty */
    {
      $$ = {};
    }
    | simple_statement
    {
      $$ = $1;
    }
;

declarator:
    type VALUE_ID
    {
      $$ = MakeASTTreeNode(ASTNodeType::kDECLARE, $1, $2, drv);
    }
    | type VALUE_ID ASSIGN expression
    {
      $$ = MakeASTTreeNode(ASTNodeType::kDECLARE, $1, $2, drv);
      auto assign_node = MakeASTTreeNode(ASTNodeType::kASSIGN, $1, "=", drv);
      
      assign_node->AddChild(
        MakeASTTreeNode(ASTNodeType::kIDENT, "declarator", $2, drv));
      assign_node->AddChild($4);  

      $$->AddChild(assign_node);
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

control:
    IF LEFT_BRACKET expression RIGHT_BRACKET statement ELSE statement
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      $$->AddChild($3);
      $$->AddChild($5);
      $$->AddChild($7);
    }
    | IF LEFT_BRACKET expression RIGHT_BRACKET statement %prec IFX
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      $$->AddChild($3);
      $$->AddChild($5);
    } 
    | WHILE LEFT_BRACKET expression RIGHT_BRACKET statement
    {
      $$ = MakeASTTreeNode(ASTNodeType::kWHILE, "while", {}, drv);
      $$->AddChild($3, ASTNodeTag::kCOND);
      $$->AddChild($5, ASTNodeTag::kBODY);
    }
    | FOR LEFT_BRACKET { EnterScope(drv); } simple_optional SEMICOLON expression SEMICOLON simple_optional RIGHT_BRACKET statement
    {
      $$ = MakeASTTreeNode(ASTNodeType::kWHILE, "for", {}, drv);
      $$->AddChild($4, ASTNodeTag::kINIT);
      $$->AddChild($6, ASTNodeTag::kCOND);
      $$->AddChild($8, ASTNodeTag::kSTEP);
      $$->AddChild($10, ASTNodeTag::kBODY);

      LeaveScope(drv);
    } 
    | CONTINUE SEMICOLON
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCONTINUE, "continue", {}, drv);
    }
    | BREAK SEMICOLON
    {
      $$ = MakeASTTreeNode(ASTNodeType::kBREAK, "break", {}, drv);
    }
    | RETURN expression SEMICOLON
    {
      $$ = MakeASTTreeNode(ASTNodeType::kRETURN, "return_statement", {}, drv);
      $$->AddChild($2);
    }
;

expression:
    LEFT_BRACKET expression RIGHT_BRACKET   { $$ = $2; }
    | call { $$ = $1; }
    | expression PLUS expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PLUS", "+", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression SUB expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SUB", "-", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression MULT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "MULT", "*", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression SLASH expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "SLASH", "/", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression PERCENT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "PERCENT", "%", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression LT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT", "<", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression LT_EQ expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LT_EQ", "<=", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression GT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT", ">", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression GT_EQ expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "GT_EQ", ">=", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression EQ expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "EQ", "==", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression NOT_EQ expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "NOT_EQ", "!=", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression LOGIC_AND expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_AND", "&&", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression LOGIC_OR expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LOGIC_OR", "||", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression BIT_AND expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_AND", "&", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression BIT_EX_OR expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_EX_OR", "^", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression BIT_OR expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "BIT_OR", "|", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression LEFT_SHIFT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "LEFT_SHIFT", "<<", drv); $$->AddChild($1); $$->AddChild($3); }
    | expression RIGHT_SHIFT expression
        { $$ = MakeASTTreeNode(ASTNodeType::kBIN_OP, "RIGHT_SHIFT", ">>", drv); $$->AddChild($1); $$->AddChild($3); }

    // ?:
    | expression QUESTION expression COLON expression %prec CON_EXP
        { $$ = MakeASTTreeNode(ASTNodeType::kCOND_EXP, "conditional_expression", {}, drv); $$->AddChild($1); $$->AddChild($3); $$->AddChild($5); }

    // ! ~ -
    | LOGIC_NOT expression %prec UMINUS
        { $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "LOGIC_NOT", "!", drv); $$->AddChild($2); }
    | BIT_NOT expression %prec UMINUS
        { $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "BIT_NOT", "~", drv); $$->AddChild($2); }
    | SUB expression %prec UMINUS
        { $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "SUB", "-", drv); $$->AddChild($2); }

    // ident and const
    | VALUE_INTEGER
        { $$ = MakeASTTreeNode(ASTNodeType::kVALUE, "int", $1, drv); }
    | VALUE_ID
        { $$ = MakeASTTreeNode(ASTNodeType::kIDENT, "reference", $1, drv); }
    | TRUE
        { $$ = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "true", drv); }
    | FALSE
        { $$ = MakeASTTreeNode(ASTNodeType::kVALUE, "bool", "false", drv); }
;

assign_operator:
      ASSIGN              { $$ = "="; }
    | PLUS_ASSIGN         { $$ = "+="; }
    | SUB_ASSIGN          { $$ = "-="; }
    | MULT_ASSIGN         { $$ = "*="; }
    | SLASH_ASSIGN        { $$ = "/="; }
    | PERCENT_ASSIGN      { $$ = "%="; }
    | BIT_AND_ASSIGN      { $$ = "&="; }
    | BIT_EX_OR_ASSIGN    { $$ = "^="; }
    | BIT_OR_ASSIGN       { $$ = "|="; }
    | LEFT_SHIFT_ASSIGN   { $$ = "<<="; }
    | RIGHT_SHIFT_ASSIGN  { $$ = ">>="; }
;

call:
    VALUE_ID LEFT_BRACKET arg_list RIGHT_BRACKET
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + $1, drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | PRINT LEFT_BRACKET arg_list RIGHT_BRACKET
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "print", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | READ LEFT_BRACKET arg_list RIGHT_BRACKET
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "read", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | FLUSH LEFT_BRACKET arg_list RIGHT_BRACKET
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "flush", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  ;

arg_list:
    /* empty */              { $$ = nullptr; }
  | expression arg_list_follow
    {
      $$ = MakeASTTreeNode(ASTNodeType::kARG_LIST, "arg_list", {}, drv);
      $$->AddChild($1);
      for (auto& a : $2) $$->AddChild(a);
    }
  ;

arg_list_follow:
    /* empty */              { $$ = {}; }
  | COMMA expression arg_list_follow
    {
      $3.insert($3.begin(), $2);
      $$ = std::move($3);
    }
  ;

%%
