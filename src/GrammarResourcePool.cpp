//
// Created by Administrator on 2021/4/30.
//

#include "GrammarResourcePool.h"

const std::set<int> *GrammarResourcePool::FIRST(const std::vector<int> &symbols,
                                                int start_index) {
  // ���ɼ���
  auto *non_terminator_symbols = new std::set<int>();

  for (int i = start_index; i < symbols.size(); i++) {
    const auto *const p_non_term_set = FIRST(symbols[i]);

    non_terminator_symbols->insert(p_non_term_set->begin(),
                                   p_non_term_set->end());

    const auto sec_it = p_non_term_set->find(0);
    if (sec_it != p_non_term_set->end()) {
      continue;
    } else {
      break;
    }
  }

  return non_terminator_symbols;
}

auto GrammarResourcePool::FIRST(int symbol) -> const std::set<int> * {
  // ���һ���
  const auto it = firsts.find(symbol);
  if (it != firsts.end()) {
    return it->second;
  }

  // ���ɼ���
  auto *non_terminator_symbols = new std::set<int>();

  // ������ս��
  if (symbolTable.getSymbol(symbol)->terminator) {
    non_terminator_symbols->insert(symbol);
  } else {
    bool production_found = false;

    // ����ÿһ����ʽ
    for (const auto &production : productions) {
      const Production *p_pdt = production;

      if (p_pdt->left != symbol) continue;

      production_found = true;

      for (const auto &right_symbol : p_pdt->right) {
        const auto *const p_non_term_set = FIRST(right_symbol);

        non_terminator_symbols->insert(p_non_term_set->begin(),
                                       p_non_term_set->end());

        const auto sec_it = p_non_term_set->find(0);

        if (sec_it != p_non_term_set->end()) {
          continue;
        } else {
          break;
        }
      }
    }

    if (!production_found) non_terminator_symbols->insert(0);
  }

  this->firsts.insert(
      std::pair<int, const std::set<int> *>(symbol, non_terminator_symbols));

  return non_terminator_symbols;
}

const std::set<int> *GrammarResourcePool::FOLLOW(int symbol) {
  if (follows.empty()) {
    FOLLOW();
  }

  const auto it = follows.find(symbol);
  if (it != follows.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Symbol_Index" + std::to_string(symbol) +
                             "NOT Found");
  }
}

void GrammarResourcePool::FOLLOW() {
  for (const auto &symbol : symbolTable.getAllSymbols()) {
    if (!symbol->terminator) {
      if (symbol->start) {
        std::set<int> *non_terminator_symbols = get_follow_set(symbol->index);
        non_terminator_symbols->insert(-1);
      }
    }
  }

  // ָ��û���µķ��ű����ӵ�����FOLLOW����
  bool ifAdded = true;

  while (ifAdded) {
    ifAdded = false;

    std::set<int> *non_terminator_symbols = nullptr;

    for (const auto &production : productions) {
      const auto &right_symbols = production->right;

      std::set<int> equal_left_non_terminators;

      for (int i = 0; i < right_symbols.size() - 1; i++) {
        // ���ս��
        if (!symbolTable.getSymbol(right_symbols[i])->terminator) {
          const auto p_non_term_set = FIRST(right_symbols, i + 1);

          // ���FOLLOW��
          non_terminator_symbols = get_follow_set(right_symbols[i]);

          const size_t set_size = non_terminator_symbols->size();

          non_terminator_symbols->insert(p_non_term_set->begin(),
                                         p_non_term_set->end());

          // �ڼ����з��ֿ��ַ�
          if (non_terminator_symbols->find(0) !=
              non_terminator_symbols->end()) {
            non_terminator_symbols->erase(0);
            equal_left_non_terminators.insert(right_symbols[i]);
          }

          // ����Ƿ����µ��ս���ű�����
          if (set_size < non_terminator_symbols->size()) {
            ifAdded = true;
          }
        }
      }

      if (!right_symbols.empty()) {
        if (!symbolTable.getSymbol(right_symbols[right_symbols.size() - 1])
                 ->terminator) {
          equal_left_non_terminators.insert(
              right_symbols[right_symbols.size() - 1]);
        }
      }

      for (const auto symbol : equal_left_non_terminators) {
        // �����߷��ս����FOLLOW��
        const auto left_non_terminator_symbols =
            get_follow_set(production->left);
        // ���FOLLOW��
        non_terminator_symbols = get_follow_set(symbol);

        const size_t set_size = non_terminator_symbols->size();

        non_terminator_symbols->insert(left_non_terminator_symbols->begin(),
                                       left_non_terminator_symbols->end());

        if (non_terminator_symbols->find(0) != non_terminator_symbols->end()) {
          non_terminator_symbols->erase(0);
        }

        // ����Ƿ����µ��ս���ű�����
        if (set_size < non_terminator_symbols->size()) {
          ifAdded = true;
        }
      }
    }
  }
}

std::set<int> *GrammarResourcePool::get_follow_set(int symbol) {
  std::set<int> *non_terminator_symbols = nullptr;

  // ���һ���
  auto it = follows.find(symbol);
  if (it != follows.end()) {
    non_terminator_symbols = it->second;
  } else {
    non_terminator_symbols = new std::set<int>();
    this->follows.insert(
        std::pair<int, std::set<int> *>(symbol, non_terminator_symbols));
  }

  return non_terminator_symbols;
}

void GrammarResourcePool::print_symbols(const std::set<int> &symbols_index) {
  std::cout << "{ ";
  for (const auto &symbol_index : symbols_index) {
    const auto *p_sym = symbolTable.getSymbol(symbol_index);

    if (p_sym->terminator) {
      if (p_sym->name == "��") {
        std::cout << " [Epsilon] ";
      } else
        std::cout << " \"" << p_sym->name << "\" ";
    } else {
      std::cout << " " << p_sym->name << " ";
    }
  }
  std::cout << "}" << std::endl;
}

void GrammarResourcePool::parse_production_string_line(
    const std::string &temp_line) {
  auto middle_index = temp_line.find("->", 0);

  if (middle_index == std::string::npos) {
    throw std::runtime_error("-> NOT FOUND");
  }

  std::string front = trim(temp_line.substr(0, middle_index));
  int left = symbolTable.addSymbol(front, false);

  std::string back = trim(
      temp_line.substr(middle_index + 2, temp_line.size() - middle_index - 2));

  std::stringstream terminator, non_terminator;
  std::vector<int> symbols;
  bool is_terminator = false;
  for (const auto &c : back) {
    if (c == '\"') {
      if (is_terminator) {
        symbols.push_back(symbolTable.addSymbol(trim(terminator.str()), true));
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
            symbolTable.addSymbol(trim(non_terminator.str()), false));
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
    symbols.push_back(symbolTable.addSymbol(trim(non_terminator.str()), false));
  }

  auto p_pdt = new Production(pdt_index++, left, symbols);

  productions.push_back(p_pdt);
}

const Production *GrammarResourcePool::addProduction(
    int left, std::initializer_list<int> right) {
  std::vector<int> right_vector;
  for (int symbol : right) {
    right_vector.push_back(symbol);
  }
  auto p_pdt = new Production(pdt_index++, left, right_vector);
  productions.push_back(p_pdt);
  return p_pdt;
}
