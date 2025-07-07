%{
  #include "core/Driver.hpp"
  #include "core/Parser.hpp"
  #include "core/Utils.h"

  using token = yy::parser::token;
  int comment_depth = 0;

  #define YY_USER_ACTION  loc.columns (yyleng);
  #define yyterminate()   return yy::parser::make_YYEOF(loc);
%}

%option c++
%option noyywrap nounput noinput batch debug
%option outfile="Lexer.yy.cpp"

%x COMMENT
%%

%{
  yy::location& loc = drv.Location();
%}

"/*" {
  comment_depth = 1;
  BEGIN(COMMENT);
}

<COMMENT>{
  "/*"    {
    ++comment_depth;
  }

  "*/"    {
    --comment_depth;
    if (comment_depth == 0) {
      BEGIN(INITIAL);
    }
  }

  \n      { loc.lines (1); loc.step (); }

  <<EOF>> {
    drv.LexerError("Unterminated comment at EOF");
    yyterminate();
  }

  .       ;
}

[ \t\r]+            { loc.step (); }
[\n]+               { loc.lines (yyleng); loc.step (); }
"//".*              { loc.lines (1); loc.step (); }

"{"                 { return yy::parser::make_OPENING_BRACE(loc); }
"}"                 { return yy::parser::make_CLOSING_BRACE(loc); }
";"                 { return yy::parser::make_SEMICOLON(loc); }
"["                 { return yy::parser::make_LEFT_BRACKET(loc); }
"]"                 { return yy::parser::make_RIGHT_BRACKET(loc); }
"("                 { return yy::parser::make_LEFT_PAREN(loc); }
")"                 { return yy::parser::make_RIGHT_PAREN(loc); }
","                 { return yy::parser::make_COMMA(loc); }

"!"                 { return yy::parser::make_LOGIC_NOT(loc); }
"~"                 { return yy::parser::make_BIT_NOT(loc); }
"<<"                { return yy::parser::make_LEFT_SHIFT(loc); }
">>"                { return yy::parser::make_RIGHT_SHIFT(loc); }
"&"                 { return yy::parser::make_BIT_AND(loc); }
"^"                 { return yy::parser::make_BIT_EX_OR(loc); }
"|"                 { return yy::parser::make_BIT_OR(loc); }

"="                 { return yy::parser::make_ASSIGN(loc); }
"+="                { return yy::parser::make_PLUS_ASSIGN(loc); }
"-="                { return yy::parser::make_SUB_ASSIGN(loc); }
"*="                { return yy::parser::make_MULT_ASSIGN(loc); }
"/="                { return yy::parser::make_SLASH_ASSIGN(loc); }
"%="                { return yy::parser::make_PERCENT_ASSIGN(loc); }
"&="                { return yy::parser::make_BIT_AND_ASSIGN(loc); }
"^="                { return yy::parser::make_BIT_EX_OR_ASSIGN(loc); }
"|="                { return yy::parser::make_BIT_OR_ASSIGN(loc); }
"<<="               { return yy::parser::make_LEFT_SHIFT_ASSIGN(loc); }
">>="               { return yy::parser::make_RIGHT_SHIFT_ASSIGN(loc); }

"*"                 { return yy::parser::make_MULT(loc); }
"+"                 { return yy::parser::make_PLUS(loc); }
"-"                 { return yy::parser::make_SUB(loc); }
"/"                 { return yy::parser::make_SLASH(loc); }
"%"                 { return yy::parser::make_PERCENT(loc); }

"&&"                { return yy::parser::make_LOGIC_AND(loc); }
"||"                { return yy::parser::make_LOGIC_OR(loc); }

"<"                 { return yy::parser::make_LT(loc); }
"<="                { return yy::parser::make_LT_EQ(loc); }
">"                 { return yy::parser::make_GT(loc); }
">="                { return yy::parser::make_GT_EQ(loc); }
"=="                { return yy::parser::make_EQ(loc); }
"!="                { return yy::parser::make_NOT_EQ(loc); }

"?"                 { return yy::parser::make_QUESTION(loc); }
":"                 { return yy::parser::make_COLON(loc); }

"struct"            { return yy::parser::make_STRUCT(loc); }
"->"                 { return yy::parser::make_ARROW(loc); }
"."                 { return yy::parser::make_DOT(loc); }

"if"                { return yy::parser::make_IF(loc); }
"else"              { return yy::parser::make_ELSE(loc); }
"while"             { return yy::parser::make_WHILE(loc); }
"for"               { return yy::parser::make_FOR(loc); }
"continue"          { return yy::parser::make_CONTINUE(loc); }
"break"             { return yy::parser::make_BREAK(loc); }
"return"            { return yy::parser::make_RETURN(loc); }
"assert"            { return yy::parser::make_ASSERT(loc); }
"alloc"             { return yy::parser::make_ALLOC(loc); }
"alloc_array"       { return yy::parser::make_ALLOC_ARRAY(loc); }

"true"              { return yy::parser::make_TRUE(loc); }
"false"             { return yy::parser::make_FALSE(loc); }
"NULL"              { return yy::parser::make_KW_NULL(loc); }
"string"            { return yy::parser::make_STRING(loc); }
"bool"              { return yy::parser::make_BOOLEAN(loc); }
"void"              { return yy::parser::make_VOID(loc); }
"char"              { return yy::parser::make_CHAR(loc); }
"int"               { return yy::parser::make_INT(loc); }

"read"             { return yy::parser::make_READ(loc); }
"print"             { return yy::parser::make_PRINT(loc); }
"flush"             { return yy::parser::make_FLUSH(loc); }

0[xX][A-Fa-f0-9]+       {
                          return yy::parser::make_VALUE_INTEGER(YYText(), loc);
                        }

0                       { return yy::parser::make_VALUE_INTEGER("0", loc); }

[1-9][0-9]*             { 
                          return yy::parser::make_VALUE_INTEGER(YYText(), loc);
                        }

[A-Za-z_][A-Za-z0-9_]*  {
                          return yy::parser::make_VALUE_ID(YYText(), loc);
                        }

.                       {   
                          SPDLOG_ERROR("Illegal character: {}", YYText());
                          return yy::parser::make_NONE(loc);
                        }

%%
