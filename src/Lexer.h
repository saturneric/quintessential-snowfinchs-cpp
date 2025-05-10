//
// Created by Administrator on 2021/5/1.
//

#pragma once

#include <map>
#include <queue>
#include <regex>
#include <string>
#include <utility>
#include <vector>

constexpr int kTokenIndexStart = 1 << 16;

struct LexerTokenSpec {
  int id;
  std::string name;
  std::string pattern;
};

struct LexerToken {
  LexerTokenSpec spec;
  std::string value;

  LexerToken(LexerTokenSpec spec, std::string value)
      : spec(std::move(spec)), value(std::move(value)) {}

  [[nodiscard]] auto Id() const -> int { return spec.id; }
  [[nodiscard]] auto Name() const -> std::string { return spec.name; }
};

class Lexer {
 public:
  Lexer() = default;

  void Lex(const std::string& path);

  void LoadTokenSpecs(const std::string& path);

  void Print(const std::string& path);

  [[nodiscard]] auto Tokens() const -> std::queue<LexerToken>;

  [[nodiscard]] auto TokenSpecs() const -> std::vector<LexerTokenSpec>;

 private:
  std::vector<LexerToken> tokens_;
  std::vector<LexerTokenSpec> token_specs_;
  std::map<int, LexerTokenSpec> token_specs_index_;
  std::map<int, std::regex> regexes_;
};
