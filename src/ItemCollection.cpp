//
// Created by Administrator on 2021/4/30.
//
#include "ItemCollection.h"

size_t ItemCollection::GetHash() const {
  size_t seed = 0;

  std::vector<Item *> cache_sorted(cache.begin(), cache.end());
  std::sort(cache_sorted.begin(), cache_sorted.end(), compare_item_ptr);

  for (auto *const item : cache_sorted) {
    if (item->generated) {
      continue;
    }

    hash_combine(seed, item->get_production());
    hash_combine(seed, item->get_dot_index());
    hash_combine(seed, item->get_terminator());
  }
  return seed;
}

void ItemCollection::Print(std::ofstream &output) const {
  output << "I" << index << ": ";

  for (const auto &item : cache) {
    auto p_pdt = item->get_production();
    int dot_index = item->get_dot_index();
    output << pool->GetSymbol(p_pdt->left)->name << " -> ";
    int i = 0;
    for (const auto &symbol_index : p_pdt->right) {
      if (i > 0) output << " ";
      if (i++ == dot_index) output << "*";

      const auto *symbol = pool->GetSymbol(symbol_index);

      if (symbol->index == 0) {
        output << "[Epsilon]";
        continue;
      }

      if (!symbol->terminator) {
        output << pool->GetSymbol(symbol_index)->name;
      } else {
        output << '"' << pool->GetSymbol(symbol_index)->name << '"';
      }
    }

    if (i++ == dot_index) output << "*";

    output << L", \"" << pool->GetSymbol(item->get_terminator())->name << "\""
           << std::endl;
  }
  output << std::endl;
}

void ItemCollection::CLOSURE() {
  bool ifAdd = true;

  while (ifAdd) {
    ifAdd = false;

    for (const auto &item : items) {
      int next_symbol = item.second->get_dot_next_symbol();

      if (next_symbol == 0 || pool->GetSymbol(next_symbol)->terminator) {
        continue;
      }

      for (const auto &production : pool->GetProductions()) {
        if (production->left == next_symbol) {
          std::vector<int> first_args;
          auto *p_ic = item.second;
          const auto last_right_symbol_count =
              p_ic->get_right_size() - p_ic->get_dot_index();
          for (int i = 1; i <= last_right_symbol_count; i++) {
            first_args.push_back(p_ic->get_dot_next_i_symbol(i));
          }
          first_args.push_back(p_ic->get_terminator());

          const auto *const first_set = pool->FIRST(first_args, 0);
          for (auto terminator : *first_set) {
            if (terminator == 0) continue;
            if (this->AddItem(production, 0, terminator, true)) {
              ifAdd = true;
            }
          }
        }
      }
    }
  }
}

auto ItemCollection::AddItem(const std::shared_ptr<Production> &p_pdt,
                             int dot_index, int terminator, bool generated)
    -> bool {
  auto hasher = std::hash<int>();
  size_t seed = hasher(reinterpret_cast<long>(p_pdt.get()));
  hash_combine(seed, dot_index);
  hash_combine(seed, terminator);

  auto it = items.find(seed);
  if (it != items.end()) {
    return false;
  }

  auto *p_item = new Item(p_pdt, terminator, generated);
  p_item->set_dot_index(dot_index);
  items.insert(std::pair<size_t, Item *>(seed, p_item));
  cache.push_back(p_item);

  return true;
}

bool ItemCollection::compare_item_ptr(const Item *lhs, const Item *rhs) {
  if (lhs->get_production() != rhs->get_production()) {
    return lhs->get_production() < rhs->get_production();
  }
  if (lhs->get_dot_index() != rhs->get_dot_index()) {
    return lhs->get_dot_index() < rhs->get_dot_index();
  }
  return lhs->get_terminator() < rhs->get_terminator();
}

auto ItemCollection::GetItems() const -> const std::vector<Item *> & {
  return cache;
}

auto ItemCollection::GetIndex() const -> int { return index; }
