//
// Created by Administrator on 2021/4/30.
//

#include "AnalyseTableGenerator.h"

#include <iostream>

void AnalyseTableGenerator::add_action(
    int index, int terminator_symbol, Action action,
    const std::shared_ptr<Production> &target_pdt) {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, terminator_symbol);

  auto it = ACTION.find(seed);
  if (it == ACTION.end()) {
    auto *step = new Step(action, target_pdt);
    ACTION.insert(std::pair<size_t, Step *>(seed, step));
  } else {
    if (it->second->action != action ||
        it->second->target.production != target_pdt) {
      std::cout << "\n"
                << "Conflict at state: " << index
                << " with symbol: " << pool->GetSymbol(terminator_symbol)->name
                << "\n"
                << "Existing action: " << it->second->action
                << ", new action: " << action
                << ", existing target: " << it->second->target.index
                << ", new target: " << target_pdt->index << '\n';

      throw std::runtime_error(
          "Conflict Occurred on Action Adding (A), Syntax NOT LR(1)");
    }
  }
}

void AnalyseTableGenerator::add_action(int index, int terminator_symbol,
                                       Action action, int target_index) {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, terminator_symbol);

  auto it = ACTION.find(seed);
  if (it == ACTION.end()) {
    auto *step = new Step(action, target_index);
    ACTION.insert(std::pair<size_t, Step *>(seed, step));
  } else {
    if (it->second->action != action ||
        it->second->target.index != target_index) {
      std::cout << "\n"
                << "Conflict at state: " << index
                << " with symbol: " << pool->GetSymbol(terminator_symbol)->name
                << "\n"
                << "Existing action: " << it->second->action
                << ", new action: " << action
                << ", existing target: " << it->second->target.index
                << ", new target: " << target_index << '\n';

      throw std::runtime_error(
          "Conflict Occurred on Action Adding (B), Syntax NOT LR(1)");
    }
  }
}

void AnalyseTableGenerator::add_goto(int index, int non_terminator_symbol,
                                     int target_index) {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, non_terminator_symbol);

  auto it = GOTO.find(seed);
  if (it == GOTO.end()) {
    auto *step = new Step(STEP_GOTO, target_index);
    GOTO.insert(std::pair<size_t, Step *>(seed, step));
  } else {
    if (it->second->target.index != target_index) {
      throw std::runtime_error(
          "Conflict Occurred on GOTO Adding, Syntax NOT LR(1)");
    }
  }
}

void AnalyseTableGenerator::generate() {
  const auto &ics = icm->getItemCollections();
  for (const auto *ic : ics) {
    for (const auto *item : ic->GetItems()) {
      if (item->get_production() == icm->getStartProduction() &&
          item->get_dot_next_symbol() == 0 &&
          item->get_terminator() == kStopSymbolId) {
        this->add_action(ic->GetIndex(), kStopSymbolId, ACC, 0);
      }
      int next_symbol = item->get_dot_next_symbol();
      if (next_symbol != 0) {
        const auto *p_ic = icm->getGOTO(ic->GetIndex(), next_symbol);
        if (pool->GetSymbol(next_symbol)->terminator) {
          if (p_ic != nullptr) {
            this->add_action(ic->GetIndex(), next_symbol, MOVE,
                             p_ic->GetIndex());
          }
        } else {
          if (p_ic != nullptr) {
            this->add_goto(ic->GetIndex(), next_symbol, p_ic->GetIndex());
          }
        }
      } else {
        if (pool->GetSymbol(next_symbol)->terminator) {
          if (item->get_production()->left != pool->GetStartSymbol()->index) {
            this->add_action(ic->GetIndex(), item->get_terminator(), REDUCE,
                             item->get_production());
          }
        }
      }
    }
  }
}

auto AnalyseTableGenerator::findActionStep(int index, int symbol) const
    -> const AnalyseTableGenerator::Step * {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, symbol);

  auto it = ACTION.find(seed);
  if (it != ACTION.end()) {
    return it->second;
  }
  return nullptr;
}

const AnalyseTableGenerator::Step *AnalyseTableGenerator::findGotoStep(
    int index, int non_terminator_symbol) const {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, non_terminator_symbol);

  auto it = GOTO.find(seed);
  if (it != GOTO.end()) {
    return it->second;
  }
  return nullptr;
}

void AnalyseTableGenerator::Print(const std::string &path) const {
  std::ofstream output(path);

  size_t indent = 4;

  output << "[ACTION]---------------------->" << '\n';
  std::vector<int> symbols;

  for (const auto *symbol : pool->GetAllSymbols()) {
    if (symbol->index == 0) continue;
    if (symbol->terminator) {
      indent = std::max(indent, symbol->name.size() + 2);
      symbols.push_back(symbol->index);
    }
  }

  auto space = std::setw(static_cast<int>(indent));

  output << std::left << space << " ";
  for (const auto symbol_index : symbols) {
    output << std::left << space << pool->GetSymbol(symbol_index)->name;
  }

  output << '\n';

  for (int i = 0; i < icm->getItemCollections().size(); i++) {
    output << std::left << space << i;
    for (int symbol : symbols) {
      const auto *p_step = this->findActionStep(i, symbol);
      if (p_step == nullptr) {
        output << std::left << space << " ";
      } else {
        if (p_step->action == MOVE) {
          output << std::left << space
                 << std::string("s") + std::to_string(p_step->target.index);
        } else if (p_step->action == ACC) {
          output << std::left << space << "acc";
        } else if (p_step->action == REDUCE) {
          output << std::left << space
                 << "r" + std::to_string(p_step->target.production->index);
        }
      }
    }
    output << '\n';
  }

  output << '\n';

  indent = 4;

  output << "[GOTO]---------------------->" << '\n';
  symbols.clear();

  for (const auto *symbol : pool->GetAllSymbols()) {
    if (symbol->index == 0) continue;
    if (!symbol->terminator && !symbol->start) {
      indent = std::max(indent, symbol->name.size() + 2);
      symbols.push_back(symbol->index);
    }
  }

  space = std::setw(static_cast<int>(indent));

  output << std::left << space << " ";
  for (const auto symbol_index : symbols) {
    output << std::left << space << pool->GetSymbol(symbol_index)->name;
  }

  output << std::endl;

  for (int k = 0; k < icm->getItemCollections().size(); k++) {
    output << std::left << space << k;
    for (int symbol : symbols) {
      auto p_step = this->findGotoStep(k, symbol);
      if (p_step == nullptr) {
        output << std::left << space << " ";
      } else {
        output << std::left << space << std::to_string(p_step->target.index);
      }
    }
    output << std::endl;
  }

  output << std::endl << std::endl;

  output.close();
}
