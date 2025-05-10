#include "ItemCollectionManager.h"

#include <iostream>

void ItemCollectionManager::BuildItems() {
  auto start_symbol = pool_->GetStartSymbol();
  auto new_symbol_name = start_symbol->Name() + "'";

  start_symbol->SetName(start_symbol->Name().substr(1));
  start_symbol->SetStartSymbol(false);

  auto new_start_symbol =
      pool_->AddSymbol(new_symbol_name, start_symbol->IsTerminator());
  new_start_symbol->SetStartSymbol(true);

  const auto p_pdt =
      pool_->AddProduction(new_start_symbol->Index(), {start_symbol->Index()});

  this->start_pdt_ = p_pdt;

  auto pi_ic = std::make_shared<ItemCollection>(pool_);
  pi_ic->AddItem(p_pdt, 0, kStopSymbolId);
  pi_ic->CLOSURE();

  AddItemCollection(0, 0, pi_ic);

  bool is_add = true;

  while (is_add) {
    is_add = false;
    const auto &r_ics = GetItemCollections();
    std::vector<std::shared_ptr<ItemCollection>> temp_ics(r_ics.begin(),
                                                          r_ics.end());
    for (const auto &ic : temp_ics) {
      for (const auto &symbol : pool_->GetAllSymbols()) {
        if (symbol->Index() == kEmptySymbolId ||
            symbol->Index() == kStopSymbolId) {
          continue;
        }
        if (GOTO(ic, symbol->Index())) is_add = true;
      }
    }
  }
}

auto ItemCollectionManager::GetItemCollectionByHash(size_t hash)
    -> std::shared_ptr<ItemCollection> {
  std::shared_ptr<ItemCollection> p_ic = nullptr;
  auto it = ic_content_map_.find(hash);
  if (it != ic_content_map_.end()) {
    p_ic = it->second;
  }
  return p_ic;
}

auto ItemCollectionManager::AddItemCollection(
    int idx, int symbol, std::shared_ptr<ItemCollection> p_ic) -> bool {
  size_t ic_hash = p_ic->GetHash();
  auto it = ic_content_map_.find(ic_hash);
  if (it != ic_content_map_.end()) {
    p_ic = it->second;
  } else {
    p_ic->index_ = this->index_++;
    ic_content_map_.insert(
        std::pair<size_t, std::shared_ptr<ItemCollection>>(ic_hash, p_ic));
    ics_.push_back(p_ic);
  }

  auto hasher = std::hash<int>();
  size_t seed = hasher(idx);
  hash_combine(seed, symbol);

  auto it2 = ic_map_.find(seed);
  if (it2 != ic_map_.end()) {
    return false;
  }

  if (symbol != 0) {
    const auto p_symbol = pool_->GetSymbol(symbol);
    if (p_symbol->IsTerminator()) {
      output_ << "GOTO(" << idx << ", \"" << p_symbol->Name() << "\")" << '\n';
    } else {
      output_ << "GOTO(" << idx << ", " << p_symbol->Name() << ")" << '\n';
    }
  } else {
    output_ << "GOTO(" << idx << ", [Epsilon])" << '\n';
  }

  ic_map_.insert({seed, p_ic});
  p_ic->Print(output_);
  return true;
}

std::shared_ptr<ItemCollection> ItemCollectionManager::GetGoto(
    int idx, int symbol) const {
  auto hasher = std::hash<int>();
  size_t seed = hasher(idx);
  hash_combine(seed, symbol);

  auto it = ic_map_.find(seed);
  if (it != ic_map_.end()) {
    return it->second;
  }
  return nullptr;
}

bool ItemCollectionManager::GOTO(const std::shared_ptr<ItemCollection> &p_ic,
                                 int symbol) {
  auto pt_ic = std::make_shared<ItemCollection>(pool_);

  for (const auto &item : p_ic->cache_) {
    if (item->GetDotNextSymbol() == symbol) {
      pt_ic->AddItem(item->GetProduction(), item->GetDotIndex() + 1,
                     item->GetTerminator());
    }
  }
  auto p_temp_ic = this->GetItemCollectionByHash(pt_ic->GetHash());
  if (p_temp_ic == nullptr) {
    pt_ic->CLOSURE();
  } else {
    pt_ic = p_temp_ic;
  }

  if (!pt_ic->items_.empty()) {
    return this->AddItemCollection(p_ic->index_, symbol, pt_ic);
  }

  return false;
}

ItemCollectionManager::ItemCollectionManager(
    std::shared_ptr<GrammarResourcePool> pool)
    : pool_(std::move(pool)) {}

auto ItemCollectionManager::GetStartProduction() const -> ProductionPtr {
  return start_pdt_;
}

auto ItemCollectionManager::GetItemCollections() const
    -> std::vector<std::shared_ptr<ItemCollection>> {
  return ics_;
}

void ItemCollectionManager::Print(const std::string &path) {
  std::ofstream f(path);
  f << output_.str();
  f.close();
}
