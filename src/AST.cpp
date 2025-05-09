#include "AST.h"

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

auto HandleUselessSyntaxNode(const ASTNodePtr& parent, TreeNode* syntax,
                             const AST::RouterFunc& f) -> ASTNodePtr {
  for (const auto& syntax_child : syntax->GetChildren()) {
    if (parent != nullptr) parent->AddChildren(f(parent, syntax_child));
  }
  return nullptr;
}

auto HandleValueNode(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                     const AST::RouterFunc& /*f*/) -> ASTNodePtr {
  auto syntax_value_vec = syntax->GetValueVec();
  if (syntax_value_vec.empty()) return nullptr;

  auto ast =
      std::make_shared<ASTNode>(ASTNodeType::kVALUE, syntax_value_vec.front());
  return ast;
}

auto HandleProgram(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                   const AST::RouterFunc& f) -> ASTNodePtr {
  auto ast = std::make_shared<ASTNode>(ASTNodeType::kPROGRAM, "");

  for (const auto& child : syntax->GetChildren()) {
    ast->AddChildren(f(ast, child));
  }
  return ast;
}

auto HandleAssign(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                  const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;
  if (syntax_children.size() != 3) return nullptr;

  // <lvalue> <asnop> <exp>

  auto* assign_operator = syntax_children[1];
  auto ast = std::make_shared<ASTNode>(ASTNodeType::kASSIGN,
                                       assign_operator->GetInfoVec().back());

  ast->AddChildren(f(ast, syntax_children.front()));
  ast->AddChildren(f(ast, syntax_children.back()));
  return ast;
}

auto HandleReturn(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                  const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;

  auto ast = std::make_shared<ASTNode>(ASTNodeType::kRETURN, "return");
  ast->AddChildren(f(ast, syntax_children.front()));
  return ast;
}

auto HandleRootExpression(const ASTNodePtr& parent, TreeNode* syntax,
                          const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  if (syntax_children.empty()) return nullptr;

  // <exp> <binop> <exp>
  if (syntax_children.size() == 3) {
    auto* binary_operator = syntax_children[1];
    auto ast = std::make_shared<ASTNode>(ASTNodeType::kBIN_OP,
                                         binary_operator->GetInfoVec().back());

    ast->AddChildren(f(ast, syntax_children.front()));
    ast->AddChildren(f(ast, syntax_children.back()));
    return ast;
  }

  // <exp>

  return HandleUselessSyntaxNode(parent, syntax, f);
}

auto HandleExpression(const ASTNodePtr& parent, TreeNode* syntax,
                      const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto syntax_info_vec = syntax->GetInfoVec();
  if (syntax_info_vec.empty()) return nullptr;

  if (syntax_info_vec.size() > 1 && syntax_info_vec[1] == "ID") {
    return HandleValueNode(parent, syntax, f);
  }

  if (syntax_children.empty()) return nullptr;

  auto* first_syntax_child = syntax_children.front();

  // exp -> ( <exp> )
  if (syntax_info_vec.size() > 1 && syntax_info_vec.back() == "RIGHT_BRACKET") {
    return HandleUselessSyntaxNode(parent, first_syntax_child, f);
  }

  // exp -> <unop> <exp>

  if (first_syntax_child->NodeType() == "unary_operator") {
    auto ast = std::make_shared<ASTNode>(
        ASTNodeType::kUN_OP, first_syntax_child->GetInfoVec().back());

    // <exp>
    ast->AddChildren(f(ast, syntax_children.back()));
    return ast;
  }

  for (const auto& syntax_child : syntax->GetChildren()) {
    return HandleUselessSyntaxNode(parent, first_syntax_child, f);
  }

  return nullptr;
}

auto HandleDeclarator(const ASTNodePtr& /*parent*/, TreeNode* syntax,
                      const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto syntax_info_vec = syntax->GetInfoVec();
  auto syntax_value_vec = syntax->GetValueVec();
  if (syntax_info_vec.size() < 3) return nullptr;

  auto ast = std::make_shared<ASTNode>(ASTNodeType::kDECLARE, "declare");

  // int ident

  if (syntax_info_vec.size() > 2) {
    // int
    ast->AddChildren(std::make_shared<ASTNode>(ASTNodeType::kTYPE,
                                               syntax_value_vec.front()));

    // ident
    ast->AddChildren(
        std::make_shared<ASTNode>(ASTNodeType::kVALUE, syntax_value_vec[1]));
  }

  // int ident <exp>

  if (!syntax_children.empty()) {
    ast->AddChildren(f(ast, syntax_children.front()));
  }

  return ast;
}

auto HandleLeftValue(const ASTNodePtr& parent, TreeNode* syntax,
                     const AST::RouterFunc& f) -> ASTNodePtr {
  auto syntax_children = syntax->GetChildren();
  auto syntax_info_vec = syntax->GetInfoVec();
  if (syntax_info_vec.empty()) return nullptr;

  if (syntax_info_vec.back() == "ID") {
    return HandleValueNode(parent, syntax, f);
  }

  return HandleUselessSyntaxNode(parent, syntax, f);
}

}  // namespace

std::map<std::string, AST::HandlerFunc> AST::handler_registry = {
    {"program", HandleProgram},
    {"function_name", HandleValueNode},
    {"statements", HandleUselessSyntaxNode},
    {"statement", HandleUselessSyntaxNode},
    {"more_statement", HandleUselessSyntaxNode},
    {"root_expression", HandleRootExpression},
    {"left_expression", HandleUselessSyntaxNode},
    {"right_expression", HandleUselessSyntaxNode},
    {"return_statement", HandleReturn},
    {"simple_statement", HandleAssign},
    {"declarator", HandleDeclarator},
    {"expression", HandleExpression},
    {"left_value", HandleLeftValue},
    {"int_const", HandleValueNode}};

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

  stream << "[" << node->Operation() << "]";

  stream << '\n';

  for (const auto& child : node->Children()) {
    do_ast_node_print(child, stream);
  }

  tab_stack_.pop();
}

void AST::Print() {
  if (root_ == nullptr) return;

  tab_stack_.push(-1);

  std::ofstream f("AST.txt");

  do_ast_node_print(root_, f);

  f.close();
}
