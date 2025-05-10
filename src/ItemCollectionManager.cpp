//
// Created by Administrator on 2021/4/30.
//

#include "ItemCollectionManager.h"

void ItemCollectionManager::buildItems() {
  const auto *const start_symbol = pool->GetStartSymbol();

  std::string new_symbol_name = start_symbol->name + "'";

  int new_symbol_index =
      pool->AddSymbol(new_symbol_name, start_symbol->terminator);

  pool->ModifySymbol(start_symbol->index, start_symbol->name.substr(1), false,
                     false);

  const auto p_pdt =
      pool->AddProduction(new_symbol_index, {start_symbol->index});

  this->start_pdt = p_pdt;

  auto *pi_ic = new ItemCollection(pool);

  pi_ic->AddItem(p_pdt, 0, kStopSymbolId);

  pi_ic->CLOSURE();

  addItemCollection(0, 0, pi_ic);

  bool if_add = true;

  while (if_add) {
    if_add = false;
    const auto &r_ics = getItemCollections();
    std::vector<const ItemCollection *> temp_ics(r_ics.begin(), r_ics.end());
    for (const auto ic : temp_ics) {
      for (const auto symbol : pool->GetAllSymbols()) {
        if (symbol->index <= 0) {
          continue;
        }
        if (GOTO(ic, symbol->index)) {
          if_add = true;
        }
      }
    }
  }
}

ItemCollection *ItemCollectionManager::getItemCollectionByHash(size_t hash) {
  ItemCollection *p_ic = nullptr;
  auto it = ic_content_map.find(hash);
  if (it != ic_content_map.end()) {
    p_ic = it->second;
  }
  return p_ic;
}

bool ItemCollectionManager::addItemCollection(int idx, int symbol,
                                              ItemCollection *p_ic) {
  size_t ic_hash = p_ic->GetHash();
  auto it = ic_content_map.find(ic_hash);
  if (it != ic_content_map.end()) {
    p_ic = it->second;
  } else {
    p_ic->index = this->index++;
    ic_content_map.insert(std::pair<size_t, ItemCollection *>(ic_hash, p_ic));
    ics.push_back(p_ic);
  }

  auto hasher = std::hash<int>();
  size_t seed = hasher(idx);
  hash_combine(seed, symbol);

  auto it2 = ic_map.find(seed);
  if (it2 != ic_map.end()) {
    return false;
  }

  if (symbol != 0) {
    auto p_symbol = pool->GetSymbol(symbol);
    if (p_symbol->terminator)
      output << "GOTO(" << idx << ", \"" << p_symbol->name << "\")"
             << std::endl;
    else
      output << "GOTO(" << idx << ", " << p_symbol->name << ")" << std::endl;
  } else {
    output << "GOTO(" << idx << ", [Epsilon])" << std::endl;
  }

  ic_map.insert(std::pair<size_t, ItemCollection *>(seed, p_ic));
  p_ic->Print(output);
  return true;
}

const ItemCollection *ItemCollectionManager::getGOTO(int idx,
                                                     int symbol) const {
  auto hasher = std::hash<int>();
  size_t seed = hasher(idx);
  hash_combine(seed, symbol);

  auto it = ic_map.find(seed);
  if (it != ic_map.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

bool ItemCollectionManager::GOTO(const ItemCollection *p_ic, int symbol) {
  auto *pt_ic = new ItemCollection(pool);

  for (const auto &item : p_ic->cache) {
    if (item->get_dot_next_symbol() == symbol) {
      pt_ic->AddItem(item->get_production(), item->get_dot_index() + 1,
                     item->get_terminator());
    }
  }
  auto *p_temp_ic = this->getItemCollectionByHash(pt_ic->GetHash());
  if (p_temp_ic == nullptr)
    pt_ic->CLOSURE();
  else
    pt_ic = p_temp_ic;

  if (!pt_ic->items.empty()) {
    return this->addItemCollection(p_ic->index, symbol, pt_ic);
  } else {
    return false;
  }
}
