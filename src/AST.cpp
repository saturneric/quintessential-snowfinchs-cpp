#include "AST.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

const std::map<ASTNodeType, std::string> kAstNodeTypeStr = {
    {ASTNodeType::kPROGRAM, "Program"},
    {ASTNodeType::kASSIGN, "Assign"},
    {ASTNodeType::kDECLARE, "Declare"},
    {ASTNodeType::kBIN_OP, "Binary Operation"},
    {ASTNodeType::kUN_OP, "Unary Operation"},
    {ASTNodeType::kRETURN, "Return"},
    {ASTNodeType::kVALUE, "Value"},
    {ASTNodeType::kTYPE, "Type"},
};

auto GenerateASTSymbol(const SymbolTablePtr& table, const SymbolPtr& symbol) {
  if (symbol == nullptr) {
    return table->AddASTSymbol("", "");
  }

  if (symbol->Type() == SymbolType::kTOKEN) {
    return table->AddASTSymbol(symbol->Value(), symbol->Name());
  }

  return table->AddASTSymbol(symbol->Name(), symbol->Value());
}

auto HandleUselessSyntaxNode(const ASTNodePtr& parent, TreeNode* syntax,
                             const SymbolTablePtr& /*table*/,
                             const AST::RouterFunc& f) -> ASTNodePtr {
  if (syntax == nullptr) return nullptr;
  for (const auto& syntax_child : syntax->GetChildren()) {
    if (parent != nullptr) parent->AddChildren(f(parent, syntax_child));
  }
  return nullptr;
}

auto HandleValueNode(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                     const SymbolTablePtr& table, const AST::RouterFunc& /*f*/)
    -> ASTNodePtr {
  auto tokens = syntax->Tokens();
  if (tokens.empty()) return nullptr;
  return std::make_shared<ASTNode>(ASTNodeType::kVALUE,
                                   GenerateASTSymbol(table, tokens.front()));
}

auto HandleProgram(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                   const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto ast = std::make_shared<ASTNode>(
      ASTNodeType::kPROGRAM, GenerateASTSymbol(table, syntax->Syntax()));
  for (const auto& child : syntax->GetChildren()) {
    ast->AddChildren(f(ast, child));
  }
  return ast;
}

auto HandleAssign(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                  const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;
  if (syntax_children.size() != 3) return nullptr;

  // <lvalue> <asnop> <exp>

  auto* assign_operator = syntax_children[1];

  auto tokens = assign_operator->Tokens();
  if (tokens.empty()) return nullptr;

  auto ast = std::make_shared<ASTNode>(ASTNodeType::kASSIGN,
                                       GenerateASTSymbol(table, tokens.back()));

  ast->AddChildren(f(ast, syntax_children.front()));
  ast->AddChildren(f(ast, syntax_children.back()));
  return ast;
}

auto HandleReturn(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                  const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;

  auto ast = std::make_shared<ASTNode>(
      ASTNodeType::kRETURN, GenerateASTSymbol(table, syntax->Syntax()));
  ast->AddChildren(f(ast, syntax_children.front()));
  return ast;
}

auto HandleRootExpression(const ASTNodePtr& parent, TreeNode* syntax,
                          const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;

  // <exp> <binop> <exp>
  if (syntax_children.size() == 3) {
    auto* binary_operator = syntax_children[1];

    auto tokens = binary_operator->Tokens();
    if (tokens.empty()) return nullptr;

    auto ast = std::make_shared<ASTNode>(
        ASTNodeType::kBIN_OP, GenerateASTSymbol(table, tokens.back()));

    ast->AddChildren(f(ast, syntax_children.front()));
    ast->AddChildren(f(ast, syntax_children.back()));
    return ast;
  }

  // <exp>
  return HandleUselessSyntaxNode(parent, syntax, table, f);
}

auto HandleExpression(const ASTNodePtr& parent, TreeNode* syntax,
                      const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto tokens = syntax->Tokens();

  if (tokens.empty()) {
    // exp -> <unop> <exp>

    if (syntax_children.front()->NodeType() == "unary_operator") {
      auto tokens = syntax_children.front()->Tokens();
      if (tokens.empty()) return nullptr;

      auto ast = std::make_shared<ASTNode>(
          ASTNodeType::kUN_OP, GenerateASTSymbol(table, tokens.front()));

      // <exp>
      ast->AddChildren(f(ast, syntax_children.back()));
      return ast;
    }

    // int_const
    return HandleUselessSyntaxNode(parent, syntax, table, f);
  }

  // ident
  if (tokens.front()->Value() == "VALUE_ID") {
    return HandleValueNode(parent, syntax, table, f);
  }

  if (syntax_children.empty()) return nullptr;

  auto* first_syntax_child = syntax_children.front();

  // exp -> ( <exp> )
  if (tokens.back()->Value() == "RIGHT_BRACKET") {
    return HandleUselessSyntaxNode(parent, first_syntax_child, table, f);
  }

  return HandleUselessSyntaxNode(parent, syntax, table, f);
}

auto HandleDeclarator(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                      const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto tokens = syntax->Tokens();

  if (tokens.size() < 2) return nullptr;

  auto ast = std::make_shared<ASTNode>(
      ASTNodeType::kDECLARE, GenerateASTSymbol(table, syntax->Syntax()));

  // int
  ast->AddChildren(std::make_shared<ASTNode>(
      ASTNodeType::kTYPE, GenerateASTSymbol(table, tokens.front())));

  // ident
  ast->AddChildren(std::make_shared<ASTNode>(
      ASTNodeType::kVALUE, GenerateASTSymbol(table, tokens[1])));

  // int ident <exp>
  if (!syntax_children.empty()) {
    ast->AddChildren(f(ast, syntax_children.front()));
  }

  return ast;
}

auto HandleLeftValue(const ASTNodePtr& parent, TreeNode* syntax,
                     const SymbolTablePtr& table, const AST::RouterFunc& f)
    -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto tokens = syntax->Tokens();
  if (tokens.empty()) return nullptr;

  if (tokens.back()->Value() == "VALUE_ID") {
    return HandleValueNode(parent, syntax, table, f);
  }

  return HandleUselessSyntaxNode(parent, syntax, table, f);
}

}  // namespace

std::map<std::string, AST::HandlerFunc> AST::handler_registry = {
    {"ProgramNode", HandleProgram},
    {"ValueNode", HandleValueNode},
    {"UselessNode", HandleUselessSyntaxNode},
    {"RootExpressionNode", HandleRootExpression},
    {"ReturnNode", HandleReturn},
    {"AssignNode", HandleAssign},
    {"DeclaratorNode", HandleDeclarator},
    {"ExpressionNode", HandleExpression},
    {"LeftValueNode", HandleLeftValue},
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

  stream << "<" << node->Operation()->Name() << ","
         << node->Operation()->Value() << ">";

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

auto AST::do_build_tree(const ASTNodePtr& ast_node, TreeNode* syntax_node)
    -> std::shared_ptr<ASTNode> {
  auto node_type = syntax_node->NodeType();

  if (syntax_symbol_to_handler_.count(node_type) == 0U) return nullptr;
  if (syntax_symbol_to_handler_.at(node_type) == nullptr) return nullptr;
  if (syntax_node == nullptr) return nullptr;

  return syntax_symbol_to_handler_[node_type](
      ast_node, syntax_node, symbol_table_, [this](auto&& PH1, auto&& PH2) {
        return do_build_tree(std::forward<decltype(PH1)>(PH1),
                             std::forward<decltype(PH2)>(PH2));
      });
}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr opera)
    : type_(type), opera_(std::move(opera)) {}

ASTNode::ASTNode(ASTNodeType type, SymbolPtr opera,
                 std::initializer_list<ASTNodePtr> children)
    : type_(type), opera_(std::move(opera)), children_(children) {}

auto ASTNode::Children() -> std::vector<ASTNodePtr> { return children_; }

void ASTNode::AddChildren(const ASTNodePtr& child) {
  if (child != nullptr) children_.push_back(child);
}

auto ASTNode::Type() -> ASTNodeType { return type_; }

auto ASTNode::Operation() -> SymbolPtr { return opera_; }

auto AST::Build(const SyntaxTree& tree) -> bool {
  if (tree.Root() == nullptr) return false;

  root_ = do_build_tree(nullptr, tree.Root());
  return root_ == nullptr;
}

auto AST::Root() const -> ASTNodePtr { return root_; }

void AST::LoadBinding(const std::string& path) {
  std::map<std::string, std::string> action_map;
  std::ifstream infile(path);
  std::string line;

  while (std::getline(infile, line)) {
    auto comment_pos = line.find('#');
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }

    std::istringstream iss(line);
    std::string symbol;
    std::string handler;
    if (iss >> symbol >> handler) {
      if (handler_registry.count(handler) == 0) {
        std::cout << "Cannot bind Syntax Symbol: " << symbol
                  << " to AST Handler: " << handler;
        continue;
      }
      syntax_symbol_to_handler_[symbol] = handler_registry.at(handler);
    }
  }
}

AST::AST(std::shared_ptr<SymbolTable> symbol_table)
    : symbol_table_(std::move(symbol_table)) {};