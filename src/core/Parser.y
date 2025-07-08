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
%type <ASTNodePtr> root_declaration function struct_def field
%type <ASTNodePtr> program
%type <std::vector<ASTNodePtr>> root_declarations field_list
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
// ; ( ) [ ] { } ,
%token SEMICOLON LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET OPENING_BRACE CLOSING_BRACE COMMA
// << >> & ^ |
%token LEFT_SHIFT RIGHT_SHIFT BIT_AND BIT_EX_OR BIT_OR
// && ||
%token LOGIC_AND LOGIC_OR
// ? :
%token QUESTION COLON
// < <= > >= == !=
%token LT LT_EQ GT GT_EQ EQ NOT_EQ
// function
%token PRINT READ FLUSH ALLOC ALLOC_ARRAY
// struct -> .
%token STRUCT ARROW DOT

%token IF ELSE WHILE FOR CONTINUE BREAK RETURN 
%token ASSERT
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

%left LEFT_BRACKET RIGHT_BRACKET  // [ ]
%left DOT ARROW

%right DEREF

%start program

%%
program:
    root_declarations
    {
      $$ = MakeASTTreeNode(ASTNodeType::kPROGRAM, "program", {}, drv);
      drv.SetSyntaxTreeRoot($$);
      for (auto decl : $1)
        $$->AddChild(decl);
    }
;

root_declarations:
    /* empty */
    {
      $$ = std::vector<ASTNodePtr>();
    }
  | root_declaration root_declarations
    {
      $2.insert($2.begin(), $1);
      $$ = $2;
    }
;

root_declaration:
    function
    {
      $$ = $1;
    }
  | struct_def
    {
      $$ = $1;
    }
;

function:
    type VALUE_ID { EnterScope(drv); } LEFT_PAREN param_list RIGHT_PAREN block
    {
      $$ = MakeASTTreeNode(ASTNodeType::kFUNCTION, $1, std::string("__func_") + $2, drv);
      if ($5) $$->AddChild($5, ASTNodeTag::kPARAMS);
      $$->AddChild($7, ASTNodeTag::kBODY);
      LeaveScope(drv);
    }
;

struct_def:
    STRUCT VALUE_ID OPENING_BRACE field_list CLOSING_BRACE SEMICOLON
    {
      $$ = MakeASTTreeNode(ASTNodeType::kSTRUCT, std::string("__struct_") +$2, {}, drv);
      for (auto &f : $4) $$->AddChild(f);
    }
;

field_list:
    /* empty */
    { 
      $$ = std::vector<ASTNodePtr>();
    }
  | field field_list
    {
      $2.insert($2.begin(), $1);
      $$ = $2;
    }
;

field:
    type VALUE_ID SEMICOLON
    {
      $$ = MakeASTTreeNode(ASTNodeType::kFIELD, $2, $1, drv);
    }
;

param_list:
    /* empty */
    {
      $$ = nullptr;
    }
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
      INT                     { $$ = "int"; }
    | BOOLEAN                 { $$ = "bool"; }
    | STRUCT VALUE_ID         { $$ = std::string("struct ") + $2; }
    | type MULT               /* Pointer */
      {
        $$ = $1 + "*";
      }
    | type LEFT_BRACKET RIGHT_BRACKET  /* Array */
      {
        $$ = $1 + "[]";
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
        
        if($1->Type() == ASTNodeType::kIDENT) {
          bin_op->AddChild(MakeASTTreeNode(ASTNodeType::kIDENT, "reference", $1->Symbol()->Name(), drv));
        } else {
          bin_op->AddChild($1);
        }

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
      $$ = MakeASTTreeNode(ASTNodeType::kIDENT, "lvalue", $1, drv);
    }
    | LEFT_PAREN left_value RIGHT_PAREN
    {
      $$ = $2;
    }
    | MULT left_value /* *lvalue */ %prec DEREF
    {
      $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "lvalue", "*", drv);
      $$->AddChild($2);
    }
    | left_value LEFT_BRACKET expression RIGHT_BRACKET  /* lvalue[exp] */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kARRAY_ACCESS, "subscript", "lvalue", drv);
      $$->AddChild($1);
      $$->AddChild($3);
    }
    | left_value DOT VALUE_ID    /* lvalue.field */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "dot", $3, drv);
      $$->AddChild($1);
    }
    | left_value ARROW VALUE_ID  /* lvalue->field */
    {
      /* equals to (*lvalue).field */
      auto deref = MakeASTTreeNode(ASTNodeType::kUN_OP, "lvalue", "*", drv);
      deref->AddChild($1);

      $$ = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "arrow", $3, drv);
      $$->AddChild(deref);
    }
;

control:
    IF LEFT_PAREN expression RIGHT_PAREN statement ELSE statement
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      $$->AddChild($3);
      $$->AddChild($5);
      $$->AddChild($7);
    }
    | IF LEFT_PAREN expression RIGHT_PAREN statement %prec IFX
    {
      $$ = MakeASTTreeNode(ASTNodeType::kIF, "control", {}, drv);
      $$->AddChild($3);
      $$->AddChild($5);
    } 
    | WHILE LEFT_PAREN expression RIGHT_PAREN statement
    {
      $$ = MakeASTTreeNode(ASTNodeType::kWHILE, "while", {}, drv);
      $$->AddChild($3, ASTNodeTag::kCOND);
      $$->AddChild($5, ASTNodeTag::kBODY);
    }
    | FOR LEFT_PAREN { EnterScope(drv); } simple_optional SEMICOLON expression SEMICOLON simple_optional RIGHT_PAREN statement
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
    LEFT_PAREN expression RIGHT_PAREN   { $$ = $2; }
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

    | expression DOT VALUE_ID    /* exp.field */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "dot", "exp", drv);
      $$->AddChild($1);
      $$->AddChild(MakeASTTreeNode(ASTNodeType::kIDENT, "field", $3, drv));
    }
    | expression ARROW VALUE_ID  /* exp->field */
    {
      /* equals to (*exp).field */
      auto deref = MakeASTTreeNode(ASTNodeType::kUN_OP, "deref", "*", drv);
      deref->AddChild($1);

      $$ = MakeASTTreeNode(ASTNodeType::kFIELD_ACCESS, "arrow", "exp", drv);
      $$->AddChild(deref);
      $$->AddChild(MakeASTTreeNode(ASTNodeType::kIDENT, "field", $3, drv));
    }
    | MULT expression /* *exp */ %prec DEREF
    {
      $$ = MakeASTTreeNode(ASTNodeType::kUN_OP, "deref", "*", drv);
      $$->AddChild($2);
    }
    | expression LEFT_BRACKET expression RIGHT_BRACKET  /* exp[exp] */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kARRAY_ACCESS, "subscript", "exp", drv);
      $$->AddChild($1);
      $$->AddChild($3);
    }
    | KW_NULL
    { 
      $$ = MakeASTTreeNode(ASTNodeType::kVALUE, "nullptr", "0", drv); 
    }
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
    VALUE_ID LEFT_PAREN arg_list RIGHT_PAREN
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + $1, drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | PRINT LEFT_PAREN arg_list RIGHT_PAREN
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "print", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | READ LEFT_PAREN arg_list RIGHT_PAREN
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "read", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | FLUSH LEFT_PAREN arg_list RIGHT_PAREN
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") + "flush", drv);
      if ($3) {
        for (auto child : $3->Children())
          $$->AddChild(child);
      }
    }
  | ALLOC LEFT_PAREN type RIGHT_PAREN         /* alloc(type) */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") +"alloc", drv);
      $$->AddChild(MakeASTTreeNode(ASTNodeType::kTYPE, "type", $3, drv));
    }
  | ALLOC_ARRAY LEFT_PAREN type COMMA expression RIGHT_PAREN  /* alloc_array(type, exp) */
    {
      $$ = MakeASTTreeNode(ASTNodeType::kCALL, "call", std::string("__func_") +"alloc_array", drv);
      $$->AddChild(MakeASTTreeNode(ASTNodeType::kTYPE, "type", $3, drv));
      $$->AddChild($5);
    }
;
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
