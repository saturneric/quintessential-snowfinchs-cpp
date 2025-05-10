#include "GrammarResourcePool.h"

#include <iostream>

auto GrammarResourcePool::FIRST(const std::vector<int> &symbols,
                                int start_index) -> SymbolSetPtr {
  auto non_terminator_symbols = std::make_shared<SymbolSet>();

  for (int i = start_index; i < symbols.size(); i++) {
    auto p_non_term_set = FIRST(symbols[i]);
    non_terminator_symbols->insert(p_non_term_set->begin(),
                                   p_non_term_set->end());

    const auto sec_it = p_non_term_set->find(kEmptySymbolId);
    if (sec_it != p_non_term_set->end()) continue;

    break;
  }

  return non_terminator_symbols;
}

auto GrammarResourcePool::FIRST(int symbol) -> SymbolSetPtr {
  const auto it = firsts_.find(symbol);
  if (it != firsts_.end()) return it->second;

  auto non_terminator_symbols = std::make_shared<SymbolSet>();

  if (symbol_table_.Symbol(symbol)->IsTerminator()) {
    non_terminator_symbols->insert(symbol);
  } else {
    bool production_found = false;

    for (const auto &production : productions_) {
      auto p_pdt = production;

      if (p_pdt->left != symbol) continue;

      production_found = true;

      for (const auto &right_symbol : p_pdt->right) {
        const auto p_non_term_set = FIRST(right_symbol);

        non_terminator_symbols->insert(p_non_term_set->begin(),
                                       p_non_term_set->end());

        const auto sec_it = p_non_term_set->find(0);

        if (sec_it != p_non_term_set->end()) continue;

        break;
      }
    }

    if (!production_found) non_terminator_symbols->insert(0);
  }

  this->firsts_.insert({symbol, non_terminator_symbols});
  return non_terminator_symbols;
}

auto GrammarResourcePool::FOLLOW(int symbol) -> SymbolSetPtr {
  if (follows_.empty()) FOLLOW();

  const auto it = follows_.find(symbol);
  if (it != follows_.end()) return it->second;

  throw std::runtime_error("Symbol_Index" + std::to_string(symbol) +
                           "NOT Found");
}

void GrammarResourcePool::FOLLOW() {
  auto symbol = symbol_table_.GetSyntaxStartSymbol();
  auto non_terminator_symbols = GetFollowSet(symbol->Index());
  non_terminator_symbols->insert(kStopSymbolId);

  bool if_added = true;

  while (if_added) {
    if_added = false;

    SymbolSetPtr non_terminator_symbols = nullptr;

    for (const auto &production : productions_) {
      const auto &right_symbols = production->right;

      std::set<int> equal_left_non_terminators;

      for (int i = 0; i < right_symbols.size() - 1; i++) {
        if (!symbol_table_.Symbol(right_symbols[i])->IsTerminator()) {
          auto p_non_term_set = FIRST(right_symbols, i + 1);

          auto non_terminator_symbols = GetFollowSet(right_symbols[i]);
          const size_t set_size = non_terminator_symbols->size();

          non_terminator_symbols->insert(p_non_term_set->begin(),
                                         p_non_term_set->end());

          if (non_terminator_symbols->find(kEmptySymbolId) !=
              non_terminator_symbols->end()) {
            non_terminator_symbols->erase(kEmptySymbolId);
            equal_left_non_terminators.insert(right_symbols[i]);
          }

          if (set_size < non_terminator_symbols->size()) if_added = true;
        }
      }

      if (!right_symbols.empty()) {
        if (!symbol_table_.Symbol(right_symbols[right_symbols.size() - 1])
                 ->IsTerminator()) {
          equal_left_non_terminators.insert(
              right_symbols[right_symbols.size() - 1]);
        }
      }

      for (const auto symbol : equal_left_non_terminators) {
        auto left_non_terminator_symbols = GetFollowSet(production->left);

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

auto GrammarResourcePool::GetFollowSet(int symbol) -> SymbolSetPtr {
  SymbolSetPtr non_terminator_symbols = nullptr;

  auto it = follows_.find(symbol);
  if (it != follows_.end()) {
    non_terminator_symbols = it->second;
  } else {
    non_terminator_symbols = std::make_shared<SymbolSet>();
    this->follows_.insert({symbol, non_terminator_symbols});
  }

  return non_terminator_symbols;
}

void GrammarResourcePool::PrintSymbols(const std::set<int> &symbols_index) {
  std::cout << "{ ";
  for (const auto &symbol_index : symbols_index) {
    const auto p_sym = symbol_table_.Symbol(symbol_index);

    if (p_sym->IsTerminator()) {
      if (p_sym->Name() == kEmptySymbol) {
        std::cout << " [Epsilon] ";
      } else {
        std::cout << " \"" << p_sym->Name() << "\" ";
      }
    } else {
      std::cout << " " << p_sym->Name() << " ";
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
  auto symbol = symbol_table_.AddSymbol(SymbolType::kSYNTAX, front);
  if (front.front() == kStartSymbolPrefix) symbol->SetStartSymbol(true);

  auto left = symbol->Index();

  std::string back =
      trim(line.substr(middle_index + 2, line.size() - middle_index - 2));

  std::vector<int> symbols;

  std::regex pattern("\"(.*?)\"|(\\S+)");
  std::smatch match;

  std::string::const_iterator search_start(back.cbegin());

  while (std::regex_search(search_start, back.cend(), match, pattern)) {
    if (match[1].matched) {
      const auto symbol_value = trim(match[1].str());

      auto symbol =
          symbol_table_.SearchSymbol(SymbolType::kSYNTAX, symbol_value);
      if (!symbol) {
        throw std::runtime_error("Terminator Symbol " + symbol_value +
                                 " Not Found");
      }

      std::cout << "MATCH T SYMBOL: " << symbol_value
                << " INDEX: " << symbol->Index() << "\n";

      symbols.push_back(symbol->Index());
    } else if (match[2].matched) {
      const auto symbol_value = trim(match[2].str());

      auto symbol =
          symbol_table_.SearchSymbol(SymbolType::kSYNTAX, symbol_value);

      if (!symbol) {
        symbol = symbol_table_.AddSymbol(SymbolType::kSYNTAX, symbol_value);
      }

      std::cout << "MATCH S SYMBOL: " << symbol_value
                << " INDEX: " << symbol->Index() << "\n";

      symbols.push_back(symbol->Index());
    }

    search_start = match.suffix().first;
  }

  AddProduction(left, symbols);
}

auto GrammarResourcePool::AddProduction(int left, const std::vector<int> &right)
    -> ProductionPtr {
  auto pd = std::make_shared<Production>(pdt_index_++, left, right);
  productions_.push_back(pd);
  return pd;
}

auto GrammarResourcePool::GetProductions() const
    -> const std::vector<ProductionPtr> & {
  return productions_;
}

auto GrammarResourcePool::GetSymbol(int symbol_index) const -> SymbolPtr {
  return symbol_table_.Symbol(symbol_index);
}

auto GrammarResourcePool::AddSymbol(const std::string &name, bool terminator)
    -> SymbolPtr {
  auto symbol = symbol_table_.AddSymbol(SymbolType::kSYNTAX, name);
  symbol->SetTerminator(terminator);
  return symbol;
}

void GrammarResourcePool::ModifySymbol(int index, const std::string &name,
                                       bool terminator, bool start) {
  symbol_table_.ModifySymbol(index, name, terminator, start);
}

auto GrammarResourcePool::GetSymbolIndex(const std::string &name) const -> int {
  return symbol_table_.SymbolIndex(SymbolType::kSYNTAX, name);
}

auto GrammarResourcePool::trim(const std::string &str) -> std::string {
  if (str.empty()) return str;

  auto temp = str;
  temp.erase(0, temp.find_first_not_of(' '));
  temp.erase(temp.find_last_not_of(' ') + 1);
  return temp;
}

auto GrammarResourcePool::GetAllSymbols() const -> std::vector<SymbolPtr> {
  return symbol_table_.GetAllSyntaxSymbols();
}

auto GrammarResourcePool::GetStartSymbol() const -> SymbolPtr {
  return symbol_table_.GetSyntaxStartSymbol();
}

auto GrammarResourcePool::GetSymbolByToken(const std::string &token_index) const
    -> SymbolPtr {
  return symbol_table_.SearchSymbol(SymbolType::kSYNTAX, token_index);
}
