#include "Lexer.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

void Lexer::Lex(const std::string& path) {
  std::ifstream infile(path);
  if (!infile) {
    std::cerr << "Failed to open source file: " << path << "\n";
    return;
  }

  std::string input((std::istreambuf_iterator<char>(infile)),
                    std::istreambuf_iterator<char>());
  auto start = input.cbegin();

  while (start != input.end()) {
    bool matched = false;

    if (std::isspace(*start) != 0) {
      ++start;
      continue;
    }

    for (const auto& [id, re] : regexes_) {
      std::smatch match;
      if (std::regex_search(start, input.cend(), match, re)) {
        const auto value = match.str(1);

        assert(token_specs_index_.count(id) != 0);
        tokens_.emplace_back(token_specs_index_[id], value);

        start += static_cast<long>(value.size());
        matched = true;
        break;
      }
    }

    if (!matched) {
      std::cout << "Unexpected token at: " << std::string{start, input.cend()}
                << "\n";
      break;
    }
  }
}

void Lexer::LoadTokenSpecs(const std::string& path) {
  std::ifstream infile(path);
  std::string name;
  std::string pattern;

  int id = kTokenIndexStart;
  token_specs_.clear();

  while (infile >> name) {
    if (name == "NONE") continue;
    if (!(infile >> pattern)) break;
    token_specs_.push_back({id++, name, pattern});
    token_specs_index_.insert({token_specs_.back().id, token_specs_.back()});
  }

  for (const auto& spec : token_specs_) {
    regexes_.insert({spec.id, std::regex("^(" + spec.pattern + ")")});
  }
}

void Lexer::Print(const std::string& path) {
  std::ofstream f(path);

  for (const auto& token : tokens_) {
    f << token.Name() << "(" << token.Id() << ")" << "[" << token.value << "]"
      << "\n";
  }

  f.close();
}

auto Lexer::Tokens() const -> std::queue<LexerToken> {
  return std::queue<LexerToken, std::deque<LexerToken>>{
      std::deque<LexerToken>{tokens_.cbegin(), tokens_.cend()}};
}

auto Lexer::TokenSpecs() const -> std::vector<LexerTokenSpec> {
  return token_specs_;
}
