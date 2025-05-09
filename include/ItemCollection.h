//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_ITEMCOLLECTION_H
#define SYNTAXPARSER_ITEMCOLLECTION_H

#include <GrammarResourcePool.h>
#include <Item.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>

class ItemCollectionManager;

class ItemCollection {
  int index = 0;

  std::map<size_t, Item *> items;

  std::vector<Item *> cache;

  GrammarResourcePool *pool;

  friend ItemCollectionManager;

  template <class T>
  inline void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  static bool compare_item_ptr(const Item *lhs, const Item *rhs);

 public:
  explicit ItemCollection(GrammarResourcePool *pool) : pool(pool) {}

  [[nodiscard]] const std::vector<Item *> &getItems() const { return cache; }

  [[nodiscard]] int getIndex() const { return index; }

  bool addItem(const Production *p_pdt, int dot_index, int terminator,
               bool generated = false);

  void CLOSURE();

  void print(std::ofstream &output) const;

  [[nodiscard]] size_t getHash() const;
};

#endif  // SYNTAXPARSER_ITEMCOLLECTION_H
