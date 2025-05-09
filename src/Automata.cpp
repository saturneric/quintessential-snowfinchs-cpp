//
// Created by Administrator on 2021/5/1.
//

#include "Automata.h"

#include <iostream>
#include <sstream>
#include <string>

const std::vector<Automata::ReservedWord> Automata::reservedWords = {
    {"struct", STRUCT}, {"if", IF},
    {"else", ELSE},     {"while", WHILE},
    {"for", FOR},       {"continue", CONTINUE},
    {"break", BREAK},   {"return", RETURN},
    {"assert", ASSERT}, {"true", T_TRUE},
    {"false", T_FALSE}, {"NULL", T_NULL},
    {"print", PRINT},   {"read", READ},
    {"alloc", ALLOC},   {"alloc_array", ALLOC_ARRAY},
    {"int", INT},       {"bool", BOOLEAN},
    {"void", VOID},     {"char", CHAR},
    {"string", STRING}};

const std::map<Automata::StateType, std::string> Automata::stateTypeStrMap = {
    {START, "START"},
    {S_LETTER, "LETTER"},
    {S_UNDERLINE, "UNDERLINE"},
    {S_DIGIT, "DIGIT"},
    {INT_0, "INT_0"},
    {INT_NOT_0, "INT_NOT_0"},
    {INT_TYPE_SUFFIX, "INT_TYPE_SUFFIX"},
    {STRING_START, "STRING_START"},
    {STRING_END, "STRING_END"},
    {DONE, "DONE"},
    {S_SIGN, "SIGN"}};

const std::map<Automata::TokenType, std::string> Automata::tokenTypeStrMap = {
    {STRUCT, "STRUCT"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {WHILE, "WHILE"},
    {FOR, "FOR"},
    {BREAK, "DOUBLE"},
    {RETURN, "RETURN"},
    {ASSERT, "ASSERT"},
    {T_NULL, "NULL"},
    {PRINT, "PRINT"},
    {READ, "READ"},
    {ALLOC, "ALLOC"},
    {CHAR, "CHAR"},
    {INT, "INT"},
    {ALLOC_ARRAY, "ALLOC_ARRAY"},
    {OPENING_BRACE, "OPENING_BRACE"},
    {CLOSING_BRACE, "CLOSING_BRACE"},
    {SEMICOLON, "SEMICOLON"},
    {LEFT_BRACKET, "LEFT_BRACKET"},
    {RIGHT_BRACKET, "RIGHT_BRACKET"},
    {MULT, "MULT"},
    {PLUS, "PLUS"},
    {SUB, "SUB"},
    {TILDE, "TILDE"},
    {SLASH, "SLASH"},
    {EQUAL, "EQUAL"},
    {MULT_EQUAL, "MULT_EQUAL"},
    {PLUS_EQUAL, "PLUS_EQUAL"},
    {SUB_EQUAL, "SUB_EQUAL"},
    {SLASH_EQUAL, "SLASH_EQUAL"},
    {PERCENT_EQUAL, "PERCENT_EQUALs"},
    {LEFT_SHIFT, "LEFT_SHIFT"},
    {RIGHT_SHIFT, "RIGHT_SHIFT"},
    {AND, "AND"},
    {INSERT, "INSERT"},
    {DELIMITER, "DELIMITER"},
    {COMMA, "COMMA"},
    {ID, "ID"},
    {LETTER, "LETTER"},
    {DIGIT, "DIGIT"},
    {UNDERLINE, "UNDERLINE"},
    {T_TRUE, "TRUE"},
    {T_FALSE, "FALSE"},
    {INTEGER, "INTEGER"},
    {INTEGER_TYPE_SUFFIX, "INTEGER_TYPE_SUFFIX"},
    {STRING, "STRING"},
    {COMMA, "COMMA"},
    {NONE, "NONE"},
    {ERROR, "ERROR"},
    {T_EOF, "EOF"}};

void Automata::output() {
  std::ofstream stream("./tokenOut.txt", std::ios::binary | std::ios::trunc);
  int temp_line = 1;
  stream << "1 ";

  for (const auto& token : tokens) {
    if (token.line > temp_line) {
      temp_line = token.line;
      stream << '\n' << temp_line << ' ';
    }

    if (token.token == ERROR) {
      stream << tokenTypeStrMap.find(token.token)->second << '{' << token.str
             << ", " << stateTypeStrMap.find(token.state)->second << '}' << ' ';
    } else {
      stream << tokenTypeStrMap.find(token.token)->second << '(' << token.str
             << ')' << ' ';
    }
  }

  stream.close();
}

auto Automata::nextChar() -> char {
  char c;
  input >> std::noskipws >> c;
  if (input.eof()) {
    this->ifeof = true;
  }
  return c;
}

auto Automata::nextToken() -> Automata::TokenInfo {
  TokenType current_token = NONE;
  StateType state = START;
  StateType last_state = S_NONE;

  std::stringstream ss;
  while (state != DONE) {
    last_state = state;

    char ch = nextChar();

    bool save = true;
    if (ifeof) {
      current_token = T_EOF;
      break;
    }

    switch (state) {
      case START: {
        if (isdigit(ch) != 0) {
          if (ch == '0') {
            state = INT_0;
          } else {
            state = INT_NOT_0;
          }
        } else if (isalpha(ch) != 0) {
          state = S_LETTER;
        } else if (ch == '\"') {
          state = STRING_START;
        } else if ((ch == ' ') || (ch == '\t') || (ch == '\r')) {
          save = false;
        } else if (ch == '\n') {
          this->line++;
          save = false;
        } else {
          state = S_SIGN;

          switch (ch) {
            case '{':
              current_token = OPENING_BRACE;
              state = DONE;
              break;
            case '}':
              current_token = CLOSING_BRACE;
              state = DONE;
              break;
            case ';':
              current_token = SEMICOLON;
              state = DONE;
              break;
            case '(':
              current_token = LEFT_BRACKET;
              state = DONE;
              break;
            case ')':
              current_token = RIGHT_BRACKET;
              state = DONE;
              break;
            case '*': {
              current_token = MULT;
              auto n_ch = nextChar();
              if (n_ch == '=') {
                current_token = MULT_EQUAL;
                ss << ch;
                ch = n_ch;
              } else {
                pushBackChar();
              }
              state = DONE;
              break;
            }
            case '+': {
              current_token = PLUS;
              auto n_ch = nextChar();
              if (n_ch == '=') {
                current_token = PLUS_EQUAL;
                ss << ch;
                ch = n_ch;
              } else {
                pushBackChar();
              }
              state = DONE;
              break;
            }
            case '-': {
              current_token = SUB;
              auto n_ch = nextChar();
              if (n_ch == '=') {
                current_token = SUB_EQUAL;
                ss << ch;
                ch = n_ch;
              } else {
                pushBackChar();
              }
              state = DONE;
              break;
            }
            case '~':
              current_token = TILDE;
              break;
            case '/': {
              current_token = MULT;
              auto n_ch = nextChar();
              if (n_ch == '=') {
                current_token = MULT_EQUAL;
                ss << ch;
                ch = n_ch;
              } else {
                pushBackChar();
              }
              state = DONE;
              break;
            }
            case '%': {
              current_token = MULT;
              auto n_ch = nextChar();
              if (n_ch == '=') {
                current_token = MULT_EQUAL;
                ss << ch;
                ch = n_ch;
              } else {
                pushBackChar();
              }
              state = DONE;
              break;
            }
            case '>':
              ch = nextChar();
              if (ch == '>') {
                current_token = RIGHT_SHIFT;
                ss << ch;
              } else {
                pushBackChar();
                current_token = ERROR;
              }
              state = DONE;
              break;
            case '<':
              ch = nextChar();
              if (ch == '<') {
                current_token = LEFT_SHIFT;
                ss << ch;
              } else {
                pushBackChar();
                current_token = ERROR;
              }
              state = DONE;
              break;
            case '&':
              current_token = AND;
              state = DONE;
              break;
            case '^':
              current_token = INSERT;
              state = DONE;
              break;
            case '|':
              current_token = DELIMITER;
              state = DONE;
              break;
            case ',':
              current_token = COMMA;
              state = DONE;
              break;
            case '=':
              current_token = EQUAL;
              state = DONE;
              break;
            default:
              current_token = ERROR;
              state = DONE;
              break;
          }
        }
        break;
      }
      case S_LETTER: {
        if (ch == '_') {
          state = S_UNDERLINE;
        } else if (isdigit(ch)) {
          state = S_DIGIT;
        } else if (isalpha(ch)) {
          state = S_LETTER;
        } else {
          current_token = ID;
          pushBackChar();
          state = DONE;
          save = false;
        }
        break;
      }
      case S_DIGIT: {
        if (isalpha(ch)) {
          state = S_LETTER;
        } else if (isdigit(ch)) {
          state = S_DIGIT;
        } else {
          current_token = ID;
          pushBackChar();
          state = DONE;
          save = false;
        }
        break;
      }
      case S_UNDERLINE: {
        if (isdigit(ch)) {
          state = S_DIGIT;
        } else if (isalpha(ch)) {
          state = S_LETTER;
        } else {
          pushBackChar();
          current_token = ERROR;
          save = false;
        }
        break;
      }
      case INT_0: {
        if (ch == 'l' || ch == 'L') {
          state = INT_TYPE_SUFFIX;
        } else {
          current_token = INTEGER;
          pushBackChar();
          state = DONE;
          save = false;
        }
        break;
      }
      case INT_NOT_0: {
        if (ch == 'l' || ch == 'L') {
          state = INT_TYPE_SUFFIX;
        } else if (isdigit(ch)) {
          state = INT_NOT_0;
        } else {
          current_token = INTEGER;
          pushBackChar();
          state = DONE;
          save = false;
        }
        break;
      }
      case INT_TYPE_SUFFIX: {
        state = DONE;
        current_token = INTEGER;
        pushBackChar();
        save = false;
        break;
      }
      case STRING_START: {
        if (ch == '\\') {
          char buff_c = ch;
          ch = nextChar();

          if ((ch == 'b') || (ch == 't') || (ch == 'n') || (ch == 'f') ||
              (ch == 'r') || (ch == '"') || (ch == '\\')) {
            ss << buff_c;
            state = STRING_START;
          } else {
            pushBackChar();
            current_token = ERROR;
          }
        } else if (ch == '\"') {
          state = STRING_END;
        } else if (ch == ' ') {
          ss << '\\' << '4';
          ch = '0';
          state = STRING_START;
        } else {
          state = STRING_START;
        }
        break;
      }
      case STRING_END: {
        state = DONE;
        current_token = STRING;
        pushBackChar();
        save = false;
        break;
      }
      case S_SIGN: {
        state = DONE;
        pushBackChar();
        save = false;
        break;
      }
      case DONE:
      case S_NONE: {
        break;
      }
    }

    if (save) ss << ch;

    if (state == DONE) {
      const std::string token = ss.str();
      if (current_token == ID) {
        current_token = reservedLookup(token);
      }
    }
  }

  return {this->line, current_token, ss.str(), last_state};
}

auto Automata::reservedLookup(const std::string& s) -> Automata::TokenType {
  for (const auto& word : reservedWords) {
    if (word.str == s) return word.token;
  }
  return ID;
}

void Automata::pushBackChar() { input.seekg(-1L, std::ios::cur); }
