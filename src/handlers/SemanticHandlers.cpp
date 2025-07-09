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

inline auto EndsWith(std::string const& value, std::string const& ending)
    -> bool {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

auto Trim(const std::string& s) -> std::string {
  size_t first = 0;
  while (first < s.size() &&
         (std::isspace(static_cast<unsigned char>(s[first])) != 0)) {
    ++first;
  }
  size_t last = s.size();
  while (last > first &&
         (std::isspace(static_cast<unsigned char>(s[last - 1])) != 0)) {
    --last;
  }
  return s.substr(first, last - first);
}

auto ParseType(SemanticAnalyzer* sa, const std::string& type_name,
               SymbolPtr& sym, const ASTNodePtr& node) -> std::string {
  spdlog::debug("ParseType: {}", type_name);

  if (type_name == "int" || type_name == "bool" || type_name == "type" ||
      type_name == "nullptr") {
    sym = sa->MapType(type_name, {});
    return type_name;
  }

  // array []
  if (EndsWith(type_name, "[]")) {
    auto base_type = Trim(type_name.substr(0, type_name.size() - 2));
    SymbolPtr sym_base = nullptr;
    auto base_type_name = ParseType(sa, base_type, sym_base, node);

    if (base_type_name.empty() || sym_base == nullptr) {
      sa->Error(nullptr, "Unknown base type for array: " + base_type);
      return "";
    }

    sym = sa->MapType("array_" + base_type_name, "8");
    sym->SetMeta(SymbolMetaKey::kBASE_TYPE, sym_base);
    return "array_" + base_type_name;
  }

  if (type_name.back() == '*') {
    auto base_type = Trim(type_name.substr(0, type_name.size() - 1));
    SymbolPtr sym_base = nullptr;
    auto base_type_name = ParseType(sa, base_type, sym_base, node);

    if (base_type_name.empty() || sym_base == nullptr) {
      sa->Error(nullptr, "Unknown base type for pointer: " + base_type);
      return "";
    }

    sym = sa->MapType("ptr_" + base_type_name, "8");
    sym->SetMeta(SymbolMetaKey::kBASE_TYPE, sym_base);
    return "ptr_" + base_type_name;
  }

  if (type_name.rfind("struct", 0) == 0) {
    // struct type
    auto struct_name = Trim(type_name.substr(6));
    if (struct_name.empty()) {
      sa->Error(nullptr, "Invalid struct type name: " + type_name);
      return "";
    }

    auto def_sym = sa->LookupSymbol(
        sa->MapSymbol(sa->GetRootScopeId(), "struct_" + struct_name, {}));
    if (!def_sym) {
      sa->Error(node, "Undeclared struct: " + sym->Name());
      return "";
    }

    sym = sa->MapType("struct_" + struct_name, def_sym->Value());
    return "struct_" + struct_name;
  }

  sa->Error(node, "Unknown type: " + type_name);
  return "";  // Unknown type
}

auto TypeName2Symbol(SemanticAnalyzer* sa, const std::string& type_name,
                     const ASTNodePtr& node) -> SymbolPtr {
  SymbolPtr type_desc_sym = nullptr;
  auto type = ParseType(sa, type_name, type_desc_sym, node);

  if (type.empty() || type_desc_sym == nullptr) {
    sa->Error(node, "Invalid type name: " + type_name);
    return nullptr;
  }

  if (type_desc_sym->Type() != SymbolType::kTYPEDESC) {
    sa->Error(node, "Invalid type descriptor: " + type_name);
    return nullptr;
  }

  return type_desc_sym;
}

void TypeName2SymbolMetaType(SemanticAnalyzer* sa, const SymbolPtr& sym,
                             const std::string& type_name,
                             const ASTNodePtr& node) {
  auto sym_type = TypeName2Symbol(sa, type_name, node);
  if (!sym_type) {
    sa->Error(node, "Failed to resolve type: " + type_name);
    return;
  }
  sym->SetMeta(SymbolMetaKey::kTYPE, sym_type);
}

void SetReturnType(SemanticAnalyzer* sa, const SymbolPtr& sym,
                   const std::string& type_name, const ASTNodePtr& node) {
  spdlog::debug("SetReturnType: {} with type: {}", sym->Name(), type_name);
  TypeName2SymbolMetaType(sa, sym, type_name, node);
}

auto StructContainsType(const SymbolPtr& curr_type,
                        const SymbolPtr& target_type, SemanticAnalyzer* sa,
                        std::set<std::string>& visited) -> bool {
  if (!curr_type || curr_type->Name().rfind("struct_", 0) != 0) return false;
  if (curr_type == target_type) return true;
  if (visited.count(curr_type->Name()) != 0U) return false;

  visited.insert(curr_type->Name());

  spdlog::debug("Checking struct: {} for target type: {}", curr_type->Name(),
                target_type->Name());

  // get struct symbol
  auto def_sym =
      MetaGet<SymbolPtr>(curr_type, SymbolMetaKey::kSTRUCT_SYMBOL, nullptr);
  if (!def_sym) {
    spdlog::error("Struct symbol not found for: {}", curr_type->Name());
    return false;
  }

  // look up l_value
  auto fields = MetaGet<std::vector<std::pair<std::string, SymbolPtr>>>(
      def_sym, SymbolMetaKey::kSTRUCT_FIELDS, {});
  if (fields.empty()) return false;

  for (const auto& [fname, ftype] : fields) {
    if (!ftype) continue;
    if (ftype == target_type) return true;
    if (ftype->Name().rfind("struct_", 0) == 0) {
      if (StructContainsType(ftype, target_type, sa, visited)) return true;
    }
  }

  return false;
}

/* Handlers */

auto SMDeclareHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                      const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();

  // record symbol and alloc inner variable name
  auto [succ, def_sym] = sa->RecordSymbol(symbol);
  if (!succ) {
    sa->Error(node, "Redefine identity: " + symbol->Name());
  }

  // set type info
  TypeName2SymbolMetaType(sa, def_sym, symbol->Value(), node);

  auto type = MetaGet<SymbolPtr>(def_sym, SymbolMetaKey::kTYPE, nullptr);
  if (!type) {
    sa->Error(node, "Type information missing for: " + symbol->Name());
    return node;
  }

  if (type->Name().rfind("struct_", 0) == 0) {
    sa->Error(node, "Struct type cannot be declared directly: " + type->Name());
    return node;
  }

  // sync type info from def to ast
  MetaSet(symbol, SymbolMetaKey::kTYPE, type);

  spdlog::debug("Declare: {}:{} with type: {}", def_sym->Name(),
                def_sym->Index(), type ? type->Name() : "unknown");

  auto children = node->Children();
  if (children.empty()) return node;

  const auto& assign = children.front();
  router(assign);

  return node;
}

auto SMAssignHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                     const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();
  if (children.empty()) return node;

  const auto& target = children.front();
  const auto& value = children.back();

  auto sym = target->Symbol();

  SymbolPtr def_sym = nullptr;
  if (target->Type() == ASTNodeType::kIDENT) {
    def_sym = sa->LookupSymbol(sym);
    if (!def_sym) {
      sa->Error(node, "Undeclared variable: " + sym->Name());
      return node;
    }
  }

  // parse node
  auto node_target = router(target);
  spdlog::debug("Assign: {}:{} = {}", sym->Name(), sym->Index(),
                value->Symbol()->Name());

  auto node_value = router(value);
  if (MetaGet<bool>(node_value->Symbol(), SymbolMetaKey::kWILL_RETURN, false)) {
    MetaSet(node->Symbol(), SymbolMetaKey::kWILL_RETURN, true);
  }

  // strange, why shouldn't we analyse type?
  if (sa->Meta("only_redefine_analyse").has_value()) return node;

  // check type
  auto sym_type = MetaGet<SymbolPtr>(sym, SymbolMetaKey::kTYPE);
  auto exp_type = MetaGet<SymbolPtr>(value->Symbol(), SymbolMetaKey::kTYPE);

  spdlog::debug("Assign: {}:{} = {}", sym->Name(), sym->Index(),
                sym_type ? sym_type->Name() : "unknown");

  if (!sym_type || !exp_type) {
    sa->Error(node, "Type information missing for: " + sym->Name());
    return node;
  }

  if (sym_type != exp_type) {
    if (sym_type->Name().rfind("ptr_", 0) == 0 &&
        exp_type->Name() == "nullptr") {
      // pointer type, allow assignment of nullptr
      spdlog::debug("Allow pointer assignment of nullptr: {} -> {}",
                    sym_type->Name(), exp_type->Name());
    } else {
      sa->Error(node, "Assignment type conflicts: " + sym->Name() +
                          " (expected: " + sym_type->Name() +
                          ", got: " + exp_type->Name() + ")");
    }
  }

  if (def_sym) {
    // mark as initialized
    def_sym->SetMeta(SymbolMetaKey::kHAS_INIT, true);
    spdlog::debug("Mark {} as initialized", def_sym->Name());
  }
  return node;
}

auto SMReturnHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                     const ASTNodePtr& node) -> ASTNodePtr {
  auto children = node->Children();

  if (children.empty()) return node;
  auto value = router(children.back());
  auto exp_type =
      MetaGet<SymbolPtr>(value->Symbol(), SymbolMetaKey::kTYPE, nullptr);

  auto rtn_type = MetaGet<SymbolPtr>(
      node->Symbol(), SymbolMetaKey::kRETURN_TYPE, sa->LookupType("int"));

  spdlog::debug("Return: {} with type: {}", node->Symbol()->Name(),
                rtn_type ? rtn_type->Name() : "unknown");

  if (exp_type != rtn_type) {
    // nullptr
    if (rtn_type->Name().rfind("ptr_", 0) == 0 && exp_type &&
        exp_type->Name() == "nullptr") {
      // pass
    } else {
      sa->Error(node,
                "The return type mismatch"
                " (expected: " +
                    rtn_type->Name() + ", got: " +
                    (exp_type ? exp_type->Name() : "unknown") + ")");
    }
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
  auto sym_type = MetaGet<SymbolPtr>(symbol, SymbolMetaKey::kTYPE, nullptr);
  if (!sym_type) {
    sa->Error(node, "Unknown type: " + symbol->Value());
    return node;
  }

  if (sym_type == sa->LookupType("int")) {
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

  if (sym->Value() != "declarator" && sym->Value() != "lvalue") {
    if (!def_sym->MetaData(SymbolMetaKey::kHAS_INIT).has_value()) {
      sa->Error(node, "Variable not initialized: " + def_sym->Name() +
                          " (index: " + std::to_string(def_sym->Index()) + ")");
    }
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

  auto sym_lhs_type = MetaGet<SymbolPtr>(sym_lhs, SymbolMetaKey::kTYPE);
  auto sym_rhs_type = MetaGet<SymbolPtr>(sym_rhs, SymbolMetaKey::kTYPE);

  spdlog::debug("Binary Op: {}:{} {} <-> {}", sym->Name(), sym->Index(),
                sym_lhs_type ? sym_lhs_type->Name() : "unknown",
                sym_rhs_type ? sym_rhs_type->Name() : "unknown");

  if (sym_lhs_type == nullptr || sym_rhs_type == nullptr) {
    sa->Error(node, "Type information missing for binary operation");
    return node;
  }

  if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
      op == "&" || op == "^" || op == "|" || op == "<<" || op == ">>") {
    if (sym_lhs_type != sa->LookupType("int") ||
        sym_rhs_type != sa->LookupType("int")) {
      sa->Error(node, "Operands must be integers for binary arithmetic op");
      return node;
    }

    // set as an int expr
    MetaSet(sym, SymbolMetaKey::kTYPE, sa->LookupType("int"));
    return node;
  }

  if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" ||
      op == ">=") {
    if (sym_lhs_type != sym_rhs_type) {
      if (sym_lhs_type->Name().rfind("ptr_", 0) == 0 &&
          sym_rhs_type->Name().rfind("nullptr", 0) == 0) {
        // pointer type, allow comparison with nullptr
      } else if (sym_rhs_type->Name().rfind("ptr_", 0) == 0 &&
                 sym_lhs_type->Name().rfind("nullptr", 0) == 0) {
        // pointer type, allow comparison with nullptr
      } else {
        sa->Error(node, "Operands must be of the same type for comparison");
        return node;
      }
    }

    // set as an bool expr
    MetaSet(sym, SymbolMetaKey::kTYPE, sa->LookupType("bool"));
    return node;
  }

  if (op == "&&" || op == "||") {
    if (sym_lhs_type != sa->LookupType("bool") ||
        sym_rhs_type != sa->LookupType("bool")) {
      sa->Error(node, "Operands must be boolean for logical op");
      return node;
    }

    // set as an bool expr
    MetaSet(sym, SymbolMetaKey::kTYPE, sa->LookupType("bool"));
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
  auto sym_val_type = MetaGet<SymbolPtr>(sym_val, SymbolMetaKey::kTYPE);

  if (!sym_val_type) {
    sa->Error(node, "Type information missing for unary operation");
    return node;
  }

  if (op == "-" || op == "~") {
    if (sym_val_type != sa->LookupType("int")) {
      sa->Error(node, "Unary minus requires integer operand");
      return node;
    }
    MetaSet(sym, SymbolMetaKey::kTYPE, sa->LookupType("int"));
    return node;
  }

  if (op == "!") {
    if (sym_val_type != sa->LookupType("bool")) {
      sa->Error(node, "Logical NOT requires boolean operand");
      return node;
    }
    MetaSet(sym, SymbolMetaKey::kTYPE, sa->LookupType("bool"));
    return node;
  }

  if (op == "*") {
    if (sym_val_type->Name().find("ptr_") != 0) {
      sa->Error(node, "Dereference operator requires pointer operand");
      return node;
    }

    auto base_type =
        MetaGet<SymbolPtr>(sym_val_type, SymbolMetaKey::kBASE_TYPE);
    if (!base_type) {
      sa->Error(node, "Dereference operator requires valid pointer type");
      return node;
    }

    MetaSet(sym, SymbolMetaKey::kTYPE, base_type);
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
  auto sym_type = MetaGet<SymbolPtr>(sym, SymbolMetaKey::kTYPE);
  if (sym && sym_type != sa->LookupType("bool")) {
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
  auto sym_type = MetaGet<SymbolPtr>(sym, SymbolMetaKey::kTYPE);
  if (sym && sym_type != sa->LookupType("bool")) {
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

  auto sym_lhs_type = MetaGet<SymbolPtr>(sym_lhs, SymbolMetaKey::kTYPE);
  auto sym_rhs_type = MetaGet<SymbolPtr>(sym_rhs, SymbolMetaKey::kTYPE);

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
    auto sym_type = MetaGet<SymbolPtr>(sym, SymbolMetaKey::kTYPE);
    if (sym && sym_type != sa->LookupType("bool")) {
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
    auto sym_type = MetaGet<SymbolPtr>(sym, SymbolMetaKey::kTYPE);
    if (!sym || sym_type != sa->LookupType("bool")) {
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
          TypeName2Symbol(sa, return_type, node));

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
  const auto root_scope_id = sa->GetRootScopeId();

  // predefined functions
  auto [succ_0, def_p_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_print", "__func_print"));
  SetReturnType(sa, def_p_sym, "int", node);
  MetaSet<std::vector<SymbolPtr>>(def_p_sym, SymbolMetaKey::kFUNCTION_PARAMS,
                                  {sa->LookupType("int")});

  auto [succ_1, def_r_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_read", "__func_read"));
  SetReturnType(sa, def_r_sym, "int", node);
  MetaSet<std::vector<SymbolPtr>>(def_r_sym, SymbolMetaKey::kFUNCTION_PARAMS,
                                  {});

  auto [succ_2, def_f_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_flush", "__func_flush"));
  SetReturnType(sa, def_f_sym, "int", node);
  MetaSet<std::vector<SymbolPtr>>(def_f_sym, SymbolMetaKey::kFUNCTION_PARAMS,
                                  {});

  auto [succ_3, def_a_sym] = sa->RecordRealSymbol(
      sa->MapFunction(root_scope_id, "__func_alloc", "__func_alloc"));
  SetReturnType(sa, def_a_sym, "type*", node);
  MetaSet<std::vector<SymbolPtr>>(def_a_sym, SymbolMetaKey::kFUNCTION_PARAMS,
                                  {sa->LookupType("type")});

  auto [succ_4, def_aa_sym] = sa->RecordRealSymbol(sa->MapFunction(
      root_scope_id, "__func_alloc_array", "__func_alloc_array"));
  SetReturnType(sa, def_aa_sym, "type[]", node);
  MetaSet<std::vector<SymbolPtr>>(
      def_aa_sym, SymbolMetaKey::kFUNCTION_PARAMS,
      {sa->LookupType("type"), sa->LookupType("int")});

  for (auto& fn : node->Children()) {
    if (fn->Type() != ASTNodeType::kSTRUCT) continue;

    auto symbol = fn->Symbol();
    auto struct_name = symbol->Name();

    // check if struct is already defined
    auto [succ, def_sym] = sa->RecordRealSymbol(
        sa->MapStruct(sa->GetRootScopeId(), struct_name, {}));
    if (!succ) {
      sa->Error(fn, "Redefine struct: " + struct_name);
    }

    spdlog::debug("Struct definition: {}:{} with name: {}", def_sym->Name(),
                  def_sym->Index(), struct_name);

    // get real struct name
    // remove "__struct_"
    auto real_struct_name = struct_name.substr(9);

    // create new struct type
    auto def_type_sym = TypeName2Symbol(sa, "struct " + real_struct_name, node);
    def_sym->SetMeta(SymbolMetaKey::kTYPE, def_type_sym);

    // parse fields
    std::vector<std::pair<std::string, SymbolPtr>> fields;
    for (const auto& field : fn->Children()) {
      if (field->Type() != ASTNodeType::kFIELD) continue;

      auto field_sym = field->Symbol();
      auto field_type = TypeName2Symbol(sa, field_sym->Name(), field);
      if (!field_type) {
        sa->Error(field, "Invalid type for field: " + field_sym->Name());
        continue;
      }

      if (field_type == def_type_sym) {
        sa->Error(field,
                  "Struct cannot contain itself as a field: " + struct_name);
        continue;
      }

      spdlog::debug("Struct field: {}:{} with type: {}", field_sym->Value(),
                    field_sym->Index(),
                    field_type ? field_type->Name() : "unknown");

      fields.emplace_back(field_sym->Value(), field_type);
    }

    MetaSet(def_sym, SymbolMetaKey::kSTRUCT_FIELDS, fields);
    MetaSet(def_type_sym, SymbolMetaKey::kSTRUCT_SYMBOL, def_sym);
  }

  for (auto& fn : node->Children()) {
    auto sym = fn->Symbol();

    if (fn->Type() != ASTNodeType::kFUNCTION) continue;

    auto [succ, def_sym] = sa->RecordRealSymbol(
        sa->MapFunction(root_scope_id, sym->Name(), sym->Value()));
    if (!succ) {
      sa->Error(fn, "Redefine function: " + sym->Name());
    }
    auto ret_type = sym->Value();
    SetReturnType(sa, def_sym, ret_type, node);

    std::vector<SymbolPtr> param_types;
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

        auto p_type = MetaGet<SymbolPtr>(pdef, SymbolMetaKey::kTYPE);

        if (p_type == nullptr) {
          sa->Error(param_node, "Invalid type for parameter: " + p_sym->Name());
          continue;
        }

        // should not be a struct type
        if (p_type->Name().rfind("struct_", 0) == 0) {
          sa->Error(param_node, "Invalid type for parameter: " + p_sym->Name() +
                                    " (struct type is not allowed)");
        }

        spdlog::debug("Function parameter: {}:{} with type: {}", p_sym->Name(),
                      p_sym->Index(), p_type->Name());

        param_types.push_back(p_type);
      }
    }

    MetaSet(def_sym, SymbolMetaKey::kFUNCTION_PARAMS, param_types);
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
      auto t = MetaGet<SymbolPtr>(g, SymbolMetaKey::kTYPE);
      if (t != sa->LookupType("int")) {
        sa->Error(node, "main must return int");
      }

      auto param_types = MetaGet<std::vector<SymbolPtr>>(
          g, SymbolMetaKey::kFUNCTION_PARAMS, {});
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

  std::vector<SymbolPtr> arg_types;
  auto children = node->Children();

  for (auto& arg : children) {
    auto node = router(arg);
    arg_types.push_back(
        MetaGet<SymbolPtr>(node->Symbol(), SymbolMetaKey::kTYPE));
  }

  auto param_types = MetaGet<std::vector<SymbolPtr>>(
      def_sym, SymbolMetaKey::kFUNCTION_PARAMS, {});

  if (arg_types.size() != param_types.size()) {
    sa->Error(node, "Function '" + sym->Name() + "' expects " +
                        std::to_string(param_types.size()) +
                        " argument(s), "
                        "but " +
                        std::to_string(arg_types.size()) + " provided.");
  }

  size_t n = std::min(arg_types.size(), param_types.size());

  for (size_t i = 0; i < n; ++i) {
    const auto& param_type = param_types[i];

    // allow type to be "type" for generic functions
    if (param_type->Name().find("type") != std::string::npos) {
      continue;  // skip type check for generic types
    }

    if (arg_types[i] != param_types[i]) {
      sa->Error(node, "Type mismatch for argument " + std::to_string(i + 1) +
                          " of function '" + sym->Name() + "'.");
    }
  }

  auto ret_type = MetaGet<SymbolPtr>(def_sym, SymbolMetaKey::kTYPE);

  if (def_sym->Name() == "__func_alloc") {
    if (arg_types.size() != 1) {
      sa->Error(node, "Function '__func_alloc' expects one argument: 'type'.");
      return node;
    }

    auto arg_type_name = arg_types.front()->Name();
    spdlog::debug("Allocating type: {}", arg_type_name);

    ret_type = sa->MapType("ptr_" + arg_type_name, "8");
    ret_type->SetMeta(SymbolMetaKey::kBASE_TYPE, arg_types.front());
    if (!ret_type) {
      sa->Error(node, "Failed to resolve type for '__func_alloc': " +
                          arg_type_name + "*");
      return node;
    }
  }

  if (def_sym->Name() == "__func_alloc_array") {
    if (arg_types.size() != 2 || arg_types.back()->Name() != "int") {
      sa->Error(node,
                "Function '__func_alloc_array' expects two arguments: "
                "'type' and 'int'.");
      return node;
    }

    auto arg_type_name = arg_types.front()->Name();
    spdlog::debug("Allocating array of type: {}", arg_type_name);

    ret_type = sa->MapType("array_" + arg_type_name, {});
    if (!ret_type) {
      sa->Error(node, "Failed to resolve type for '__func_alloc_array': " +
                          arg_type_name + "[]");
      return node;
    }
  }

  MetaSet(sym, SymbolMetaKey::kTYPE, ret_type);
  spdlog::debug("Call function: {}:{} with return type: {}", def_sym->Name(),
                def_sym->Index(), ret_type ? ret_type->Name() : "unknown");
  return node;
}

auto SMTypeHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                   const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();
  auto sym_type = TypeName2Symbol(sa, symbol->Name(), node);
  if (!sym_type) {
    sa->Error(node, "Failed to resolve type: " + symbol->Name());
    return node;
  }
  symbol->SetMeta(SymbolMetaKey::kTYPE, sym_type);
  return node;
}

auto SMArrayAccessHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                          const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();

  auto children = node->Children();
  if (children.size() != 2) {
    sa->Error(node,
              "Array access must have exactly one index: " + symbol->Name());
    return node;
  }

  for (const auto& child : children) {
    router(child);
  }

  auto l_value = children.front()->Symbol();
  const auto& l_value_sym = l_value;

  auto l_value_type =
      MetaGet<SymbolPtr>(l_value_sym, SymbolMetaKey::kTYPE, nullptr);
  if (!l_value_type) {
    sa->Error(node, "Left value of array access must have a type: " +
                        l_value->Name());
    return node;
  }

  spdlog::debug("Array access: {}:{} with type: {}", l_value_sym->Name(),
                l_value_sym->Index(),
                l_value_type ? l_value_type->Name() : "unknown");

  const auto& index = children.back();
  auto index_type =
      MetaGet<SymbolPtr>(index->Symbol(), SymbolMetaKey::kTYPE, nullptr);
  if (!index_type || index_type != sa->LookupType("int")) {
    sa->Error(node, "Index of array access must be an integer: " +
                        index->Symbol()->Name());
    return node;
  }

  auto base_type =
      MetaGet<SymbolPtr>(l_value_type, SymbolMetaKey::kBASE_TYPE, nullptr);
  if (!base_type) {
    sa->Error(node, "Array access must have a base type: " + l_value->Name());
    return node;
  }

  symbol->SetMeta(SymbolMetaKey::kTYPE, base_type);
  return node;
}

auto SMStructHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                     const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();
  auto struct_name = symbol->Name();

  // check if struct is already defined
  auto def_sym = sa->LookupSymbol(sa->GetRootScope(), struct_name);
  if (!def_sym) {
    sa->Error(node, "Undeclared struct: " + struct_name);
    return node;
  }

  auto def_type_sym =
      MetaGet<SymbolPtr>(def_sym, SymbolMetaKey::kTYPE, nullptr);
  if (!def_type_sym) {
    sa->Error(node, "Struct definition missing type: " + struct_name);
    return node;
  }

  auto fields = MetaGet<std::vector<std::pair<std::string, SymbolPtr>>>(
      def_sym, SymbolMetaKey::kSTRUCT_FIELDS, {});

  spdlog::debug("Struct definition: {}:{} with name: {}, fields: {}",
                def_sym->Name(), def_sym->Index(), struct_name, fields.size());

  std::set<std::string> field_names;

  for (const auto& field : fields) {
    std::set<std::string> visited;
    if (StructContainsType(field.second, def_type_sym, sa, visited)) {
      sa->Error(node,
                "Struct cannot (directly or indirectly) contain itself: " +
                    struct_name);
      continue;
    }

    if (field_names.count(field.first) > 0) {
      sa->Error(node, "Duplicate field name in struct: " + field.first);
      continue;
    }

    field_names.insert(field.first);
  }

  return node;
}

auto SMFieldAccessHandler(SemanticAnalyzer* sa, const SMNodeRouter& router,
                          const ASTNodePtr& node) -> ASTNodePtr {
  auto symbol = node->Symbol();
  auto children = node->Children();

  if (children.empty()) {
    sa->Error(node,
              "Field access must have exactly one field: " + symbol->Name());
    return node;
  }

  auto l_value = router(children.front());
  auto field_name = symbol->Name();

  auto l_value_type =
      MetaGet<SymbolPtr>(l_value->Symbol(), SymbolMetaKey::kTYPE, nullptr);
  if (!l_value_type) {
    sa->Error(node, "Left value of field access must have a type: " +
                        l_value->Symbol()->Name());
    return node;
  }

  // lvalue type name must start with "struct_"
  auto lvalue_type_name = l_value_type->Name();
  if (lvalue_type_name.rfind("struct", 0) == std::string::npos) {
    sa->Error(node, "Field access must be on a struct type, not: " +
                        lvalue_type_name);
    return node;
  }

  auto struct_type_name = lvalue_type_name.substr(7);  // remove "struct_"

  // look up l_value
  auto def_sym =
      sa->LookupSymbol(sa->GetRootScope(), "__struct_" + struct_type_name);
  if (!def_sym) {
    sa->Error(node, "Undeclared struct: " + l_value->Symbol()->Name());
    return node;
  }

  spdlog::debug("Field access: {}:{} with type: {} and field: {}",
                def_sym->Name(), def_sym->Index(),
                l_value_type ? l_value_type->Name() : "unknown", field_name);

  // get struct fields
  auto fields = MetaGet<std::vector<std::pair<std::string, SymbolPtr>>>(
      def_sym, SymbolMetaKey::kSTRUCT_FIELDS, {});

  // find the field
  for (const auto& [fname, ftype] : fields) {
    spdlog::debug("Checking field: {} with type: {}", fname,
                  ftype ? ftype->Name() : "unknown");
    if (fname == field_name) {
      symbol->SetMeta(SymbolMetaKey::kTYPE, ftype);
      return node;
    }
  }

  sa->Error(node, "Field '" + field_name +
                      "' not found in struct: " + def_sym->Name());
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
    {ASTNodeType::kARG_LIST, SMMeaninglessHandler},
    {ASTNodeType::kTYPE, SMTypeHandler},
    {ASTNodeType::kARRAY_ACCESS, SMArrayAccessHandler},
    {ASTNodeType::kSTRUCT, SMStructHandler},
    {ASTNodeType::kFIELD_ACCESS, SMFieldAccessHandler},
};

}  // namespace

auto GetSMHandlersMap() -> SMHandlerMapping { return kSMHandlerMapping; }