#include "SemanticAnalyzer.h"

#include <algorithm>

#include "SymbolMetaTypedef.h"
#include "Utils.h"

namespace {

auto TypeName2SymbolMetaType(SemanticAnalyzer* sa, const SymbolPtr& sym,
                             const std::string& type_name,
                             const ASTNodePtr& node) {
  if (type_name == "int") {
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kINT);
  } else if (type_name == "bool") {
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kBOOL);
  } else {
    sa->Error(node, "Unknown type of declaration: " + type_name);
  }
}

auto DeclareHandler(SemanticAnalyzer* sa,
                    const SemanticAnalyzer::NodeRouter& router,
                    const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();

  // record symbol and alloc inner variable name
  auto [succ, def_sym] = sa->RecordSymbol(symbol);
  if (!succ) {
    sa->Error(node, "Redefine identity: " + symbol->Name());
  }

  TypeName2SymbolMetaType(sa, def_sym, symbol->Value(), node);

  auto children = node->Children();
  if (children.empty()) return node;

  const auto& assign = children.front();
  router(assign);

  // sync type info from def to ast
  MetaSet(symbol, SymbolMetaKey::kTYPE,
          def_sym->MetaData(SymbolMetaKey::kTYPE));
  return node;
}

auto AssignHandler(SemanticAnalyzer* sa,
                   const SemanticAnalyzer::NodeRouter& router,
                   const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();
  if (children.empty()) return node;

  auto& target = children.front();
  auto& value = children.back();

  auto var = target->Symbol();

  auto sym = sa->LookupSymbol(var);
  if (!sym) {
    sa->Error(node, "Undeclared variables: " + var->Name());
    return node;
  }

  auto node_value = router(value);
  if (MetaGet<bool>(node_value->Symbol(), SymbolMetaKey::kWILL_RETURN, false)) {
    MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
  }

  // strange, why shouldn't we analyse type?
  if (sa->Meta("only_redefine_analyse").has_value()) return node;

  // check type
  auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
  auto exp_type =
      MetaGet<SymbolMetaType>(value->Symbol(), SymbolMetaKey::kTYPE);

  if (sym_type != exp_type) {
    sa->Error(node, "Assignment type conflicts: " + sym->Name());
  }

  sym->SetMeta(SymbolMetaKey::kHAS_INIT, true);
  return node;
}

auto ReturnHandler(SemanticAnalyzer* sa,
                   const SemanticAnalyzer::NodeRouter& router,
                   const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();

  if (children.empty()) return node;
  auto& value = children.back();
  router(value);

  auto exp_type =
      MetaGet(value->Symbol(), SymbolMetaKey::kTYPE, SymbolMetaType::kNONE);

  if (exp_type != SymbolMetaType::kINT) {
    sa->Error(node, "The return type must be int");
  }

  sa->SetMeta("has_return", true);
  MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
  return node;
}

auto MeaninglessHandler(SemanticAnalyzer* sa,
                        const SemanticAnalyzer::NodeRouter& router,
                        const ASTNodePtr& node) -> ASTNodePtr {
  for (auto& c : node->Children()) {
    auto child = router(c);

    const auto will_return =
        MetaGet<bool>(child->Symbol(), SymbolMetaKey::kWILL_RETURN, false);
    if (will_return) {
      MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
      sa->SetMeta("only_redefine_analyse", true);
    }

    const auto will_break =
        MetaGet<bool>(child->Symbol(), SymbolMetaKey::kWILL_BREAK, false);
    if (will_break) {
      MetaSet(node->Symbol(), SymbolMetaKey::kWILL_BREAK, true);
      // should stop here now
      break;
    }
  }
  return node;
}

auto ValueHandler(SemanticAnalyzer* sa,
                  const SemanticAnalyzer::NodeRouter& router,
                  const ASTNodePtr& node) -> ASTNodePtr {
  int val;
  auto symbol = node->Symbol();

  TypeName2SymbolMetaType(sa, symbol, symbol->Value(), node);

  if (MetaGet<SymbolMetaType>(symbol, SymbolMetaKey::kTYPE) ==
      SymbolMetaType::kINT) {
    if (!SafeParseInt(symbol->Name(), val)) {
      sa->Error(node, "Integer Overflow");
    }
  }

  return node;
}

auto IdentHandler(SemanticAnalyzer* sa,
                  const SemanticAnalyzer::NodeRouter& router,
                  const ASTNodePtr& node) -> ASTNodePtr {
  auto sym = node->Symbol();

  auto def_sym = sa->LookupSymbol(sym);
  if (!def_sym) {
    sa->Error(node, "Undeclared variables: " + sym->Name());
    return node;
  }

  // sync type info from def to ast
  MetaSet(sym, SymbolMetaKey::kTYPE, def_sym->MetaData(SymbolMetaKey::kTYPE));

  // strange, why shouldn't we analyse further?
  if (!sa->Meta("only_redefine_analyse").has_value()) {
    if (!def_sym->MetaData(SymbolMetaKey::kHAS_INIT).has_value()) {
      sa->Error(node, "Variable not initialized: " + def_sym->Name());
      return node;
    }
  }

  return node;
}

auto BinOpHandler(SemanticAnalyzer* sa,
                  const SemanticAnalyzer::NodeRouter& router,
                  const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();

  auto lhs = router(children.front());
  auto rhs = router(children.back());

  assert(lhs != nullptr);
  assert(rhs != nullptr);

  auto sym_lhs = lhs->Symbol();
  auto sym_rhs = rhs->Symbol();

  // get op type
  auto sym = node->Symbol();
  auto op = sym->Name();

  auto sym_lhs_type = MetaGet<SymbolMetaType>(sym_lhs, SymbolMetaKey::kTYPE);
  auto sym_rhs_type = MetaGet<SymbolMetaType>(sym_rhs, SymbolMetaKey::kTYPE);

  if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
      op == "&" || op == "^" || op == "|" || op == "<<" || op == ">>") {
    if (sym_lhs_type != SymbolMetaType::kINT ||
        sym_rhs_type != SymbolMetaType::kINT) {
      sa->Error(node, "Operands must be integers for binary arithmetic op");
      return node;
    }

    // set as an int expr
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kINT);
    return node;
  }

  if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" ||
      op == ">=") {
    if (sym_lhs_type != sym_rhs_type) {
      sa->Error(node, "Operands must be comparable");
      return node;
    }

    // set as an bool expr
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kBOOL);
    return node;
  }

  if (op == "&&" || op == "||") {
    if (sym_lhs_type != SymbolMetaType::kBOOL ||
        sym_rhs_type != SymbolMetaType::kBOOL) {
      sa->Error(node, "Operands must be boolean for logical op");
      return node;
    }

    // set as an bool expr
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kBOOL);
    return node;
  }

  sa->Error(node, "Unknown binary operator: " + op);
  return node;
}

auto UnOpHandler(SemanticAnalyzer* sa,
                 const SemanticAnalyzer::NodeRouter& router,
                 const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();
  if (children.empty()) return node;

  auto sym = node->Symbol();
  auto op = sym->Name();

  auto val = router(children.front());
  assert(val != nullptr);

  auto sym_val = val->Symbol();
  auto sym_val_type = MetaGet<SymbolMetaType>(sym_val, SymbolMetaKey::kTYPE);

  if (op == "-" || op == "~") {
    if (sym_val_type != SymbolMetaType::kINT) {
      sa->Error(node, "Unary minus requires integer operand");
      return node;
    }
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kINT);
    return node;
  }

  if (op == "!") {
    if (sym_val_type != SymbolMetaType::kBOOL) {
      sa->Error(node, "Logical NOT requires boolean operand");
      return node;
    }
    MetaSet(sym, SymbolMetaKey::kTYPE, SymbolMetaType::kBOOL);
    return node;
  }

  sa->Error(node, "Unknown unary operator: " + op);
  return node;
}

auto IfHandler(SemanticAnalyzer* sa, const SemanticAnalyzer::NodeRouter& router,
               const ASTNodePtr& node) -> ASTNodePtr {
  // children: [condition, then, (else)?]
  auto children = node->Children();
  if (children.empty()) return node;

  // type check
  auto exp = router(children.front());
  auto sym = exp->Symbol();
  auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
  if (sym && sym_type != SymbolMetaType::kBOOL) {
    sa->Error(node, "The condition of if-statement must be boolean type.");
  }

  // handle
  auto node_then = router(children[1]);

  bool will_return = true;
  will_return =
      will_return &&
      MetaGet<bool>(node_then->Symbol(), SymbolMetaKey::kWILL_RETURN, false);

  bool will_break = true;
  will_break = will_break && MetaGet<bool>(node_then->Symbol(),
                                           SymbolMetaKey::kWILL_BREAK, false);

  // no else
  if (children.size() < 3) {
    will_return = false;
    will_break = false;
  }

  // handle optional "else"
  if (children.size() > 2) {
    auto node_else = router(children.back());
    will_return =
        will_return &&
        MetaGet<bool>(node_else->Symbol(), SymbolMetaKey::kWILL_RETURN, false);
  }

  if (will_return) MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
  if (will_break) MetaSet(node->Symbol(), SymbolMetaKey::kWILL_BREAK, true);
  return node;
}

auto CondExpHandler(SemanticAnalyzer* sa,
                    const SemanticAnalyzer::NodeRouter& router,
                    const ASTNodePtr& node) -> ASTNodePtr {
  // children: [condition, then, else]
  auto children = node->Children();
  if (children.empty()) return node;

  // type check
  auto exp = router(children.front());
  auto sym = exp->Symbol();
  auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
  if (sym && sym_type != SymbolMetaType::kBOOL) {
    sa->Error(node, "The condition of if-statement must be boolean type.");
  }

  // then value
  auto lhs = router(children[1]);

  // else value
  auto rhs = router(children.back());

  auto sym_lhs = lhs->Symbol();
  auto sym_rhs = rhs->Symbol();

  auto will_return = true;

  will_return =
      will_return && MetaGet<bool>(sym_lhs, SymbolMetaKey::kWILL_RETURN, false);
  will_return =
      will_return && MetaGet<bool>(sym_rhs, SymbolMetaKey::kWILL_RETURN, false);

  if (will_return) MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);

  auto sym_lhs_type = MetaGet<SymbolMetaType>(sym_lhs, SymbolMetaKey::kTYPE);
  auto sym_rhs_type = MetaGet<SymbolMetaType>(sym_rhs, SymbolMetaKey::kTYPE);

  if (sym_lhs_type != sym_rhs_type) {
    sa->Error(
        node,
        "The two sub-exp of condition expression must be as the same type.");
  }

  MetaSet(node->Symbol(), SymbolMetaKey::kTYPE, sym_lhs_type);
  return node;
}

auto WhileHandler(SemanticAnalyzer* sa,
                  const SemanticAnalyzer::NodeRouter& router,
                  const ASTNodePtr& node) -> ASTNodePtr {
  // children: [condition, body]
  auto children = node->Children();
  if (children.empty()) return node;

  MetaSet(node->Symbol(), SymbolMetaKey::kIN_LOOP, true);

  // while
  if (children.size() < 3) {
    auto exp = router(children.front());
    assert(exp != nullptr);

    // check exp type
    auto sym = exp->Symbol();
    auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
    if (sym && sym_type != SymbolMetaType::kBOOL) {
      sa->Error(node, "The condition of if-statement must be boolean type.");
    }

    // handle body
    router(children.back());

    return node;
  }

  // for(declare; exp; simple_optional)

  for (const auto& child : children) {
    if (child->Tag() == ASTNodeTag::kCOND) {
      auto exp = router(child);
      assert(exp != nullptr);

      // check exp type
      auto sym = exp->Symbol();
      auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
      if (sym && sym_type != SymbolMetaType::kBOOL) {
        sa->Error(node, "The condition of for-statement must be boolean type.");
      }
      continue;
    }

    if (child->Tag() == ASTNodeTag::kSTEP) {
      auto exp = router(child);
      assert(exp != nullptr);

      if (exp->Type() == ASTNodeType::kDECLARE) {
        sa->Error(node, "No declaration in step statement.");
      }
      continue;
    }

    router(child);
  }

  return node;
}

auto ContinueBreakHandler(SemanticAnalyzer* sa,
                          const SemanticAnalyzer::NodeRouter& router,
                          const ASTNodePtr& node) -> ASTNodePtr {
  if (!MetaGet(node->Symbol(), SymbolMetaKey::kIN_LOOP, false)) {
    sa->Error(node, "Continue or Break must not be placed outside a loop.");
  }

  if (node->Type() == ASTNodeType::kBREAK) {
    MetaSet(node->Symbol(), SymbolMetaKey::kWILL_BREAK, true);
  }

  return node;
}

}  // namespace

std::map<ASTNodeType, SemanticAnalyzer::NodeHandler>
    SemanticAnalyzer::node_handler_register = {
        {ASTNodeType::kASSIGN, AssignHandler},
        {ASTNodeType::kDECLARE, DeclareHandler},
        {ASTNodeType::kRETURN, ReturnHandler},
        {ASTNodeType::kUN_OP, UnOpHandler},
        {ASTNodeType::kBIN_OP, BinOpHandler},
        {ASTNodeType::kVALUE, ValueHandler},
        {ASTNodeType::kPROGRAM, MeaninglessHandler},
        {ASTNodeType::kIDENT, IdentHandler},
        {ASTNodeType::kBLOCK, MeaninglessHandler},
        {ASTNodeType::kIF, IfHandler},
        {ASTNodeType::kWHILE, WhileHandler},
        {ASTNodeType::kCOND_EXP, CondExpHandler},
        {ASTNodeType::kCONTINUE, ContinueBreakHandler},
        {ASTNodeType::kBREAK, ContinueBreakHandler},
};

SemanticAnalyzer::SemanticAnalyzer(SymbolTablePtr symbol_table)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kAST, symbol_table_),
      def_sym_helper_(SymbolType::kDEFINE, symbol_table_) {};

auto SemanticAnalyzer::visit(const ASTNodePtr& node) -> ASTNodePtr {
  if (node == nullptr) return node;
  if (node_handler_register.count(node->Type()) == 0) return {};

  if (node && (node->Parent() != nullptr)) {
    node->Symbol()->Inheritance(node->Parent()->Symbol());
  }

  return node_handler_register[node->Type()](
      this, [=](const ASTNodePtr& n) -> ASTNodePtr { return this->visit(n); },
      node);
}

auto SemanticAnalyzer::LookupSymbol(const SymbolPtr& symbol) -> SymbolPtr {
  return def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());
}

auto SemanticAnalyzer::is_integer_literal(const std::string& s) -> bool {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void SemanticAnalyzer::Error(const ASTNodePtr& /*node*/,
                             const std::string& msg) {
  SPDLOG_ERROR("Semantic Error: {}", msg);
  succ_ = false;
}

auto SemanticAnalyzer::RecordSymbol(const SymbolPtr& symbol)
    -> std::tuple<bool, SymbolPtr> {
  // we are not allow variable-shadowing
  auto sym = def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());

  // should not check twice
  if (sym != nullptr) return {false, sym};

  const auto in_var = "ins_" + std::to_string(symbol->ScopeId()) + "_" +
                      std::to_string(inner_var_index_++);

  sym = symbol_table_->AddSymbol(SymbolType::kDEFINE, symbol->Name(), in_var,
                                 true, symbol->ScopeId());
  return {true, sym};
}

auto SemanticAnalyzer::Analyze(const AST& ast) -> bool {
  succ_ = true;

  auto root = ast.Root();
  if (!root) {
    Error(nullptr, "The AST is empty");
  }

  visit(root);

  if (!MetaGet<bool>(root->Symbol(), SymbolMetaKey::kWILL_RETURN, false)) {
    Error(ast.Root(), "Return statement not found");
  }

  if (root && meta_data_.count("has_return") == 0) {
    Error(ast.Root(), "Return statement not found");
  }

  return succ_;
}

void SemanticAnalyzer::SetMeta(const std::string& key, std::any value) {
  meta_data_[key] = std::move(value);
}

auto SemanticAnalyzer::Meta(const std::string& key) -> std::any& {
  return meta_data_[key];
}

void SemanticAnalyzer::PrintSymbolTable(const std::string& path) {
  if (symbol_table_ == nullptr) return;

  std::ofstream f(path);
  symbol_table_->PrintByType(SymbolType::kDEFINE, f);
  f.close();
}
