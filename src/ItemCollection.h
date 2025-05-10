#pragma once

#include <GrammarResourcePool.h>
#include <Item.h>

#include <functional>
#include <sstream>

class ItemCollectionManager;

class ItemCollection {
 public:
  explicit ItemCollection(std::shared_ptr<GrammarResourcePool> pool)
      : pool_(std::move(pool)) {}

  [[nodiscard]] auto GetItems() const -> const std::vector<ItemPtr> &;

  [[nodiscard]] auto GetIndex() const -> int;

  auto AddItem(const ProductionPtr &p_pdt, int dot_index, int terminator,
               bool generated = false) -> bool;

  void CLOSURE();

  void Print(std::stringstream &output) const;

  [[nodiscard]] auto GetHash() const -> size_t;

 private:
  int index_ = 0;

  std::map<size_t, ItemPtr> items_;

  std::vector<ItemPtr> cache_;

  std::shared_ptr<GrammarResourcePool> pool_;

  friend ItemCollectionManager;

  template <class T>
  void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  static auto compare_item_ptr(const ItemPtr &lhs, const ItemPtr &rhs) -> bool;
};
