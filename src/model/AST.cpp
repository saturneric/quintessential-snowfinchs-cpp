#include "AST.h"

struct ASTNode::Impl {
  ASTNodeType type_;
  ASTNodeTag tag_;
  SymbolPtr symbol_;
  ASTNode* parent_ = nullptr;
  std::vector<ASTNodePtr> children_;
};

ASTNode::ASTNode() : impl_(std::make_unique<ASTNode::Impl>()) {}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr symbol)
    : impl_(std::make_unique<ASTNode::Impl>()) {
  impl_->type_ = type;
  impl_->symbol_ = std::move(symbol);
}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr symbol,
                 std::initializer_list<ASTNodePtr> children)
    : impl_(std::make_unique<ASTNode::Impl>()) {
  impl_->type_ = type;
  impl_->symbol_ = std::move(symbol);
  impl_->children_ = children;
}

ASTNode::~ASTNode() = default;

auto ASTNode::Children() -> std::vector<ASTNodePtr> { return impl_->children_; }

void ASTNode::AddChild(const ASTNodePtr& child, ASTNodeTag tag) {
  if (child == nullptr) return;

  // attach tag to distinguish children
  child->impl_->tag_ = tag;
  // set parent
  child->impl_->parent_ = this;
  impl_->children_.push_back(child);
}

auto ASTNode::Type() -> ASTNodeType { return impl_->type_; }

auto ASTNode::Symbol() -> SymbolPtr { return impl_->symbol_; }

auto ASTNode::Tag() -> ASTNodeTag { return impl_->tag_; }

auto ASTNode::Parent() -> ASTNode* { return impl_->parent_; }
