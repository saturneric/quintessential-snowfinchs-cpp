#include "AST.h"

#include <fstream>
#include <stack>
#include <utility>

struct ASTNode::Impl {
  ASTNodeType type_;
  ASTNodeTag tag_;
  SymbolPtr symbol_;
  ASTNode* parent_ = nullptr;
  std::vector<ASTNodePtr> children_;
};

const std::map<ASTNodeType, std::string> kAstNodeTypeStr = {
    {ASTNodeType::kPROGRAM, "Program"},
    {ASTNodeType::kASSIGN, "Assign"},
    {ASTNodeType::kDECLARE, "Declare"},
    {ASTNodeType::kBIN_OP, "Binary Operation"},
    {ASTNodeType::kUN_OP, "Unary Operation"},
    {ASTNodeType::kRETURN, "Return"},
    {ASTNodeType::kVALUE, "Value"},
    {ASTNodeType::kTYPE, "Type"},
    {ASTNodeType::kIDENT, "Identity"},
    {ASTNodeType::kBREAK, "Break"},
    {ASTNodeType::kCONTINUE, "Continue"},
    {ASTNodeType::kWHILE, "While"},
    {ASTNodeType::kIF, "If"},
    {ASTNodeType::kCOND_EXP, "Conditional Expression"},
    {ASTNodeType::kBLOCK, "Block"},
    {ASTNodeType::kBREAK, "Break"},
    {ASTNodeType::kCONTINUE, "Continue"},
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

struct AST::Impl {
  SymbolTablePtr symbol_table_;
  std::shared_ptr<ASTNode> root_ = nullptr;
  std::stack<int> tab_stack_;
  const int spaces_in_tab_ = 4;
};

AST::AST(SymbolTablePtr symbol_table) : impl_(std::make_unique<AST::Impl>()) {
  impl_->symbol_table_ = std::move(symbol_table);
}

AST::~AST() = default;

auto AST::Root() const -> ASTNodePtr { return impl_->root_; }

void AST::SetRoot(const ASTNodePtr& root) { impl_->root_ = root; }

void AST::do_ast_node_print(const ASTNodePtr& node, std::ofstream& stream) {
  if (node == nullptr) return;

  impl_->tab_stack_.push(impl_->tab_stack_.top() + 1);

  for (int i = impl_->tab_stack_.top() * impl_->spaces_in_tab_; i > 0; i--) {
    if (i % impl_->spaces_in_tab_ == 0) {
      stream << '|';
    }
    stream << ' ';
  }

  auto type = node->Type();
  if (kAstNodeTypeStr.count(type) != 0) {
    stream << kAstNodeTypeStr.at(type);
  } else {
    stream << "Unknown";
  }

  stream << "<" << node->Symbol()->Name() << "," << node->Symbol()->Value()
         << "," << node->Symbol()->ScopeId() << ">";

  stream << '\n';

  for (const auto& child : node->Children()) {
    do_ast_node_print(child, stream);
  }

  impl_->tab_stack_.pop();
}

void AST::Print(const std::string& path) {
  if (impl_->root_ == nullptr) return;

  impl_->tab_stack_.push(-1);

  std::ofstream f(path);

  do_ast_node_print(impl_->root_, f);

  f.close();
}