//
// Created by Administrator on 2021/5/1.
//

#ifndef SYNTAXPARSER_AUTOMATA_H
#define SYNTAXPARSER_AUTOMATA_H

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class Automata {
 public:
  std::ifstream input;

  explicit Automata(const std::string& path) : input(path, std::ios::binary) {
    input.imbue(input.getloc());
  }

  ~Automata() { input.close(); }

  void parse() {
    while (!ifeof) {
      tokens.push_back(nextToken());
    }
  }

  void output();

 private:
  using TokenType = enum {
    /* Reserve Words */
    STRUCT,
    IF,
    ELSE,
    WHILE,
    FOR,
    CONTINUE,
    BREAK,
    RETURN,
    ASSERT,
    PRINT,
    READ,
    ALLOC,
    ALLOC_ARRAY,
    /* Special Symbols */
    OPENING_BRACE,
    CLOSING_BRACE,
    SEMICOLON,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    MULT,
    PLUS,
    SUB,
    TILDE,
    SLASH,
    PERCENT,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    AND,
    INSERT,
    DELIMITER,
    COMMA,
    EQUAL,
    PLUS_EQUAL,
    SUB_EQUAL,
    MULT_EQUAL,
    SLASH_EQUAL,
    PERCENT_EQUAL,
    /* Multicharacter Tokens */
    ID,
    LETTER,
    DIGIT,
    UNDERLINE,
    T_NULL,
    T_TRUE,
    T_FALSE,
    INTEGER,
    INTEGER_TYPE_SUFFIX,
    STRING,
    BOOLEAN,
    VOID,
    CHAR,
    INT,
    /* None & Error & EOF */
    NONE,
    ERROR,
    T_EOF
  };

  using StateType = enum {
    START,
    S_LETTER,
    S_UNDERLINE,
    S_DIGIT,
    INT_0,
    INT_NOT_0,
    INT_TYPE_SUFFIX,
    STRING_START,
    STRING_END,
    S_SIGN,
    DONE,
    S_NONE
  };

  using ReservedWord = struct {
    std::string str;
    TokenType token;
  };

  struct TokenInfo {
    const int line;
    const TokenType token;
    const std::string str;
    const StateType state;

    TokenInfo(const int line, const TokenType token, std::string str,
              StateType state)
        : line(line), token(token), str(std::move(str)), state(state) {}
  };

  FILE* fp = nullptr;

  bool ifeof = false;

  std::vector<TokenInfo> tokens;

  int line = 1;

  const static std::vector<ReservedWord> reservedWords;

  const static std::map<TokenType, std::string> tokenTypeStrMap;

  const static std::map<StateType, std::string> stateTypeStrMap;

  char nextChar();

  void pushBackChar();

  TokenInfo nextToken();

  static TokenType reservedLookup(const std::string& s);
};

#endif  // SYNTAXPARSER_AUTOMATA_H
