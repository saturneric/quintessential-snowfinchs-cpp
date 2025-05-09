//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_GRAMMARRESOURCEPOOL_H
#define SYNTAXPARSER_GRAMMARRESOURCEPOOL_H

#include <Production.h>
#include <SymbolTable.h>

#include <iostream>
#include <set>
#include <sstream>

// �﷨��Դ��
class GrammarResourcePool {
  int pdt_index = 0;

  // ���ű�
  SymbolTable symbolTable;

  // ����ʽ
  std::vector<const Production *> productions;

  // FIRST����洢��
  std::map<int, const std::set<int> *> firsts;

  // FOLLOW����洢��
  std::map<int, std::set<int> *> follows;

  // ȥ����β�ո�
  static std::string &trim(std::string &&str) {
    if (str.empty()) {
      return str;
    }

    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
    return str;
  }

 public:
  const std::set<int> *FIRST(const std::vector<int> &symbols, int start_index);

  const std::set<int> *FIRST(int symbol);

  const std::set<int> *FOLLOW(int symbol);

  void FOLLOW();

  std::set<int> *get_follow_set(int symbol);

  void print_symbols(const std::set<int> &symbols_index);

  void parse_production_string_line(const std::string &temp_line);

  [[nodiscard]] const std::vector<const Production *> &get_productions() const {
    return productions;
  }

  [[nodiscard]] const Symbol *getSymbol(int symbol_index) const {
    return symbolTable.getSymbol(symbol_index);
  }

  [[nodiscard]] const Symbol *getStartSymbol() const {
    return symbolTable.getStartSymbol();
  }

  int addSymbol(const std::string &name, bool terminator) {
    return symbolTable.addSymbol(name, terminator);
  }

  const Production *addProduction(int left, std::initializer_list<int> right);

  [[nodiscard]] const std::vector<const Symbol *> &getAllSymbols() const {
    return symbolTable.getAllSymbols();
  }

  void modifySymbol(int index, const std::string &name, bool terminator,
                    bool start) {
    symbolTable.modifySymbol(index, name, terminator, start);
  }

  [[nodiscard]] int getSymbolIndex(const std::string &name) const {
    return symbolTable.getSymbolIndex(name);
  }
};

#endif  // SYNTAXPARSER_GRAMMARRESOURCEPOOL_H
