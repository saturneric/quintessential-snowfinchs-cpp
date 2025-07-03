#include "IGHandlers.h"

#include "model/AST.h"
#include "model/Symbol.h"
#include "model/SymbolDefs.h"
#include "model/SymbolMetaTypedef.h"
#include "model/Utils.h"

namespace {

auto DeclareHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());
  assert(sym != nullptr);

  ctx->AddIns("dcl", sym);
  return sym;
}

auto IRValueExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  assert(node != nullptr);

  auto sym = ctx->MapSymbol(node->Symbol());

  // ident
  if (node->Type() == ASTNodeType::kIDENT) return sym;

  // immediate
  auto type = MetaGet<SymbolMetaType>(node->Symbol(), SymbolMetaKey::kTYPE);

  if (type == SymbolMetaType::kBOOL) {
    if (sym->Name() == "true") {
      return ctx->MapSymbol("1", "immediate");
    }
    return ctx->MapSymbol("0", "immediate");
  }

  return sym;
}

auto IRBinOpExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

auto IRUnOpExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

  else {
    ctx->AddError("Unsupported unary operator: " + op);
    return {};
  }

  return sym_tmp;
}

auto IRMeaninglessNodeHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  for (const auto& child : node->Children()) {
    ctx->ExpRoute(child);
  }
  return {};
}

auto IRReturnExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().empty()) return {};

  auto sym_val = ctx->ExpRoute(children.front());

  ctx->AddIns("rtn", {}, sym_val);
  return {};
}

auto IRAssignExpHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  auto children = node->Children();

  if (node->Children().size() != 2) return {};

  auto sym_rhs = ctx->ExpRoute(node->Children().back());
  auto sym_lhs = ctx->ExpRoute(node->Children().front());

  auto rhs = ctx->MapSymbol(sym_rhs);
  auto lhs = ctx->MapSymbol(sym_lhs);

  ctx->AddIns("mov", lhs, rhs);
  return {};
}

auto IRIfHandler(IRGeneratorContext* ctx, const ASTNodePtr& node) -> SymbolPtr {
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

auto IRCondHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
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

auto IRWhileHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

auto IRContinueBreakHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

auto IRProgramHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

auto IRFunctionHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
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

auto IRCallHandler(IRGeneratorContext* ctx, const ASTNodePtr& node)
    -> SymbolPtr {
  std::vector<SymbolPtr> args;

  auto children = node->Children();
  for (auto& arg : children) {
    auto sym = ctx->ExpRoute(arg);
    args.push_back(sym);
  }

  auto ret_tmp = ctx->NewTempVariable();
  auto ret_sym = ctx->MapSymbol(ret_tmp);

  int index = 0;
  for (auto& a : args) {
    auto sym = ctx->MapSymbol(a);
    MetaSet<int>(sym, SymbolMetaKey::kPARAM_INDEX, index++);
    ctx->AddIns("param", nullptr, sym);
  }

  auto func_sym = ctx->MapSymbol(node->Symbol());
  ctx->AddIns("call", ret_sym, func_sym);

  return ret_sym;
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
};

}  // namespace

auto GetIRHandlersMap() -> IRHandlerMapping { return kIRHandlerMapping; }