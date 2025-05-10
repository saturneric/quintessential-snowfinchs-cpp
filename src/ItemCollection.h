//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_ITEMCOLLECTION_H
#define SYNTAXPARSER_ITEMCOLLECTION_H

#include <GrammarResourcePool.h>
#include <Item.h>

#include <fstream>
#include <functional>
#include <utility>

class ItemCollectionManager;

class ItemCollection {
 public:
  explicit ItemCollection(std::shared_ptr<GrammarResourcePool> pool)
      : pool(std::move(pool)) {}

  [[nodiscard]] auto GetItems() const -> const std::vector<Item *> &;

  [[nodiscard]] auto GetIndex() const -> int;

  auto AddItem(const std::shared_ptr<Production> &p_pdt, int dot_index,
               int terminator, bool generated = false) -> bool;

  void CLOSURE();

  void Print(std::ofstream &output) const;

  [[nodiscard]] auto GetHash() const -> size_t;

 private:
  int index = 0;

  std::map<size_t, Item *> items;

  std::vector<Item *> cache;

  std::shared_ptr<GrammarResourcePool> pool;

  friend ItemCollectionManager;

  template <class T>
  void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  static auto compare_item_ptr(const Item *lhs, const Item *rhs) -> bool;
};

#endif  // SYNTAXPARSER_ITEMCOLLECTION_H
