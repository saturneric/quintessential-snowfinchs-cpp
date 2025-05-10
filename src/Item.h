#pragma once

#include <Production.h>

class Item;
using ItemPtr = std::shared_ptr<Item>;

class Item {
 public:
  const bool generated = false;

  explicit Item(ProductionPtr p_pdt, int m_terminator,
                bool m_generated = false);

  void SetDotIndex(int m_dot_index);

  [[nodiscard]] auto GetDotIndex() const -> int;

  [[nodiscard]] auto GetRightSize() -> size_t;

  [[nodiscard]] auto GetDotNextSymbol() const -> int;

  [[nodiscard]] auto GetDotNextISymbol(int i) const -> int;

  [[nodiscard]] auto GetTerminator() const -> int;

  [[nodiscard]] auto GetProduction() const -> ProductionPtr;

 private:
  ProductionPtr production_;
  int dot_index_ = 0;
  const int terminator_ = 0;
};
