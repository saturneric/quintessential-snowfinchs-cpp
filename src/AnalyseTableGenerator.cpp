//
// Created by Administrator on 2021/4/30.
//

#include "AnalyseTableGenerator.h"

void AnalyseTableGenerator::add_action(int index, int terminator_symbol,
                                       Action action,
                                       const Production *target_pdt) {
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
                << " with symbol: " << pool->getSymbol(terminator_symbol)->name
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
    for (const auto *item : ic->getItems()) {
      if (item->get_production() == icm->getStartProduction() &&
          item->get_dot_next_symbol() == 0 && item->get_terminator() == -1) {
        this->add_action(ic->getIndex(), -1, ACC, 0);
      }
      int next_symbol = item->get_dot_next_symbol();
      if (next_symbol != 0) {
        const auto *p_ic = icm->getGOTO(ic->getIndex(), next_symbol);
        if (pool->getSymbol(next_symbol)->terminator) {
          if (p_ic != nullptr) {
            this->add_action(ic->getIndex(), next_symbol, MOVE,
                             p_ic->getIndex());
          }
        } else {
          if (p_ic != nullptr) {
            this->add_goto(ic->getIndex(), next_symbol, p_ic->getIndex());
          }
        }
      } else {
        if (pool->getSymbol(next_symbol)->terminator) {
          if (item->get_production()->left != pool->getStartSymbol()->index) {
            this->add_action(ic->getIndex(), item->get_terminator(), REDUCE,
                             item->get_production());
          }
        }
      }
    }
  }
}

const AnalyseTableGenerator::Step *AnalyseTableGenerator::findActionStep(
    int index, int terminator_symbol) const {
  size_t seed = 0;
  hash_combine(seed, index);
  hash_combine(seed, terminator_symbol);

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

void AnalyseTableGenerator::print() const {
  std::ofstream output("tables.txt");

  size_t space = 4;

  output << "ACTION" << '\n';
  std::vector<int> symbols;

  for (const auto *symbol : pool->getAllSymbols()) {
    if (symbol->index == 0) continue;
    if (symbol->terminator) {
      space = std::max(space, symbol->name.size() + 2);
      symbols.push_back(symbol->index);
    }
  }

  output << std::left << std::setw(space) << " ";
  for (const auto symbol_index : symbols) {
    output << std::left << std::setw(space)
           << pool->getSymbol(symbol_index)->name;
  }

  output << std::endl;

  for (int i = 0; i < icm->getItemCollections().size(); i++) {
    output << std::left << std::setw(space) << i;
    for (int symbol : symbols) {
      auto p_step = this->findActionStep(i, symbol);
      if (p_step == nullptr) {
        output << std::left << std::setw(space) << " ";
      } else {
        if (p_step->action == MOVE)
          output << std::left << std::setw(space)
                 << std::string("s") + std::to_string(p_step->target.index);
        else if (p_step->action == ACC)
          output << std::left << std::setw(space) << "acc";
        else if (p_step->action == REDUCE)
          output << std::left << std::setw(space)
                 << "r" + std::to_string(p_step->target.production->index);
      }
    }
    output << std::endl;
  }

  output << std::endl;

  space = 4;

  output << "GOTO" << std::endl;
  symbols.clear();

  for (const auto *symbol : pool->getAllSymbols()) {
    if (symbol->index == 0) continue;
    if (!symbol->terminator && !symbol->start) {
      space = std::max(space, symbol->name.size() + 2);
      symbols.push_back(symbol->index);
    }
  }

  output << std::left << std::setw(space) << " ";
  for (const auto symbol_index : symbols) {
    output << std::left << std::setw(space)
           << pool->getSymbol(symbol_index)->name;
  }

  output << std::endl;

  for (int k = 0; k < icm->getItemCollections().size(); k++) {
    output << std::left << std::setw(space) << k;
    for (int symbol : symbols) {
      auto p_step = this->findGotoStep(k, symbol);
      if (p_step == nullptr) {
        output << std::left << std::setw(space) << " ";
      } else {
        output << std::left << std::setw(space)
               << std::to_string(p_step->target.index);
      }
    }
    output << std::endl;
  }

  output << std::endl << std::endl;

  output.close();
}
