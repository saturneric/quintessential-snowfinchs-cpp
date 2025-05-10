#pragma once

#include <map>
#include <queue>
#include <regex>
#include <string>
#include <vector>

#include "SymbolTable.h"

constexpr int kTokenIndexStart = 1 << 16;

class Lexer {
 public:
  explicit Lexer(std::shared_ptr<SymbolTable> symbol_table);

  void Lex(const std::string& path);

  void LoadTokenSpecs(const std::string& path);

  void Print(const std::string& path);

  [[nodiscard]] auto Tokens() const -> std::queue<SymbolPtr>;

  [[nodiscard]] auto TokenSpecs() const -> std::vector<SymbolPtr>;

  [[nodiscard]] auto SymbolTable() const -> std::shared_ptr<SymbolTable>;

 private:
  std::shared_ptr<class SymbolTable> symbol_table_;

  std::vector<SymbolPtr> tokens_;
  std::vector<SymbolPtr> token_specs_;
  std::map<int, SymbolPtr> token_specs_index_;
  std::map<int, std::regex> regexes_;
};
