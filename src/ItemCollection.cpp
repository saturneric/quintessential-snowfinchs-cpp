#include "ItemCollection.h"

auto ItemCollection::GetHash() const -> size_t {
  size_t seed = 0;

  std::vector<ItemPtr> cache_sorted(cache_.begin(), cache_.end());
  std::sort(cache_sorted.begin(), cache_sorted.end(), compare_item_ptr);

  for (const auto &item : cache_sorted) {
    if (item->generated) {
      continue;
    }

    hash_combine(seed, item->GetProduction());
    hash_combine(seed, item->GetDotIndex());
    hash_combine(seed, item->GetTerminator());
  }
  return seed;
}

void ItemCollection::Print(std::stringstream &output) const {
  output << "I" << index_ << ": ";

  for (const auto &item : cache_) {
    auto p_pdt = item->GetProduction();
    int dot_index = item->GetDotIndex();
    output << pool_->GetSymbol(p_pdt->left)->Name() << " -> ";
    int i = 0;
    for (const auto &symbol_index : p_pdt->right) {
      if (i > 0) output << " ";
      if (i++ == dot_index) output << "*";

      const auto symbol = pool_->GetSymbol(symbol_index);

      if (symbol->Index() == kEmptySymbolId) {
        output << "[Epsilon]";
        continue;
      }

      if (!symbol->IsTerminator()) {
        output << pool_->GetSymbol(symbol_index)->Name();
      } else {
        output << '"' << pool_->GetSymbol(symbol_index)->Name() << '"';
      }
    }

    if (i++ == dot_index) output << "*";

    output << ", \"" << pool_->GetSymbol(item->GetTerminator())->Name() << "\""
           << '\n';
  }
  output << '\n';
}

void ItemCollection::CLOSURE() {
  bool add = true;

  while (add) {
    add = false;

    for (const auto &item : items_) {
      int next_symbol = item.second->GetDotNextSymbol();

      if (next_symbol == kEmptySymbolId ||
          pool_->GetSymbol(next_symbol)->IsTerminator()) {
        continue;
      }

      for (const auto &production : pool_->GetProductions()) {
        if (production->left == next_symbol) {
          std::vector<int> first_args;
          auto p_ic = item.second;
          const auto last_right_symbol_count =
              p_ic->GetRightSize() - p_ic->GetDotIndex();
          for (int i = 1; i <= last_right_symbol_count; i++) {
            first_args.push_back(p_ic->GetDotNextISymbol(i));
          }
          first_args.push_back(p_ic->GetTerminator());

          auto first_set = pool_->FIRST(first_args, 0);
          for (auto terminator : *first_set) {
            if (terminator == 0) continue;
            if (this->AddItem(production, 0, terminator, true)) {
              add = true;
            }
          }
        }
      }
    }
  }
}

auto ItemCollection::AddItem(const ProductionPtr &p_pdt, int dot_index,
                             int terminator, bool generated) -> bool {
  auto hasher = std::hash<int>();
  size_t seed = hasher(reinterpret_cast<long>(p_pdt.get()));
  hash_combine(seed, dot_index);
  hash_combine(seed, terminator);

  auto it = items_.find(seed);
  if (it != items_.end()) {
    return false;
  }

  auto p_item = std::make_shared<Item>(p_pdt, terminator, generated);
  p_item->SetDotIndex(dot_index);
  items_.insert({seed, p_item});
  cache_.push_back(p_item);

  return true;
}

auto ItemCollection::compare_item_ptr(const ItemPtr &lhs, const ItemPtr &rhs)
    -> bool {
  if (lhs->GetProduction() != rhs->GetProduction()) {
    return lhs->GetProduction() < rhs->GetProduction();
  }
  if (lhs->GetDotIndex() != rhs->GetDotIndex()) {
    return lhs->GetDotIndex() < rhs->GetDotIndex();
  }
  return lhs->GetTerminator() < rhs->GetTerminator();
}

auto ItemCollection::GetItems() const -> const std::vector<ItemPtr> & {
  return cache_;
}

auto ItemCollection::GetIndex() const -> int { return index_; }
