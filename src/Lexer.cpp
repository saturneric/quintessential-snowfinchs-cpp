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

        auto token_spec_symbol = token_specs_index_[id];
        assert(token_spec_symbol != nullptr);

        auto token_symbol = symbol_table_->AddSymbol(SymbolType::kTOKEN, value,
                                                     token_spec_symbol->Name());

        tokens_.emplace_back(token_symbol);

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
    if (!(infile >> pattern)) break;

    auto symbol =
        symbol_table_->AddSymbol(SymbolType::kTOKEN_SPEC, name, pattern);

    token_specs_.push_back(symbol);
    token_specs_index_.insert({symbol->Index(), symbol});
  }

  for (const auto& spec : token_specs_) {
    regexes_.insert({spec->Index(), std::regex("^(" + spec->Value() + ")")});
  }
}

void Lexer::Print(const std::string& path) {
  std::ofstream f(path);

  for (const auto& token : tokens_) {
    f << token->Name() << "(" << token->Index() << ")" << "[" << token->Value()
      << "]"
      << "\n";
  }

  f.close();
}

auto Lexer::Tokens() const -> std::queue<SymbolPtr> {
  return std::queue<SymbolPtr, std::deque<SymbolPtr>>{
      std::deque<SymbolPtr>{tokens_.cbegin(), tokens_.cend()}};
}

auto Lexer::TokenSpecs() const -> std::vector<SymbolPtr> {
  return token_specs_;
}

Lexer::Lexer(std::shared_ptr<class SymbolTable> symbol_table)
    : symbol_table_(std::move(symbol_table)) {}

auto Lexer::SymbolTable() const -> std::shared_ptr<class SymbolTable> {
  return symbol_table_;
}
