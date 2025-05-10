#pragma once

#include <ItemCollection.h>

#include <sstream>

class ItemCollectionManager {
  std::stringstream output_;

  int index_ = 0;

  std::map<size_t, std::shared_ptr<ItemCollection>> ic_map_;

  std::map<size_t, std::shared_ptr<ItemCollection>> ic_content_map_;

  std::vector<std::shared_ptr<ItemCollection>> ics_;

  std::shared_ptr<GrammarResourcePool> pool_;

  ProductionPtr start_pdt_;

  template <class T>
  void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

 public:
  explicit ItemCollectionManager(std::shared_ptr<GrammarResourcePool> pool);

  void BuildItems();

  [[nodiscard]] auto GetStartProduction() const -> ProductionPtr;

  [[nodiscard]] auto GetItemCollections() const
      -> std::vector<std::shared_ptr<ItemCollection>>;

  auto GetItemCollectionByHash(size_t hash) -> std::shared_ptr<ItemCollection>;

  auto AddItemCollection(int idx, int symbol,
                         std::shared_ptr<ItemCollection> p_ic) -> bool;

  [[nodiscard]] auto GetGoto(int idx, int symbol) const
      -> std::shared_ptr<ItemCollection>;

  auto GOTO(const std::shared_ptr<ItemCollection> &p_ic, int symbol) -> bool;
};
