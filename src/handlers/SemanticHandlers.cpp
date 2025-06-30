#include "SemanticHandlers.h"

#include "model/SymbolDefs.h"
#include "model/SymbolMetaTypedef.h"

namespace {

auto SafeParseInt(const std::string& text, int& result) -> bool {
  if (text.empty()) return false;

  // assert: text is not negative

  size_t pos = 0;

  int base = 10;
  if (pos + 2 <= text.size() && text[pos] == '0' &&
      (text[pos + 1] == 'x' || text[pos + 1] == 'X')) {
    base = 16;
    pos += 2;
  } else if (pos + 1 < text.size() && text[pos] == '0') {
    base = 8;
    pos += 1;
  }

  std::string digits = text.substr(pos);
  if (digits.empty()) return false;

  size_t idx = 0;
  unsigned long long u = 0;
  try {
    u = std::stoull(digits, &idx, base);
  } catch (...) {
    return false;
  }
  if (idx != digits.size()) return false;

  if (base == 16) {
    if (u > 0xFFFFFFFFULL) return false;
    auto v32 = static_cast<uint32_t>(u);
    int32_t sv = *reinterpret_cast<int32_t*>(&v32);
    result = sv;
    return true;
  }

  if (base == 10) {
    if (u > 0x80000000ULL) return false;  // Allow up to 2^31 = 2147483648
    result = static_cast<int>(u);  // 2^31 == -2^31 when interpreted as int
    return true;
  }

  // Octal or others, fit into int range
  if (u > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
    return false;
  }
  result = static_cast<int>(u);
  return true;
}

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

void SetReturnType(SemanticAnalyzer* sa, const SymbolPtr& sym,
                   const std::string& type_name, const ASTNodePtr& node) {
  TypeName2SymbolMetaType(sa, sym, type_name, node);
}

auto SMDeclareHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMAssignHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMReturnHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                     const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();

  if (children.empty()) return node;
  auto& value = children.back();
  router(value);

  auto exp_type =
      MetaGet(value->Symbol(), SymbolMetaKey::kTYPE, SymbolMetaType::kNONE);

  auto rtn_type = MetaGet(node->Symbol(), SymbolMetaKey::kRETURN_TYPE,
                          SymbolMetaType::kINT);

  if (exp_type != rtn_type) {
    sa->Error(node, "The return type mismatch");
  }

  sa->SetMeta("has_return", true);
  MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);

  for (auto& sym : sa->VisibleDefineSymbols(node->Symbol()->Scope())) {
    sym->SetMeta(SymbolMetaKey::kHAS_INIT, true);
  }
  return node;
}

auto SMMeaninglessHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                          const ASTNodePtr& node) -> ASTNodePtr {
  for (auto& c : node->Children()) {
    auto child = router(c);

    const auto will_return =
        MetaGet<bool>(child->Symbol(), SymbolMetaKey::kWILL_RETURN, false);
    if (will_return) {
      MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
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

auto SMValueHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMIdentHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                    const ASTNodePtr& node) -> ASTNodePtr {
  auto sym = node->Symbol();

  auto def_sym = sa->LookupSymbol(sym);
  if (!def_sym) {
    sa->Error(node, "Undeclared variables: " + sym->Name());
    return node;
  }

  // sync type info from def to ast
  MetaSet(sym, SymbolMetaKey::kTYPE, def_sym->MetaData(SymbolMetaKey::kTYPE));

  if (!def_sym->MetaData(SymbolMetaKey::kHAS_INIT).has_value()) {
    sa->Error(node, "Variable not initialized: " + def_sym->Name());
  }

  return node;
}

auto SMBinOpHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMUnOpHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMIfHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMCondExpHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
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

auto SMWhileHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                    const ASTNodePtr& node) -> ASTNodePtr {
  // children: [condition, body]
  auto children = node->Children();
  if (children.empty()) return node;

  MetaSet(node->Symbol(), SymbolMetaKey::kIN_LOOP, true);

  // while
  if (children.size() < 3) {
    // record init state of variables
    auto visible = sa->VisibleDefineSymbols(node->Symbol()->Scope());
    std::vector<bool> was_init;
    was_init.reserve(visible.size());
    for (auto& sym : visible) {
      was_init.push_back(sym->MetaData(SymbolMetaKey::kHAS_INIT).has_value());
    }

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

    // unset init state of variables
    for (size_t i = 0; i < visible.size(); ++i) {
      if (!was_init[i]) {
        visible[i]->RemoveMeta(SymbolMetaKey::kHAS_INIT);
      }
    }

    return node;
  }

  ASTNodePtr init_node = nullptr;
  ASTNodePtr cond_node = nullptr;
  ASTNodePtr step_node = nullptr;
  ASTNodePtr body_node = nullptr;

  for (const auto& child : children) {
    switch (child->Tag()) {
      case ASTNodeTag::kINIT:
        init_node = child;
        break;
      case ASTNodeTag::kCOND:
        cond_node = child;
        break;
      case ASTNodeTag::kSTEP:
        step_node = child;
        break;
      default:
        body_node = child;
        break;
    }
  }

  if (init_node) {
    auto init_exp = router(init_node);
    assert(init_exp != nullptr);
  }

  if (cond_node) {
    auto cond_exp = router(cond_node);
    assert(cond_exp != nullptr);
    auto sym = cond_exp->Symbol();
    auto sym_type = MetaGet<SymbolMetaType>(sym, SymbolMetaKey::kTYPE);
    if (!sym || sym_type != SymbolMetaType::kBOOL) {
      sa->Error(node, "The condition of for-statement must be boolean type.");
    }
  }

  if (body_node) router(body_node);

  if (step_node) {
    auto step_exp = router(step_node);
    assert(step_exp != nullptr);
    if (step_exp->Type() == ASTNodeType::kDECLARE) {
      sa->Error(node, "No declaration in step statement.");
    }
  }

  return node;
}

auto SMContinueBreakHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                            const ASTNodePtr& node) -> ASTNodePtr {
  if (!MetaGet(node->Symbol(), SymbolMetaKey::kIN_LOOP, false)) {
    sa->Error(node, "Continue or Break must not be placed outside a loop.");
  }

  if (node->Type() == ASTNodeType::kBREAK) {
    MetaSet(node->Symbol(), SymbolMetaKey::kWILL_BREAK, true);
  }

  return node;
}

auto SMFunctionHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                       const ASTNodePtr& node) -> ASTNodePtr {
  auto sym = node->Symbol();
  auto return_type = sym->Value();

  MetaSet(node->Symbol(), SymbolMetaKey::kRETURN_TYPE,
          return_type == "bool" ? SymbolMetaType::kBOOL : SymbolMetaType::kINT);

  auto children = node->Children();

  const auto& body = children.back();
  if (body->Tag() == ASTNodeTag::kBODY) router(body);

  const auto will_return =
      MetaGet<bool>(body->Symbol(), SymbolMetaKey::kWILL_RETURN, false);
  if (!will_return) {
    sa->Error(node, "Return statement not found");
  }

  return node;
}

auto SMProgramHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                      const ASTNodePtr& node) -> ASTNodePtr {
  const auto root_scope_id = node->Symbol()->ScopeId();
  // predefined functions
  auto [succ_0, def_p_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_print", "__func_print"));
  SetReturnType(sa, def_p_sym, "int", node);
  MetaSet<std::vector<SymbolMetaType>>(def_p_sym, SymbolMetaKey::kPARAM_TYPES,
                                       {SymbolMetaType::kINT});

  auto [succ_1, def_r_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_read", "__func_read"));
  SetReturnType(sa, def_r_sym, "int", node);
  MetaSet<std::vector<SymbolMetaType>>(def_r_sym, SymbolMetaKey::kPARAM_TYPES,
                                       {});

  auto [succ_2, def_f_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_flush", "__func_flush"));
  SetReturnType(sa, def_f_sym, "int", node);
  MetaSet<std::vector<SymbolMetaType>>(def_f_sym, SymbolMetaKey::kPARAM_TYPES,
                                       {});

  for (auto& fn : node->Children()) {
    auto sym = fn->Symbol();
    auto [succ, def_sym] = sa->RecordRealSymbol(
        sa->MapFunction(root_scope_id, sym->Name(), sym->Value()));
    // auto [succ, def_sym] = sa->RecordRealSymbol(sym);
    if (!succ) {
      sa->Error(fn, "Redefine function: " + sym->Name());
    }
    auto ret_type = sym->Value();
    SetReturnType(sa, def_sym, ret_type, node);

    std::vector<SymbolMetaType> param_types;
    for (auto& child : fn->Children()) {
      if (child->Tag() != ASTNodeTag::kPARAMS) continue;

      for (auto& param_node : child->Children()) {
        auto p_sym = param_node->Symbol();
        auto [pok, pdef] = sa->RecordSymbol(p_sym);
        if (!pok) {
          sa->Error(param_node, "Redefine parameter: " + p_sym->Name());
        }

        SetReturnType(sa, pdef, param_node->Symbol()->Value(), param_node);
        pdef->SetMeta(SymbolMetaKey::kHAS_INIT, true);

        auto p_type = MetaGet<SymbolMetaType>(pdef, SymbolMetaKey::kTYPE);
        param_types.push_back(p_type);
      }
    }

    MetaSet(def_sym, SymbolMetaKey::kPARAM_TYPES, param_types);
  }

  for (auto& fn : node->Children()) {
    router(fn);
  }

  auto globals = sa->VisibleDefineSymbols(node->Symbol()->Scope());
  bool found_main = false;
  for (auto& g : globals) {
    if (g->Name() == "__func_main") {
      found_main = true;
      // check main function
      auto t = MetaGet<SymbolMetaType>(g, SymbolMetaKey::kTYPE);
      if (t != SymbolMetaType::kINT) {
        sa->Error(node, "main must return int");
      }
      auto param_types = MetaGet<std::vector<SymbolMetaType>>(
          g, SymbolMetaKey::kPARAM_TYPES, {});

      if (!param_types.empty()) {
        sa->Error(node, "main should not have any arg");
      }

      break;
    }
  }

  // check if main function is defined
  if (!found_main) {
    sa->Error(node, "undefined reference to main");
  }
  return node;
}

auto SMCallHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                   const ASTNodePtr& node) -> ASTNodePtr {
  auto sym = node->Symbol();

  auto def_sym = sa->LookupSymbol(sym);
  if (!def_sym) {
    sa->Error(node, "Undeclared function: " + sym->Name());
    return node;
  }

  std::vector<SymbolMetaType> arg_types;
  auto children = node->Children();

  if (!children.empty()) {
    for (auto& arg : children.front()->Children()) {
      router(arg);
      arg_types.push_back(
          MetaGet<SymbolMetaType>(arg->Symbol(), SymbolMetaKey::kTYPE));
    }
  }

  auto param_types = MetaGet<std::vector<SymbolMetaType>>(
      def_sym, SymbolMetaKey::kPARAM_TYPES, {});

  if (arg_types.size() != param_types.size()) {
    sa->Error(node, "Function '" + sym->Name() + "' expects " +
                        std::to_string(param_types.size()) +
                        " argument(s), "
                        "but " +
                        std::to_string(arg_types.size()) + " provided.");
  }

  size_t n = std::min(arg_types.size(), param_types.size());
  for (size_t i = 0; i < n; ++i) {
    if (arg_types[i] != param_types[i]) {
      sa->Error(node, "Type mismatch for argument " + std::to_string(i + 1) +
                          " of function '" + sym->Name() + "'.");
    }
  }

  auto ret_type = MetaGet<SymbolMetaType>(def_sym, SymbolMetaKey::kTYPE);
  MetaSet(sym, SymbolMetaKey::kTYPE, ret_type);

  return node;
}

const SMHandlerMapping kSMHandlerMapping = {
    {ASTNodeType::kASSIGN, SMAssignHandler},
    {ASTNodeType::kDECLARE, SMDeclareHandler},
    {ASTNodeType::kRETURN, SMReturnHandler},
    {ASTNodeType::kUN_OP, SMUnOpHandler},
    {ASTNodeType::kBIN_OP, SMBinOpHandler},
    {ASTNodeType::kVALUE, SMValueHandler},
    {ASTNodeType::kFUNCTION, SMFunctionHandler},
    {ASTNodeType::kPROGRAM, SMProgramHandler},
    {ASTNodeType::kIDENT, SMIdentHandler},
    {ASTNodeType::kBLOCK, SMMeaninglessHandler},
    {ASTNodeType::kIF, SMIfHandler},
    {ASTNodeType::kWHILE, SMWhileHandler},
    {ASTNodeType::kCOND_EXP, SMCondExpHandler},
    {ASTNodeType::kCONTINUE, SMContinueBreakHandler},
    {ASTNodeType::kBREAK, SMContinueBreakHandler},
    {ASTNodeType::kCALL, SMCallHandler},
};

}  // namespace

auto GetSMHandlersMap() -> SMHandlerMapping { return kSMHandlerMapping; }