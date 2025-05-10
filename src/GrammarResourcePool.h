#pragma once

#include <Production.h>
#include <SymbolTable.h>

#include <set>

class GrammarResourcePool {
 public:
  using SymbolSet = std::set<int>;
  using SymbolSetPtr = std::shared_ptr<SymbolSet>;

  auto FIRST(const std::vector<int> &symbols, int start_index) -> SymbolSetPtr;

  auto FIRST(int symbol) -> SymbolSetPtr;

  auto FOLLOW(int symbol) -> SymbolSetPtr;

  void FOLLOW();

  auto GetFollowSet(int symbol) -> SymbolSetPtr;

  void PrintSymbols(const std::set<int> &symbols_index);

  void ParseProductionStringLine(const std::string &temp_line);

  [[nodiscard]] auto GetProductions() const
      -> const std::vector<ProductionPtr> &;

  [[nodiscard]] auto GetSymbol(int symbol_index) const -> SymbolPtr;

  [[nodiscard]] auto GetSymbolByToken(const std::string &token) const
      -> SymbolPtr;

  [[nodiscard]] auto GetStartSymbol() const -> SymbolPtr;

  auto AddSymbol(const std::string &name, bool terminator) -> SymbolPtr;

  auto AddProduction(int left, const std::vector<int> &right) -> ProductionPtr;

  [[nodiscard]] auto GetAllSymbols() const -> std::vector<SymbolPtr>;

  void ModifySymbol(int index, const std::string &name, bool terminator,
                    bool start);

  [[nodiscard]] auto GetSymbolIndex(const std::string &name) const -> int;

 private:
  int pdt_index_ = 0;
  SymbolTable symbol_table_;
  std::vector<ProductionPtr> productions_;
  std::map<int, SymbolSetPtr> firsts_;
  std::map<int, SymbolSetPtr> follows_;

  static auto trim(const std::string &str) -> std::string;
};
