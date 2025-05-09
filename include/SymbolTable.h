//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_SYMBOLTABLE_H
#define SYNTAXPARSER_SYMBOLTABLE_H

#include <Symbol.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class SymbolTable {
  int index = 1;

  std::map<std::string, Symbol *> table;

  std::map<int, Symbol *> cache;

  std::vector<const Symbol *> line;

 public:
  SymbolTable();

  [[nodiscard]] const std::vector<const Symbol *> &getAllSymbols() const {
    return line;
  }

  int addSymbol(const std::string &name, bool terminator);

  [[nodiscard]] const Symbol *getSymbol(int symbol_index) const;

  [[nodiscard]] int getSymbolIndex(const std::string &name) const;

  void modifySymbol(int idx, const std::string &name, bool terminator,
                    bool start);

  [[nodiscard]] const Symbol *getStartSymbol() const;
};

#endif  // SYNTAXPARSER_SYMBOLTABLE_H
