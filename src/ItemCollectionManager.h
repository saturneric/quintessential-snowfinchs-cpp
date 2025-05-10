//
// Created by Administrator on 2021/4/30.
//

#pragma once

#include <ItemCollection.h>

#include <codecvt>
#include <fstream>
#include <utility>

class ItemCollectionManager {
  std::ofstream output;

  int index = 0;

  std::map<size_t, ItemCollection *> ic_map;

  std::map<size_t, ItemCollection *> ic_content_map;

  std::vector<const ItemCollection *> ics;

  std::shared_ptr<GrammarResourcePool> pool;

  std::shared_ptr<Production> start_pdt;

  template <class T>
  inline void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

 public:
  explicit ItemCollectionManager(std::shared_ptr<GrammarResourcePool> pool)
      : pool(std::move(pool)), output("LR1Automata.txt", std::ios::binary) {
    output.imbue(output.getloc());
  }

  ~ItemCollectionManager() { output.close(); }

  void buildItems();

  [[nodiscard]] std::shared_ptr<Production> getStartProduction() const {
    return start_pdt;
  }

  [[nodiscard]] const std::vector<const ItemCollection *> &getItemCollections()
      const {
    return ics;
  }

  ItemCollection *getItemCollectionByHash(size_t hash);

  bool addItemCollection(int idx, int symbol, ItemCollection *p_ic);

  [[nodiscard]] const ItemCollection *getGOTO(int idx, int symbol) const;

  bool GOTO(const ItemCollection *p_ic, int symbol);
};
