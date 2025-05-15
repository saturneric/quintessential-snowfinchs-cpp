%{
  #include "Driver.hpp"
  #include "Parser.hpp"
  #include "Utils.h"

  using token = yy::parser::token;
  int comment_depth = 0;
%}

%option c++
%option noyywrap nounput noinput batch debug
%option outfile="Lexer.yy.cpp"

%x COMMENT
%%

"/*" {
  comment_depth = 1;
  BEGIN(COMMENT);
}

<COMMENT>{
  "/*" {
    ++comment_depth;
  }

  "*/" {
    --comment_depth;
    if (comment_depth == 0) {
      BEGIN(INITIAL);
    }
  }

  .|\n ;
}

[ \t\r\n]+          ;
"//".*              ;

"{"                 { return yy::parser::make_OPENING_BRACE(); }
"}"                 { return yy::parser::make_CLOSING_BRACE(); }
";"                 { return yy::parser::make_SEMICOLON(); }
"("                 { return yy::parser::make_LEFT_BRACKET(); }
")"                 { return yy::parser::make_RIGHT_BRACKET(); }
"<<"                { return yy::parser::make_LEFT_SHIFT(); }
">>"                { return yy::parser::make_RIGHT_SHIFT(); }
"&"                 { return yy::parser::make_AND(); }
"^"                 { return yy::parser::make_INSERT(); }
"|"                 { return yy::parser::make_DELIMITER(); }
","                 { return yy::parser::make_COMMA(); }
"="                 { return yy::parser::make_EQUAL(); }
"+="                { return yy::parser::make_PLUS_EQUAL(); }
"-="                { return yy::parser::make_SUB_EQUAL(); }
"*="                { return yy::parser::make_MULT_EQUAL(); }
"/="                { return yy::parser::make_SLASH_EQUAL(); }
"%="                { return yy::parser::make_PERCENT_EQUAL(); }
"*"                 { return yy::parser::make_MULT(); }
"+"                 { return yy::parser::make_PLUS(); }
"-"                 { return yy::parser::make_SUB(); }
"~"                 { return yy::parser::make_TILDE(); }
"/"                 { return yy::parser::make_SLASH(); }
"%"                 { return yy::parser::make_PERCENT(); }

"struct"            { return yy::parser::make_STRUCT(); }
"if"                { return yy::parser::make_IF(); }
"else"              { return yy::parser::make_ELSE(); }
"while"             { return yy::parser::make_WHILE(); }
"for"               { return yy::parser::make_FOR(); }
"continue"          { return yy::parser::make_CONTINUE(); }
"break"             { return yy::parser::make_BREAK(); }
"return"            { return yy::parser::make_RETURN(); }
"assert"            { return yy::parser::make_ASSERT(); }
"print"             { return yy::parser::make_PRINT(); }
"read"              { return yy::parser::make_READ(); }
"alloc"             { return yy::parser::make_ALLOC(); }
"alloc_array"       { return yy::parser::make_ALLOC_ARRAY(); }

"true"              { return yy::parser::make_TRUE(); }
"false"             { return yy::parser::make_FALSE(); }
"NULL"              { return yy::parser::make_KW_NULL(); }
"string"            { return yy::parser::make_STRING(); }
"bool"              { return yy::parser::make_BOOLEAN(); }
"void"              { return yy::parser::make_VOID(); }
"char"              { return yy::parser::make_CHAR(); }
"int"               { return yy::parser::make_INT(); }

"main"             { return yy::parser::make_MAIN_FUNC_ID("main"); }

0[xX][A-Fa-f0-9]+       {
                          return yy::parser::make_VALUE_INTEGER(YYText());
                        }

0                       { return yy::parser::make_VALUE_INTEGER("0"); }

[1-9][0-9]*             { 
                          return yy::parser::make_VALUE_INTEGER(YYText());
                        }

[A-Za-z_][A-Za-z0-9_]*  {
                          return yy::parser::make_VALUE_ID(YYText());
                        }

.                       {   
                          std::cerr << "Illegal character: " << YYText() << std::endl;
                          return yy::parser::make_NONE();
                        }

%%
