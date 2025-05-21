#include "AST.h"

#include <fstream>

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
    {ASTNodeType::kBLOCK, "Block"},
};

void AST::do_ast_node_print(const ASTNodePtr& node, std::ofstream& stream) {
  if (node == nullptr) return;

  tab_stack_.push(tab_stack_.top() + 1);

  for (int i = tab_stack_.top() * spaces_in_tab_; i > 0; i--) {
    if (i % spaces_in_tab_ == 0) {
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

  tab_stack_.pop();
}

void AST::Print(const std::string& path) {
  if (root_ == nullptr) return;

  tab_stack_.push(-1);

  std::ofstream f(path);

  do_ast_node_print(root_, f);

  f.close();
}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr symbol)
    : type_(type), symbol_(std::move(symbol)) {}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr symbol,
                 std::initializer_list<ASTNodePtr> children)
    : type_(type), symbol_(std::move(symbol)), children_(children) {}

auto ASTNode::Children() -> std::vector<ASTNodePtr> { return children_; }

void ASTNode::AddChild(const ASTNodePtr& child, ASTNodeTag tag) {
  if (child == nullptr) return;

  // attach tag to distinguish children
  child->tag_ = tag;
  children_.push_back(child);
}

auto ASTNode::Type() -> ASTNodeType { return type_; }

auto ASTNode::Symbol() -> SymbolPtr { return symbol_; }

auto AST::Root() const -> ASTNodePtr { return root_; }

AST::AST(std::shared_ptr<SymbolTable> symbol_table)
    : symbol_table_(std::move(symbol_table)) {};

void AST::SetRoot(const ASTNodePtr& root) { root_ = root; }

auto ASTNode::Tag() -> ASTNodeTag { return tag_; }
