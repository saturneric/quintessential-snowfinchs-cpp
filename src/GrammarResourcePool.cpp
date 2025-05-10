#include "GrammarResourcePool.h"

#include <iostream>
#include <sstream>

auto GrammarResourcePool::FIRST(const std::vector<int> &symbols,
                                int start_index) -> const std::set<int> * {
  auto *non_terminator_symbols = new std::set<int>();

  for (int i = start_index; i < symbols.size(); i++) {
    const auto *const p_non_term_set = FIRST(symbols[i]);

    non_terminator_symbols->insert(p_non_term_set->begin(),
                                   p_non_term_set->end());

    const auto sec_it = p_non_term_set->find(0);
    if (sec_it != p_non_term_set->end()) continue;

    break;
  }

  return non_terminator_symbols;
}

auto GrammarResourcePool::FIRST(int symbol) -> const std::set<int> * {
  const auto it = firsts_.find(symbol);
  if (it != firsts_.end()) return it->second;

  auto *non_terminator_symbols = new std::set<int>();

  if (symbol_table_.GetSymbol(symbol)->terminator) {
    non_terminator_symbols->insert(symbol);
  } else {
    bool production_found = false;

    for (const auto &production : productions_) {
      auto p_pdt = production;

      if (p_pdt->left != symbol) continue;

      production_found = true;

      for (const auto &right_symbol : p_pdt->right) {
        const auto *const p_non_term_set = FIRST(right_symbol);

        non_terminator_symbols->insert(p_non_term_set->begin(),
                                       p_non_term_set->end());

        const auto sec_it = p_non_term_set->find(0);

        if (sec_it != p_non_term_set->end()) continue;

        break;
      }
    }

    if (!production_found) non_terminator_symbols->insert(0);
  }

  this->firsts_.insert(
      std::pair<int, const std::set<int> *>(symbol, non_terminator_symbols));

  return non_terminator_symbols;
}

auto GrammarResourcePool::FOLLOW(int symbol) -> const std::set<int> * {
  if (follows_.empty()) FOLLOW();

  const auto it = follows_.find(symbol);
  if (it != follows_.end()) return it->second;

  throw std::runtime_error("Symbol_Index" + std::to_string(symbol) +
                           "NOT Found");
}

void GrammarResourcePool::FOLLOW() {
  for (const auto &symbol : symbol_table_.GetAllSymbols()) {
    if (!symbol->terminator) {
      if (symbol->start) {
        std::set<int> *non_terminator_symbols = GetFollowSet(symbol->index);
        non_terminator_symbols->insert(kStopSymbolId);
      }
    }
  }

  bool if_added = true;

  while (if_added) {
    if_added = false;

    std::set<int> *non_terminator_symbols = nullptr;

    for (const auto &production : productions_) {
      const auto &right_symbols = production->right;

      std::set<int> equal_left_non_terminators;

      for (int i = 0; i < right_symbols.size() - 1; i++) {
        if (!symbol_table_.GetSymbol(right_symbols[i])->terminator) {
          const auto *const p_non_term_set = FIRST(right_symbols, i + 1);

          non_terminator_symbols = GetFollowSet(right_symbols[i]);

          const size_t set_size = non_terminator_symbols->size();

          non_terminator_symbols->insert(p_non_term_set->begin(),
                                         p_non_term_set->end());

          if (non_terminator_symbols->find(0) !=
              non_terminator_symbols->end()) {
            non_terminator_symbols->erase(0);
            equal_left_non_terminators.insert(right_symbols[i]);
          }

          if (set_size < non_terminator_symbols->size()) if_added = true;
        }
      }

      if (!right_symbols.empty()) {
        if (!symbol_table_.GetSymbol(right_symbols[right_symbols.size() - 1])
                 ->terminator) {
          equal_left_non_terminators.insert(
              right_symbols[right_symbols.size() - 1]);
        }
      }

      for (const auto symbol : equal_left_non_terminators) {
        auto *const left_non_terminator_symbols =
            GetFollowSet(production->left);

        non_terminator_symbols = GetFollowSet(symbol);

        const size_t set_size = non_terminator_symbols->size();

        non_terminator_symbols->insert(left_non_terminator_symbols->begin(),
                                       left_non_terminator_symbols->end());

        if (non_terminator_symbols->find(0) != non_terminator_symbols->end()) {
          non_terminator_symbols->erase(0);
        }

        if (set_size < non_terminator_symbols->size()) {
          if_added = true;
        }
      }
    }
  }
}

auto GrammarResourcePool::GetFollowSet(int symbol) -> std::set<int> * {
  std::set<int> *non_terminator_symbols = nullptr;

  auto it = follows_.find(symbol);
  if (it != follows_.end()) {
    non_terminator_symbols = it->second;
  } else {
    non_terminator_symbols = new std::set<int>();
    this->follows_.insert(
        std::pair<int, std::set<int> *>(symbol, non_terminator_symbols));
  }

  return non_terminator_symbols;
}

void GrammarResourcePool::PrintSymbols(const std::set<int> &symbols_index) {
  std::cout << "{ ";
  for (const auto &symbol_index : symbols_index) {
    const auto *p_sym = symbol_table_.GetSymbol(symbol_index);

    if (p_sym->terminator) {
      if (p_sym->name == "ε") {
        std::cout << " [Epsilon] ";
      } else
        std::cout << " \"" << p_sym->name << "\" ";
    } else {
      std::cout << " " << p_sym->name << " ";
    }
  }
  std::cout << "}" << '\n';
}

void GrammarResourcePool::ParseProductionStringLine(const std::string &line) {
  auto middle_index = line.find("->", 0);

  if (middle_index == std::string::npos) {
    throw std::runtime_error("-> NOT FOUND");
  }

  std::string front = trim(line.substr(0, middle_index));
  int left = symbol_table_.AddSymbol(front, false);

  std::string back =
      trim(line.substr(middle_index + 2, line.size() - middle_index - 2));

  std::stringstream terminator;
  std::stringstream non_terminator;
  std::vector<int> symbols;
  bool is_terminator = false;

  for (const auto &c : back) {
    if (c == '\"') {
      if (is_terminator) {
        symbols.push_back(
            symbol_table_.AddSymbol(trim(terminator.str()), true));
        terminator.str("");
        terminator.clear();
      }
      is_terminator = !is_terminator;
      continue;
    }
    if (c == ' ' || c == '\r') {
      std::string temp_symbol = trim(non_terminator.str());
      if (!temp_symbol.empty()) {
        symbols.push_back(
            symbol_table_.AddSymbol(trim(non_terminator.str()), false));
        non_terminator.str("");
        non_terminator.clear();
      }
      continue;
    }
    if (is_terminator) {
      terminator << c;
    } else {
      non_terminator << c;
    }
  }

  std::string temp_symbol = trim(non_terminator.str());
  if (!temp_symbol.empty()) {
    symbols.push_back(
        symbol_table_.AddSymbol(trim(non_terminator.str()), false));
  }

  productions_.push_back(
      std::make_shared<Production>(pdt_index_++, left, symbols));
}

auto GrammarResourcePool::AddProduction(int left,
                                        std::initializer_list<int> right)
    -> ProductionPtr {
  std::vector<int> right_vector;
  for (int symbol : right) {
    right_vector.push_back(symbol);
  }

  productions_.push_back(
      std::make_shared<Production>(pdt_index_++, left, right_vector));
  return productions_.back();
}
auto GrammarResourcePool::GetProductions() const
    -> const std::vector<ProductionPtr> & {
  return productions_;
}

auto GrammarResourcePool::GetSymbol(int symbol_index) const -> const Symbol * {
  return symbol_table_.GetSymbol(symbol_index);
}

auto GrammarResourcePool::AddSymbol(const std::string &name, bool terminator)
    -> int {
  return symbol_table_.AddSymbol(name, terminator);
}

auto GrammarResourcePool::AddSymbolToken(int index, const std::string &name)
    -> int {
  return symbol_table_.AddSymbol(index, name, true, false);
}

void GrammarResourcePool::ModifySymbol(int index, const std::string &name,
                                       bool terminator, bool start) {
  symbol_table_.ModifySymbol(index, name, terminator, start);
}

auto GrammarResourcePool::GetSymbolIndex(const std::string &name) const -> int {
  return symbol_table_.GetSymbolIndex(name);
}

auto GrammarResourcePool::trim(std::string &&str) -> std::string & {
  if (str.empty()) {
    return str;
  }

  str.erase(0, str.find_first_not_of(' '));
  str.erase(str.find_last_not_of(' ') + 1);
  return str;
}

auto GrammarResourcePool::GetAllSymbols() const
    -> const std::vector<const Symbol *> & {
  return symbol_table_.GetAllSymbols();
}
auto GrammarResourcePool::GetStartSymbol() const -> const Symbol * {
  return symbol_table_.GetStartSymbol();
}
