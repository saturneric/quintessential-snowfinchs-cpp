#pragma once

#include <Production.h>
#include <SymbolTable.h>

#include <set>

class GrammarResourcePool {
 public:
  auto FIRST(const std::vector<int> &symbols, int start_index)
      -> const std::set<int> *;

  auto FIRST(int symbol) -> const std::set<int> *;

  auto FOLLOW(int symbol) -> const std::set<int> *;

  void FOLLOW();

  auto GetFollowSet(int symbol) -> std::set<int> *;

  void PrintSymbols(const std::set<int> &symbols_index);

  void ParseProductionStringLine(const std::string &temp_line);

  [[nodiscard]] auto GetProductions() const
      -> const std::vector<ProductionPtr> &;

  [[nodiscard]] auto GetSymbol(int symbol_index) const -> const Symbol *;

  [[nodiscard]] auto GetStartSymbol() const -> const Symbol *;

  auto AddSymbol(const std::string &name, bool terminator) -> int;

  auto AddSymbolToken(int index, const std::string &name) -> int;

  auto AddProduction(int left, std::initializer_list<int> right)
      -> ProductionPtr;

  [[nodiscard]] auto GetAllSymbols() const
      -> const std::vector<const Symbol *> &;

  void ModifySymbol(int index, const std::string &name, bool terminator,
                    bool start);

  [[nodiscard]] auto GetSymbolIndex(const std::string &name) const -> int;

 private:
  int pdt_index_ = 0;
  SymbolTable symbol_table_;
  std::vector<ProductionPtr> productions_;
  std::map<int, const std::set<int> *> firsts_;
  std::map<int, std::set<int> *> follows_;

  static auto trim(std::string &&str) -> std::string &;
};
