#include "IGHandlers.h"

#include "model/AST.h"
#include "model/Symbol.h"
#include "model/SymbolDefs.h"
#include "model/SymbolMetaTypedef.h"
#include "model/Utils.h"

namespace {

auto DeclareHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                    bool is_lhs) -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());
  assert(sym != nullptr);

  ctx->AddIns("dcl", sym);
  return sym;
}

auto IRValueExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                       bool is_lhs) -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());

  // ident
  if (node->Type() == ASTNodeType::kIDENT) return sym;

  // immediate
  auto type = MetaGet<SymbolPtr>(node->Symbol(), SymbolMetaKey::kTYPE);

  if (type == ctx->LookupType("bool")) {
    if (sym->Name() == "true") {
      return ctx->MapSymbol("1", "immediate");
    }
    return ctx->MapSymbol("0", "immediate");
  }

  return sym;
}

auto IRBinOpExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                       bool is_lhs) -> SymbolPtr {
  auto children = node->Children();

  if (children.size() != 2) return {};

  auto sym_lhs = ctx->ExpRoute(children.front());
  auto lhs = ctx->MapSymbol(sym_lhs);

  auto opera = node->Symbol()->Name();
  auto tmp = ctx->MapSymbol(ctx->NewTempVariable());

  if (opera == "&&") {
    auto end = ctx->NewLabel();

    // tmp = 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("0", "immediate"));

    // if (!lhs) goto false_label
    ctx->AddIns("brz", nullptr, lhs, end);

    auto sym_rhs = ctx->ExpRoute(children.back());
    auto rhs = ctx->MapSymbol(sym_rhs);
    assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

    // if (!rhs) goto false_label
    ctx->AddIns("brz", nullptr, rhs, end);

    // all not 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("1", "immediate"));

    // end:
    ctx->AddIns("label", nullptr, end);

    // avoid next operations
    return tmp;
  }

  if (opera == "||") {
    auto end = ctx->NewLabel();

    // tmp = 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("1", "immediate"));

    // if (!lhs) goto false_label
    ctx->AddIns("brnz", nullptr, lhs, end);

    auto sym_rhs = ctx->ExpRoute(children.back());
    auto rhs = ctx->MapSymbol(sym_rhs);
    assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

    // if (!rhs) goto false_label
    ctx->AddIns("brnz", nullptr, rhs, end);

    // all not 0
    ctx->AddIns("mov", tmp, ctx->MapSymbol("0", "immediate"));

    // end:
    ctx->AddIns("label", nullptr, end);

    // avoid next operations
    return tmp;
  }

  auto sym_rhs = ctx->ExpRoute(children.back());
  auto rhs = ctx->MapSymbol(sym_rhs);
  assert(sym_lhs->ScopeId() == sym_rhs->ScopeId());

  if (opera == "+") {
    ctx->AddIns("add", tmp, lhs, rhs);
  } else if (opera == "-") {
    ctx->AddIns("sub", tmp, lhs, rhs);
  } else if (opera == "*") {
    ctx->AddIns("mul", tmp, lhs, rhs);
  } else if (opera == "/") {
    ctx->AddIns("div", tmp, lhs, rhs);
  } else if (opera == "%") {
    ctx->AddIns("mod", tmp, lhs, rhs);
  }

  else if (opera == "==") {
    ctx->AddIns("eq", tmp, lhs, rhs);
  } else if (opera == "!=") {
    ctx->AddIns("neq", tmp, lhs, rhs);
  } else if (opera == "<") {
    ctx->AddIns("lt", tmp, lhs, rhs);
  } else if (opera == "<=") {
    ctx->AddIns("le", tmp, lhs, rhs);
  } else if (opera == ">") {
    ctx->AddIns("gt", tmp, lhs, rhs);
  } else if (opera == ">=") {
    ctx->AddIns("ge", tmp, lhs, rhs);
  }

  else if (opera == "&") {
    ctx->AddIns("band", tmp, lhs, rhs);
  } else if (opera == "|") {
    ctx->AddIns("bor", tmp, lhs, rhs);
  } else if (opera == "^") {
    ctx->AddIns("bxor", tmp, lhs, rhs);
  }

  else if (opera == "<<") {
    // opt for immediate which is less than 32
    if (IsImmediate(rhs) && std::stoi(rhs->Name()) < 32) {
      ctx->AddIns("sal", tmp, lhs, rhs);
      return tmp;
    }

    auto tmp1 = ctx->NewTempVariable();
    ctx->AddIns("band", tmp1, rhs, ctx->MapSymbol("31", "immediate"));
    ctx->AddIns("sal", tmp, lhs, tmp1);
  } else if (opera == ">>") {
    // opt for immediate which is less than 32
    if (IsImmediate(rhs) && std::stoi(rhs->Name()) < 32) {
      ctx->AddIns("sar", tmp, lhs, rhs);
      return tmp;
    }

    auto tmp1 = ctx->NewTempVariable();
    ctx->AddIns("band", tmp1, rhs, ctx->MapSymbol("31", "immediate"));
    ctx->AddIns("sar", tmp, lhs, tmp1);
  }

  else {
    ctx->AddError("Unsupported binary operator: " + opera);
    return {};
  }

  return tmp;
}

auto IRUnOpExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                      bool is_lhs) -> SymbolPtr {
  auto children = node->Children();

  if (children.empty()) return {};

  auto sym_operand = ctx->ExpRoute(children.front());
  auto sym_tmp = ctx->NewTempVariable();

  auto operand = ctx->MapSymbol(sym_operand);
  auto temp = ctx->MapSymbol(sym_tmp);

  ctx->AddIns("mov", temp, operand);

  auto op = node->Symbol()->Name();
  if (op == "-") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("neg", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "!") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("lnot", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "~") {
    auto sym_tmp1 = ctx->NewTempVariable();
    ctx->AddIns("bnot", sym_tmp1, temp);
    sym_tmp = sym_tmp1;
  }

  else if (op == "*") {
    auto sym_tmp1 = ctx->NewTempVariable();

    if (is_lhs) {
      // pointer dereference
      ctx->AddIns("mov", sym_tmp1, temp);
      sym_tmp1->SetMeta(SymbolMetaKey::kIS_ADDRESS, true);
    } else {
      // dereference operator
      ctx->AddIns("load", sym_tmp1, temp);
    }

    sym_tmp = sym_tmp1;
  }

  else {
    ctx->AddError("Unsupported unary operator: " + op);
    return {};
  }

  return sym_tmp;
}

auto IRMeaninglessNodeHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                              bool is_lhs) -> SymbolPtr {
  for (const auto& child : node->Children()) {
    ctx->ExpRoute(child);
  }
  return node->Symbol();
}

auto IRReturnExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                        bool is_lhs) -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().empty()) return {};

  auto sym_val = ctx->ExpRoute(children.front());

  ctx->AddIns("rtn", {}, sym_val);
  return {};
}

auto IRAssignExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                        bool is_lhs) -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().size() != 2) return {};

  auto sym_rhs = ctx->ExpRoute(node->Children().back());
  auto sym_lhs = ctx->ExpRoute(node->Children().front(), true);

  auto rhs = ctx->MapSymbol(sym_rhs);
  auto lhs = ctx->MapSymbol(sym_lhs);

  if (!lhs || !rhs) {
    ctx->AddError("Invalid assignment: " + node->Symbol()->Name());
    return {};
  }

  // check if lhs is an address
  auto is_address = MetaGet<bool>(lhs, SymbolMetaKey::kIS_ADDRESS, false);

  if (!is_address) {
    // if lhs is not an address, we store the value
    ctx->AddIns("mov", lhs, rhs);
  } else {
    // if lhs is an address, we need to store the value at the address
    ctx->AddIns("store", lhs, rhs);
  }

  return {};
}

auto IRIfHandler(IRGeneratorContext* ctx, const ASTNodePtr& node, bool is_lhs)
    -> SymbolPtr {
  auto children = node->Children();

  auto cond_sym = ctx->ExpRoute(children[0]);
  auto cond = ctx->MapSymbol(cond_sym);

  auto label_then = ctx->NewLabel();
  auto label_else = children.size() > 2 ? ctx->NewLabel() : nullptr;
  auto label_end = ctx->NewLabel();

  // cond jump
  if (label_else) {
    ctx->AddIns("brz", {}, cond, label_else);  // brz: if cond == 0 jump else
    ctx->AddIns("label", {}, label_then);
    ctx->ExpRoute(children[1]);  // then
    ctx->AddIns("jmp", {}, label_end);
    ctx->AddIns("label", {}, label_else);
    ctx->ExpRoute(children.back());  // else
    ctx->AddIns("label", {}, label_end);
  } else {
    ctx->AddIns("brz", {}, cond, label_end);  // brz: if cond == 0 jump end
    ctx->AddIns("label", {}, label_then);
    ctx->ExpRoute(children[1]);  // then
    ctx->AddIns("label", {}, label_end);
  }

  return nullptr;
}

auto IRCondHandler(IRGeneratorContext* ctx, const ASTNodePtr& node, bool is_lhs)
    -> SymbolPtr {
  auto children = node->Children();

  auto cond_sym = ctx->ExpRoute(children.front());
  auto cond = ctx->MapSymbol(cond_sym);

  auto label_then = ctx->NewLabel();
  auto label_else = children.size() > 2 ? ctx->NewLabel() : nullptr;
  auto label_end = ctx->NewLabel();

  auto tmp = ctx->NewTempVariable();

  // cond jump
  ctx->AddIns("brz", {}, cond, label_else);  // brz: if cond == 0 jump else
  ctx->AddIns("label", {}, label_then);
  auto lhs = ctx->ExpRoute(children[1]);  // then value
  ctx->AddIns("mov", tmp, lhs);
  ctx->AddIns("jmp", {}, label_end);
  ctx->AddIns("label", {}, label_else);
  ctx->ExpRoute(children.back());             // else
  auto rhs = ctx->ExpRoute(children.back());  // else value
  ctx->AddIns("mov", tmp, rhs);
  ctx->AddIns("label", {}, label_end);

  return tmp;
}

auto IRWhileHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                    bool is_lhs) -> SymbolPtr {
  const auto& children = node->Children();

  ASTNodePtr init;
  ASTNodePtr cond;
  ASTNodePtr step;
  ASTNodePtr body;

  for (const auto& child : children) {
    switch (child->Tag()) {
      case ASTNodeTag::kINIT:
        init = child;
        break;
      case ASTNodeTag::kCOND:
        cond = child;
        break;
      case ASTNodeTag::kSTEP:
        step = child;
        break;
      case ASTNodeTag::kBODY:
        body = child;
        break;
      default:
        break;
    }
  }

  auto label_cond = ctx->NewLabel();
  auto label_body = ctx->NewLabel();
  auto label_step = ctx->NewLabel();
  auto label_end = ctx->NewLabel();

  // set continue and break label
  auto symbol = node->Symbol();
  MetaSet(symbol, SymbolMetaKey::kCONTINUE_LABEL, label_step);
  MetaSet(symbol, SymbolMetaKey::kBREAK_LABEL, label_end);

  // 1. init
  if (init) ctx->ExpRoute(init);

  // cond
  ctx->AddIns("label", {}, label_cond);
  if (!cond) {
    // endless loop
    ctx->AddIns("jmp", {}, label_body);
  } else {
    auto cond_sym = ctx->ExpRoute(cond);
    auto cond_ir = ctx->MapSymbol(cond_sym);
    ctx->AddIns("brz", {}, cond_ir, label_end);  // if cond==0 jump
  }

  // body
  ctx->AddIns("label", {}, label_body);
  if (body) ctx->ExpRoute(body);

  ctx->AddIns("label", {}, label_step);

  // step
  if (step) ctx->ExpRoute(step);

  ctx->AddIns("jmp", {}, label_cond);

  // end
  ctx->AddIns("label", {}, label_end);

  return nullptr;
}

auto IRContinueBreakHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                            bool is_lhs) -> SymbolPtr {
  auto p_sym = node->Symbol();
  assert(p_sym != nullptr);

  if (node->Type() == ASTNodeType::kCONTINUE) {
    auto continue_label =
        MetaGet<SymbolPtr>(p_sym, SymbolMetaKey::kCONTINUE_LABEL);
    if (continue_label == nullptr) return nullptr;

    ctx->AddIns("jmp", {}, continue_label);
  }

  if (node->Type() == ASTNodeType::kBREAK) {
    auto break_label = MetaGet<SymbolPtr>(p_sym, SymbolMetaKey::kBREAK_LABEL);
    if (break_label == nullptr) return nullptr;

    ctx->AddIns("jmp", {}, break_label);
  }

  return nullptr;
}

auto IRProgramHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                      bool is_lhs) -> SymbolPtr {
  for (auto& fn : node->Children()) {
    if (fn->Symbol()->Name() == "__func_main") {
      ctx->ExpRoute(fn);
      break;
    }
  }
  for (auto& fn : node->Children()) {
    if (fn->Symbol()->Name() == "__func_main") continue;
    ctx->ExpRoute(fn);
  }
  return nullptr;
}

auto IRFunctionHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                       bool is_lhs) -> SymbolPtr {
  const auto func_name = node->Symbol()->Name();
  ctx->EnterInsGroup(func_name);

  ctx->AddIns("label", nullptr, ctx->MapSymbol(func_name, "function"));

  auto children = node->Children();

  int param_index = 0;
  for (auto& param_list_node : children) {
    if (param_list_node->Tag() != ASTNodeTag::kPARAMS) continue;
    for (auto& p : param_list_node->Children()) {
      auto p_sym = ctx->MapSymbol(p->Symbol());
      ctx->AddIns("arg", p_sym,
                  ctx->MapSymbol(std::to_string(param_index++), "immediate"));
    }
  }

  auto body = children.back();
  if (body->Tag() == ASTNodeTag::kBODY) ctx->ExpRoute(body);

  return nullptr;
}

auto IRCallHandler(IRGeneratorContext* ctx, const ASTNodePtr& node, bool is_lhs)
    -> SymbolPtr {
  std::vector<SymbolPtr> args;

  auto children = node->Children();
  for (auto& arg : children) {
    auto sym = ctx->ExpRoute(arg);
    args.push_back(sym);
  }

  auto ret_tmp = ctx->NewTempVariable();
  auto ret_sym = ctx->MapSymbol(ret_tmp);

  auto func_sym = ctx->MapSymbol(node->Symbol());
  auto func_name = func_sym->Name();

  // handle alloc_array function parameters
  if (func_name == "__func_alloc_array") {
    if (args.size() != 2) {
      ctx->AddError(
          "Function '__func_alloc_array' expects two arguments: "
          "'type' and 'int'.");
      return nullptr;
    }

    const auto& arg_0_sym = args.front();
    if (!arg_0_sym) {
      ctx->AddError("First argument of '__func_alloc_array' is missing.");
      return nullptr;
    }

    auto arg_0_type = MetaGet<SymbolPtr>(arg_0_sym, SymbolMetaKey::kTYPE);
    if (!arg_0_type) {
      ctx->AddError("First argument of '__func_alloc_array' must have a type.");
      return nullptr;
    }

    auto abort_label = ctx->NewLabel();
    auto after_label = ctx->NewLabel();

    // if (subscript < 0) goto abort
    auto compare_tmp = ctx->NewTempVariable();
    ctx->AddIns("lt", compare_tmp, args.back(),
                ctx->MapSymbol("0", "immediate"));
    ctx->AddIns("brnz", {}, compare_tmp, abort_label);

    ctx->AddIns("jmp", {}, after_label);
    ctx->AddIns("label", {}, abort_label);

    auto abort_tmp = ctx->NewTempVariable();
    ctx->AddIns("call", abort_tmp, ctx->MapSymbol("abort", "function"));
    ctx->AddIns("rtn", {}, abort_tmp);

    ctx->AddIns("label", {}, after_label);

    // void* calloc(size_t nmemb, size_t size);
    // allocate memory for the array
    auto tmp_elem_count = ctx->NewTempVariable();
    ctx->AddIns("add", tmp_elem_count, args.back(),
                ctx->MapSymbol("1", "immediate"));  // element count + 1

    ctx->AddIns("param", nullptr, tmp_elem_count);  // element count
    ctx->AddIns("param", nullptr,
                ctx->MapSymbol(arg_0_type->Value(),
                               "immediate"));  // element type size
  }

  else if (func_name == "__func_alloc") {
    if (args.size() != 1) {
      ctx->AddError("Function '__func_alloc' expects one argument: 'type'.");
      return nullptr;
    }

    const auto& arg_0_sym = args.front();
    if (!arg_0_sym) {
      ctx->AddError("First argument of '__func_alloc' is missing.");
      return nullptr;
    }

    auto arg_0_type = MetaGet<SymbolPtr>(arg_0_sym, SymbolMetaKey::kTYPE);
    if (!arg_0_type) {
      ctx->AddError("First argument of '__func_alloc' must have a type.");
      return nullptr;
    }

    if (arg_0_type->Value().empty()) {
      ctx->AddError("First argument of '__func_alloc' must have a valid size.");
      return nullptr;
    }

    // void* calloc(size_t nmemb, size_t size);
    // allocate memory for the object
    ctx->AddIns("param", nullptr,
                ctx->MapSymbol("1", "immediate"));  // element count
    ctx->AddIns("param", nullptr,
                ctx->MapSymbol(arg_0_type->Value(), "immediate"));
  }

  // handle normal function calls
  else {
    int index = 0;
    for (auto& a : args) {
      auto sym = ctx->MapSymbol(a);
      MetaSet<int>(sym, SymbolMetaKey::kPARAM_INDEX, index++);
      ctx->AddIns("param", nullptr, sym);
    }
  }

  ctx->AddIns("call", ret_sym, func_sym);

  if (func_name == "__func_alloc_array") {
    // set the element count for the allocated memory
    auto elem_count =
        args.size() == 2 ? args.back() : ctx->MapSymbol("1", "immediate");

    const auto& arg_0_sym = args.front();
    if (!arg_0_sym) {
      ctx->AddError("First argument of '" + func_name +
                    "' is missing. Cannot determine element type.");
      return nullptr;
    }

    auto arg_0_type = MetaGet<SymbolPtr>(arg_0_sym, SymbolMetaKey::kTYPE);
    if (!arg_0_type) {
      ctx->AddError("First argument of '" + func_name +
                    "' must have a type. Cannot determine element type.");
      return nullptr;
    }

    ctx->AddIns("store", ret_sym, elem_count);
    ctx->AddIns("add", ret_sym, ret_sym,
                ctx->MapSymbol(arg_0_type->Value(), "immediate"));
  }

  return ret_sym;
}

/**
 * |    |    |    |    Array Access<lvalue/exp,subscript,18>
 * |    |    |    |    |    Identity<arr,reference,18>
 * |    |    |    |    |    Value<10,int,18>
 */
auto IRArrayAccessHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                          bool is_lhs) -> SymbolPtr {
  auto children = node->Children();
  if (children.size() != 2) {
    ctx->AddError("Array access must have exactly one index: " +
                  node->Symbol()->Name());
    return {};
  }
  auto ident = ctx->ExpRoute(children.front());
  auto subscript = ctx->ExpRoute(children.back());

  assert(ident != nullptr && subscript != nullptr);

  // ident is an address
  ident->SetMeta(SymbolMetaKey::kIS_ADDRESS, true);

  // get element type
  auto arr_type =
      MetaGet<SymbolPtr>(children.front()->Symbol(), SymbolMetaKey::kTYPE);
  if (!arr_type) {
    ctx->AddError("Array access requires a valid array type.");
    return {};
  }

  // get array element size
  auto arr_base_type = MetaGet<SymbolPtr>(arr_type, SymbolMetaKey::kBASE_TYPE);
  if (!arr_base_type) {
    ctx->AddError("Array access requires a valid base type.");
    return {};
  }

  auto elem_size = std::stoi(arr_base_type->Value());
  if (elem_size <= 0) {
    ctx->AddError("Array element size must be greater than zero.");
    return {};
  }

  auto abort_label = ctx->NewLabel();
  auto after_label = ctx->NewLabel();

  // if (subscript < 0) goto abort
  auto compare_tmp = ctx->NewTempVariable();
  ctx->AddIns("lt", compare_tmp, subscript, ctx->MapSymbol("0", "immediate"));
  ctx->AddIns("brnz", {}, compare_tmp, abort_label);

  // get array length: ident - (elem_size) * 1
  auto p_arr_length = ctx->NewTempAddressVariable();
  ctx->AddIns("mov", p_arr_length, ident);
  ctx->AddIns("sub", p_arr_length, p_arr_length,
              ctx->MapSymbol(std::to_string(elem_size), "immediate"));

  // load array length
  auto arr_length = ctx->NewTempVariable();
  ctx->AddIns("load", arr_length, p_arr_length);

  // if (subscript >= arr_length) goto abort
  compare_tmp = ctx->NewTempVariable();
  ctx->AddIns("ge", compare_tmp, subscript, arr_length);
  ctx->AddIns("brnz", {}, compare_tmp, abort_label);

  SymbolPtr offset;
  if (elem_size == 1) {
    offset = subscript;
  } else {
    auto tmp_offset = ctx->NewTempVariable();
    ctx->AddIns("mul", tmp_offset, subscript,
                ctx->MapSymbol(std::to_string(elem_size), "immediate"));
    offset = ctx->MapSymbol(tmp_offset);
  }

  auto addr_tmp = ctx->NewTempAddressVariable();
  ctx->AddIns("add", addr_tmp, ident, offset);

  auto value_tmp = ctx->NewTempVariable();

  if (is_lhs) {
    // if this is an lvalue, we need to return the address
    ctx->AddIns("mov", value_tmp, addr_tmp);
    value_tmp->SetMeta(SymbolMetaKey::kIS_ADDRESS, true);
  } else {
    // if this is an expression, we return the value
    ctx->AddIns("load", value_tmp, addr_tmp);
  }

  ctx->AddIns("jmp", {}, after_label);
  ctx->AddIns("label", {}, abort_label);

  auto abort_tmp = ctx->NewTempVariable();
  ctx->AddIns("call", abort_tmp, ctx->MapSymbol("abort", "function"));
  ctx->AddIns("rtn", {}, abort_tmp);

  ctx->AddIns("label", {}, after_label);

  return ctx->MapSymbol(value_tmp);
}

auto IRFieldAccessHandler(IRGeneratorContext* ctx, const ASTNodePtr& node,
                          bool is_lhs) -> SymbolPtr {
  auto symbol = node->Symbol();
  auto children = node->Children();
  if (children.size() != 1) {
    ctx->AddError("Field access must have exactly one lvalue: " +
                  symbol->Name());
    return {};
  }

  auto base_addr = ctx->ExpRoute(children.front(), true);
  auto field_name = symbol->Name();

  auto l_value_type = MetaGet<SymbolPtr>(children.front()->Symbol(),
                                         SymbolMetaKey::kTYPE, nullptr);
  if (!l_value_type) {
    ctx->AddError("Field access requires a valid lvalue type.");
    return {};
  }

  // lvalue type name must start with "struct_"
  auto lvalue_type_name = l_value_type->Name();
  if (lvalue_type_name.rfind("struct", 0) == std::string::npos) {
    ctx->AddError("Field access requires a struct type, got: " +
                  lvalue_type_name);
    return {};
  }

  auto struct_type_name = lvalue_type_name.substr(7);  // remove "struct_"

  // look up l_value
  auto def_struct_sym =
      ctx->MapDefSym(symbol->Scope(), "__struct_" + struct_type_name);
  if (!def_struct_sym) {
    ctx->AddError("Field access failed: struct '" + struct_type_name +
                  "' not found.");
    return {};
  }

  spdlog::debug("IR Field access: {}:{} with type: {} and field: {}",
                def_struct_sym->Name(), def_struct_sym->Index(),
                l_value_type ? l_value_type->Name() : "unknown", field_name);

  // get field offset
  auto field_offset = MetaGet<std::map<std::string, size_t>>(
      def_struct_sym, SymbolMetaKey::kSTRUCT_FIELD_OFFSETS, {});

  auto it = field_offset.find(field_name);
  if (it == field_offset.end()) {
    ctx->AddError("Field '" + field_name + "' not found in struct '" +
                  struct_type_name + "'.");
    return {};
  }

  auto offset = it->second;
  spdlog::debug("Field '{}' offset: {}", field_name, offset);
  auto offset_sym = ctx->MapSymbol(std::to_string(offset), "immediate");

  auto field_addr = ctx->NewTempAddressVariable();
  ctx->AddIns("add", field_addr, base_addr, offset_sym);

  if (is_lhs) {
    field_addr->SetMeta(SymbolMetaKey::kIS_ADDRESS, true);
    return field_addr;
  }

  auto value_tmp = ctx->NewTempVariable();
  ctx->AddIns("load", value_tmp, field_addr);
  return ctx->MapSymbol(value_tmp);
}

const IRHandlerMapping kIRHandlerMapping = {
    {ASTNodeType::kASSIGN, IRAssignExpHandler},
    {ASTNodeType::kDECLARE, IRMeaninglessNodeHandler},
    {ASTNodeType::kRETURN, IRReturnExpHandler},
    {ASTNodeType::kUN_OP, IRUnOpExpHandler},
    {ASTNodeType::kBIN_OP, IRBinOpExpHandler},
    {ASTNodeType::kVALUE, IRValueExpHandler},
    {ASTNodeType::kPROGRAM, IRProgramHandler},
    {ASTNodeType::kFUNCTION, IRFunctionHandler},
    {ASTNodeType::kIDENT, IRValueExpHandler},
    {ASTNodeType::kBLOCK, IRMeaninglessNodeHandler},
    {ASTNodeType::kIF, IRIfHandler},
    {ASTNodeType::kCOND_EXP, IRCondHandler},
    {ASTNodeType::kWHILE, IRWhileHandler},
    {ASTNodeType::kCONTINUE, IRContinueBreakHandler},
    {ASTNodeType::kBREAK, IRContinueBreakHandler},
    {ASTNodeType::kCALL, IRCallHandler},
    {ASTNodeType::kTYPE, IRMeaninglessNodeHandler},
    {ASTNodeType::kARRAY_ACCESS, IRArrayAccessHandler},
    {ASTNodeType::kFIELD_ACCESS, IRFieldAccessHandler},
};

}  // namespace

auto GetIRHandlersMap() -> IRHandlerMapping { return kIRHandlerMapping; }