#include "SemanticAnalyzer.h"

#include <algorithm>

#include "Utils.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTablePtr symbol_table)
    : symbol_table_(std::move(symbol_table)) {};

auto SemanticAnalyzer::visit(const ASTNodePtr& node) -> ASTNodePtr {
  if (!node) {
    error(nullptr, "The AST is empty");
    return nullptr;
  }

  switch (node->Type()) {
    case ASTNodeType::kDECLARE: {
      auto name = node->Operation()->Value();
      if (!declare_ident(name, "int")) {
        error(node, "Redefine identity: " + name);
      }

      auto children = node->Children();
      if (children.empty()) break;

      const auto& assign = children.front();
      auto expr_type = visit(assign);

      lookup_ident(name)->SetMetaData("initialization", "1");
      break;
    }

    case ASTNodeType::kASSIGN: {
      auto children = node->Children();
      if (children.empty()) break;

      auto& target = children.front();
      auto& value = children.back();

      auto var = target->Operation()->Value();

      auto sym = lookup_ident(var);
      if (!sym) {
        error(node, "Undeclared variables: " + var);
        break;
      }

      auto expr_type = visit_expr(value);

      sym->SetMetaData("initialization", "1");
      break;
    }

    case ASTNodeType::kRETURN: {
      auto children = node->Children();

      if (children.empty()) break;
      auto& value = children.back();
      auto expr_type = visit_expr(value);
      break;
    }

    case ASTNodeType::kPROGRAM: {
      bool has_return = false;
      for (auto& c : node->Children()) {
        if (c->Type() == ASTNodeType::kRETURN) has_return = true;
        visit(c);
      }
      if (!has_return) error(node, "Return statement not found");
      break;
    }

    default: {
      for (auto& c : node->Children()) visit(c);
    }
  }
  return node;
}

auto SemanticAnalyzer::lookup_ident(const std::string& name) -> SymbolPtr {
  for (int i = scopes_ - 1; i >= 0; --i) {
    auto sym = symbol_table_->SearchSemanticSymbol(scopes_, name);
    if (sym != nullptr) return sym;
  }
  return nullptr;
}

auto SemanticAnalyzer::visit_expr(const ASTNodePtr& expr) -> ExpType {
  if (expr == nullptr) return ExpType::kINT;

  switch (expr->Type()) {
    case ASTNodeType::kVALUE: {
      int val;
      if (!SafeParseInt(expr->Operation()->Value(), val)) {
        error(expr, "Integer Overflow");
      }
      return ExpType::kINT;
    }

    case ASTNodeType::kIDENT: {
      auto var = expr->Operation()->Value();
      auto sym = lookup_ident(var);
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
  SPDLOG_ERROR("Semantic Error: {}", msg);
  succ_ = false;
}

auto SemanticAnalyzer::declare_ident(const std::string& name,
                                     const std::string& type) -> bool {
  auto sym = symbol_table_->SearchSemanticSymbol(scopes_, name);
  if (sym != nullptr) return false;

  const auto in_var = "inv_" + std::to_string(scopes_) + "_" +
                      std::to_string(inner_var_index_++);

  sym = symbol_table_->AddSemanticSymbol(scopes_, name, in_var);
  sym->SetMetaData("type", type);
  return true;
}

auto SemanticAnalyzer::Analyze(const AST& ast) -> bool {
  succ_ = true;

  enter_scope();
  visit(ast.Root());
  leave_scope();

  return succ_;
}

void SemanticAnalyzer::enter_scope() {
  s_in_var_idx_.push(inner_var_index_);
  inner_var_index_ = 0;
  scopes_++;
}

void SemanticAnalyzer::leave_scope() {
  inner_var_index_ = s_in_var_idx_.top();
  s_in_var_idx_.pop();
  scopes_--;
}
