#pragma once

#include <Production.h>

class Item {
 public:
  const bool generated = false;

  explicit Item(std::shared_ptr<Production> p_pdt, int m_terminator,
                bool m_generated = false)
      : production_(std::move(p_pdt)),
        terminator_(m_terminator),
        generated(m_generated) {}

  void set_dot_index(int m_dot_index);

  [[nodiscard]] int get_dot_index() const { return dot_index_; }

  [[nodiscard]] size_t get_right_size() { return production_->right.size(); }

  int get_dot_next_symbol() const;

  int get_dot_next_i_symbol(int i) const;

  [[nodiscard]] int get_terminator() const { return terminator_; }

  [[nodiscard]] std::shared_ptr<Production> get_production() const {
    return production_;
  }

 private:
  std::shared_ptr<Production> production_;
  int dot_index_ = 0;
  const int terminator_ = 0;
};
