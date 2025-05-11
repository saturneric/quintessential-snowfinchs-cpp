#include "SemanticAnalyzer.h"

#include <iostream>

auto SemanticAnalyzer::visit(const ASTNodePtr& node) -> ASTNodePtr {
  if (!node) {
    error(nullptr, "The AST is empty");
    return nullptr;
  }

  switch (node->Type()) {
    case ASTNodeType::kDECLARE: {
      auto name = node->Operation()->Value();
      if (!declare(name, "int")) {
        error(node, "Redefine: " + name);
      }

      if (!node->Children().empty()) {
        auto expr_type = visit_expr(node->Children().front());
        if (expr_type != ExpType::kINT) {
          error(node, "Type mismatch, expecting int");
        }
        lookup(name)->SetMetaData("initialization", "1");
      }
      break;
    }

    case ASTNodeType::kASSIGN: {
      auto children = node->Children();

      if (children.empty()) break;

      auto& target = children.front();
      auto& value = children.back();

      auto var = target->Operation()->Value();
      auto sym = lookup(var);
      if (!sym) error(node, "Undeclared variables: " + var);

      auto expr_type = visit_expr(value);

      break;
    }

    case ASTNodeType::kRETURN: {
      break;
    }
    case ASTNodeType::kPROGRAM:
    default:
      for (auto& c : node->Children()) visit(c);
  }
  return node;
}

auto SemanticAnalyzer::lookup(const std::string& name) -> SymbolPtr {
  for (int i = scopes_ - 1; i >= 0; --i) {
    auto symbol = symbol_table_->SearchSemanticSymbol(scopes_, name);
    if (symbol != nullptr) return symbol;
  }
  return nullptr;
}

auto SemanticAnalyzer::visit_expr(const ASTNodePtr& expr) -> ExpType {
  if (expr == nullptr) return ExpType::kINT;

  switch (expr->Type()) {
    case ASTNodeType::kVALUE:
      if (is_integer_literal(expr->Operation()->Value())) {
        return ExpType::kINT;
      } else {
        auto var = expr->Operation()->Value();
        auto sym = lookup(var);
        if (!sym) {
          error(expr, "Undeclared variables: " + var);
        }
        if (sym->MetaData("initialization").empty()) {
          error(expr, "Variable not initialized: " + var);
        }
        return ExpType::kINT;
      }

    case ASTNodeType::kIDENT: {
      auto var = expr->Operation()->Value();
      auto sym = lookup(var);
      if (!sym) {
        error(expr, "Undeclared variables: " + var);
        return ExpType::kINT;
      }

      if (sym->MetaData("initialization").empty()) {
        error(expr, "Variable not initialized: " + var);
        return ExpType::kINT;
      }

      return ExpType::kINT;
    }

    case ASTNodeType::kBIN_OP: {
      auto children = expr->Children();
      auto lhs = visit_expr(children.front());
      auto rhs = visit_expr(children.back());
      if (lhs != ExpType::kINT || rhs != ExpType::kINT) {
        error(expr, "Both sides of a binary operation must be int");
      }
      return ExpType::kINT;
    }

    case ASTNodeType::kUN_OP: {
      auto val = visit_expr(expr->Children()[0]);
      if (val != ExpType::kINT) error(expr, "Unary - operand must be int");
      return ExpType::kINT;
    }

    default:
      return ExpType::kINT;
  }
}

auto SemanticAnalyzer::is_integer_literal(const std::string& s) -> bool {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void SemanticAnalyzer::error(const ASTNodePtr& /*node*/,
                             const std::string& msg) {
  std::fprintf(stderr, "Semantic Error: %s\n", msg.c_str());
  succ_ = false;
}

auto SemanticAnalyzer::declare(const std::string& name, const std::string& type)
    -> bool {
  auto symbol = symbol_table_->SearchSemanticSymbol(scopes_, name);
  if (symbol != nullptr) return false;
  symbol_table_->AddSemanticSymbol(scopes_, name, type);
  return true;
}

auto SemanticAnalyzer::Analyze(const AST& ast) -> bool {
  succ_ = true;

  enter_scope();
  visit(ast.Root());
  leave_scope();

  return succ_;
}

SemanticAnalyzer::SemanticAnalyzer(SymbolTablePtr symbol_table)
    : symbol_table_(std::move(symbol_table)) {};
void SemanticAnalyzer::enter_scope() { scopes_++; }
void SemanticAnalyzer::leave_scope() { scopes_--; }
