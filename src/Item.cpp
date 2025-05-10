//
// Created by Administrator on 2021/4/30.
//

#include "Item.h"

void Item::set_dot_index(int m_dot_index) {
    if(m_dot_index > production_->right.size()) {
        throw std::runtime_error("DOT_INDEX out of range");
    }
    this->dot_index_ = m_dot_index;
}

int Item::get_dot_next_symbol() const {
    if(get_dot_index() == production_->right.size()) {
        return 0;
    } else {
        return production_->right[dot_index_];
    }
}

int Item::get_dot_next_i_symbol(int i) const {
    if(get_dot_index() + i >= production_->right.size()) {
        return 0;
    } else {
        return production_->right[dot_index_ + i];
    }
}
