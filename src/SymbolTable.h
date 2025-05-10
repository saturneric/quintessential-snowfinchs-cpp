#pragma once

#include <Symbol.h>

#include <map>
#include <string>
#include <vector>

#include "Lexer.h"

const std::string kEmptySymbol = "ε";
const std::string kStopSymbol = "$";
const std::string kEOFSymbol = "EOF";

const int kEmptySymbolId = 0;
const int kStopSymbolId = -1;
const int kEOFSymbolId = 1;

const char kStartSymbolPrefix = '@';

const struct LexerTokenSpec kTokenSpecEOF = {kEOFSymbolId, kEOFSymbol, {}};
const struct LexerTokenSpec kTokenSpecStop = {kStopSymbolId, kStopSymbol, {}};

class SymbolTable {
  int index = 1 << 10;

  std::map<std::string, Symbol *> table;

  std::map<int, Symbol *> cache;

  std::vector<const Symbol *> line;

 public:
  SymbolTable();

  [[nodiscard]] auto GetAllSymbols() const
      -> const std::vector<const Symbol *> &;

  auto AddSymbol(int index, const std::string &name, bool terminator,
                 bool start) -> int;

  auto AddSymbol(const std::string &name, bool terminator) -> int;

  [[nodiscard]] auto GetSymbol(int symbol_index) const -> const Symbol *;

  [[nodiscard]] auto GetSymbolIndex(const std::string &name) const -> int;

  void ModifySymbol(int idx, const std::string &name, bool terminator,
                    bool start);

  [[nodiscard]] auto GetStartSymbol() const -> const Symbol *;
};
