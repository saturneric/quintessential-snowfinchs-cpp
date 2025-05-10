#include "Item.h"

void Item::SetDotIndex(int m_dot_index) {
  if (m_dot_index > production_->right.size()) {
    throw std::runtime_error("DOT_INDEX out of range");
  }
  this->dot_index_ = m_dot_index;
}

int Item::GetDotNextSymbol() const {
  if (GetDotIndex() == production_->right.size()) {
    return 0;
  } else {
    return production_->right[dot_index_];
  }
}

int Item::GetDotNextISymbol(int i) const {
  if (GetDotIndex() + i >= production_->right.size()) {
    return 0;
  } else {
    return production_->right[dot_index_ + i];
  }
}
auto Item::GetDotIndex() const -> int { return dot_index_; }
auto Item::GetRightSize() -> size_t { return production_->right.size(); }
auto Item::GetTerminator() const -> int { return terminator_; }
auto Item::GetProduction() const -> ProductionPtr { return production_; }
Item::Item(ProductionPtr p_pdt, int m_terminator, bool m_generated)
    : production_(std::move(p_pdt)),
      terminator_(m_terminator),
      generated(m_generated) {}
