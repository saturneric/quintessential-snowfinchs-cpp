#include "SemanticAnalyzer.h"

#include <algorithm>

#include "Utils.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTablePtr symbol_table)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kAST, symbol_table_),
      def_sym_helper_(SymbolType::kDEFINE, symbol_table_) {};

auto SemanticAnalyzer::visit(const ASTNodePtr& node) -> ASTNodePtr {
  if (!node) {
    error(nullptr, "The AST is empty");
    return nullptr;
  }

  switch (node->Type()) {
    case ASTNodeType::kDECLARE: {
      auto symbol = node->Symbol();

      // record symbol and alloc inner variable name
      if (!record_symbol(symbol)) {
        error(node, "Redefine identity: " + symbol->Name());
      }

      auto children = node->Children();
      if (children.empty()) break;

      const auto& assign = children.front();
      auto expr_type = visit(assign);

      symbol->SetMeta(SymbolMetaKey::kHAS_INIT, true);
      break;
    }

    case ASTNodeType::kASSIGN: {
      auto children = node->Children();
      if (children.empty()) break;

      auto& target = children.front();
      auto& value = children.back();

      auto var = target->Symbol();

      auto sym = lookup_symbol(var);
      if (!sym) {
        error(node, "Undeclared variables: " + var->Name());
        break;
      }

      auto expr_type = visit_expr(value);

      sym->SetMeta(SymbolMetaKey::kHAS_INIT, true);
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

auto SemanticAnalyzer::lookup_symbol(const SymbolPtr& symbol) -> SymbolPtr {
  return def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());
}

auto SemanticAnalyzer::visit_expr(const ASTNodePtr& expr) -> ExpType {
  if (expr == nullptr) return ExpType::kINT;

  switch (expr->Type()) {
    case ASTNodeType::kVALUE: {
      int val;
      if (!SafeParseInt(expr->Symbol()->Name(), val)) {
        error(expr, "Integer Overflow");
      }
      return ExpType::kINT;
    }

    case ASTNodeType::kIDENT: {
      auto var = expr->Symbol();
      auto sym = lookup_symbol(var);
      if (!sym) {
        error(expr, "Undeclared variables: " + var->Name());
        return ExpType::kINT;
      }

      if (!sym->MetaData(SymbolMetaKey::kHAS_INIT).has_value()) {
        error(expr, "Variable not initialized: " + var->Name());
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

auto SemanticAnalyzer::record_symbol(const SymbolPtr& symbol) -> bool {
  auto sym =
      def_sym_helper_.LookupSymbolInScope(symbol->Scope(), symbol->Name());

  // should not check twice
  if (sym != nullptr) return false;

  const auto in_var = "ins_" + std::to_string(symbol->ScopeId()) + "_" +
                      std::to_string(inner_var_index_++);

  sym = symbol_table_->AddSymbol(SymbolType::kDEFINE, symbol->Name(), in_var,
                                 true, symbol->ScopeId());
  return true;
}

auto SemanticAnalyzer::Analyze(const AST& ast) -> bool {
  succ_ = true;

  visit(ast.Root());

  return succ_;
}
