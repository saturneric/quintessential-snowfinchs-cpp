#include "X86Translator.h"

#include "model/IRInstruction.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

using std::array;

namespace {

auto DemangleFuncName(const std::string& s) -> std::string {
  const std::string prefix = "__func_";
  if (s.rfind(prefix, 0) == 0) {  // s.startswith(prefix)
    return s.substr(prefix.size());
  }
  return s;
}

template <typename... Args>
auto MakeIRA2(Args&&... args) -> IRInstructionPtr {
  return std::make_shared<IRInstructionA2>(std::forward<Args>(args)...);
}

inline auto IsStackAccess(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  // "-4(%esp)" or "-8(%rsp)"
  return s->MetaData(SymbolMetaKey::kIN_STACK).has_value();
}

inline auto IsReg(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->MetaData(SymbolMetaKey::kIN_REGISTER).has_value();
}

inline auto IsGlobalVar(const SymbolPtr& s) -> bool {
  return s != nullptr && !IsReg(s) && !IsImmediate(s) && !IsStackAccess(s);
}

inline auto IsInMemory(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return IsStackAccess(s) /*speed up*/ || (!IsReg(s) && !IsImmediate(s));
}

const std::vector<std::string> kRegisters64 = {
    "%rbx", "%rcx", "%rsi", "%rdi", "%r8",  "%r9",
    "%r10", "%r11", "%r12", "%r13", "%r14", "%r15",
};

const std::vector<std::string> kRegisters32 = {
    "%ebx",  "%ecx",  "%esi",  "%edi",  "%r8d",  "%r9d",
    "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d"};

const std::array<const char*, 6> kFRegs32 = {"%edi", "%esi", "%edx",
                                             "%ecx", "%r8d", "%r9d"};

const std::array<const char*, 6> kFRegs64 = {"%rdi", "%rsi", "%rdx",
                                             "%rcx", "%r8",  "%r9"};

const std::set<std::string> kFRegsS64 = {"%rdi", "%rsi", "%rdx",
                                         "%rcx", "%r8",  "%r9"};

const std::set<std::string> kCallerSavedReg = {"%rcx", "%rdx", "%rsi", "%rdi",
                                               "%r8",  "%r9",  "%r10", "%r11"};

const std::set<std::string> kCalleeSavedReg = {"%rbx", "%rbp", "%r12",
                                               "%r13", "%r14", "%r15"};

auto To64BitReg(const std::string& reg) -> std::string {
  if (reg == "%eax" || reg == "%ax" || reg == "%al" || reg == "%ah") {
    return "%rax";
  }
  if (reg == "%ebx" || reg == "%bx" || reg == "%bl" || reg == "%bh") {
    return "%rbx";
  }
  if (reg == "%ecx" || reg == "%cx" || reg == "%cl" || reg == "%ch") {
    return "%rcx";
  }
  if (reg == "%edx" || reg == "%dx" || reg == "%dl" || reg == "%dh") {
    return "%rdx";
  }
  if (reg == "%esi" || reg == "%si" || reg == "%sil") return "%rsi";
  if (reg == "%edi" || reg == "%di" || reg == "%dil") return "%rdi";
  if (reg == "%ebp" || reg == "%bp" || reg == "%bpl") return "%rbp";
  if (reg == "%esp" || reg == "%sp" || reg == "%spl") return "%rsp";

  if (reg.size() >= 4 && reg.substr(1, 1) == "r" && reg.back() == 'd') {
    return reg.substr(0, reg.size() - 1);
  }
  return reg;
}
}  // namespace

X86Translator::X86Translator(bool r32) : r32_(r32), param_index_(0) {}

auto X86Translator::Optimums(const std::vector<IRInstructionPtr>& irs)
    -> std::vector<IRInstructionPtr> {
  std::vector<IRInstructionPtr> n_ir;

  IRInstructionPtr l_ir = nullptr;
  for (const auto& ir : irs) {
    const auto op = ir->Op()->Name();
    const auto dst = ir->DST();
    const auto src = ir->SRC(0);
    const auto src_2 = ir->SRC(1);

    if (op == "mov" && dst == src) continue;

    if (op == "brz" && l_ir && l_ir->Op()->Value() == kCmpOpType &&
        l_ir->DST() == src) {
      n_ir.pop_back();
      n_ir.push_back(MakeIRA2(mapOp("cmp"), l_ir->SRC(0), l_ir->SRC(1)));

      const auto op = l_ir->Op()->Name();

      if (op == "eq") {
        n_ir.push_back(MakeIRA2(mapOp("jne"), nullptr, src_2));
      }
      if (op == "neq") {
        n_ir.push_back(MakeIRA2(mapOp("je"), nullptr, src_2));
      }
      if (op == "lt") {
        n_ir.push_back(MakeIRA2(mapOp("jge"), nullptr, src_2));
      }
      if (op == "le") {
        n_ir.push_back(MakeIRA2(mapOp("jg"), nullptr, src_2));
      }
      if (op == "gt") {
        n_ir.push_back(MakeIRA2(mapOp("jle"), nullptr, src_2));
      }
      if (op == "ge") {
        n_ir.push_back(MakeIRA2(mapOp("jl"), nullptr, src_2));
      }

      l_ir = ir;
      continue;
    }

    if (op == "brz" && l_ir && l_ir->Op()->Name() == "lnot" &&
        l_ir->DST() == src) {
      n_ir.pop_back();
      n_ir.push_back(MakeIRA2(mapOp("brnz"), nullptr, l_ir->DST(), src_2));
      l_ir = ir;
      continue;
    }

    l_ir = ir;
    n_ir.push_back(ir);
  }

  return n_ir;
}

auto X86Translator::format_operand(const SymbolPtr& opr, bool force_use_loc)
    -> std::string {
  if (opr == nullptr) return {};

  if (opr->Value() == "label") return opr->Name();

  // is register
  if (opr->Value() == "register") return opr->Name();

  if (!force_use_loc) {
    // 1234 -1234 0x1234 0X1234
    if (IsImmediate(opr)) return "$" + opr->Name();
  }

  // have location
  auto loc = SymLoc(opr);
  if (!loc.empty()) return loc;

  // data var
  return r32_ ? opr->Name() : opr->Name() + "(%rip)";
}

void X86Translator::emit_jmp_op(std::vector<std::string>& fins,
                                const std::string& op, const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto src = format_operand(s_src);
  fins.push_back(op + " " + src);
}

void X86Translator::emit_spz_op(std::vector<std::string>& fins,
                                const std::string& op, const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto src = format_operand(s_src);

  if (op == "rtn") {
    if (src != acc_reg_) fins.push_back(op_mov_ + " " + src + ", " + acc_reg_);
    fins.emplace_back("leave");
    fins.emplace_back("ret");
  }

  if (op == "label") {
    fins.push_back(s_src->Value() == "function" ? DemangleFuncName(src) + ": "
                                                : src + ": ");
  }
}

void X86Translator::emit_mov_op(std::vector<std::string>& fins,
                                const std::string& op, const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto s_dst = i.DST();
  const auto src = format_operand(s_src);
  const auto dst = format_operand(s_dst);

  if (op == "mov") {
    // dst should not be constant
    assert(!IsImmediate(s_dst));

    bool src_mem = !IsReg(s_src) && !IsImmediate(s_src);
    bool dst_mem = !IsReg(s_dst);

    // [mem] -> [mem]
    if (src_mem && dst_mem) {
      fins.push_back(op_mov_ + " " + src + ", " + acc_reg_);
      fins.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
    } else {
      fins.push_back(op_mov_ + " " + src + ", " + dst);
    }
  }

  if (op == "load") {
    // dst should not be constant
    assert(!IsImmediate(s_dst));

    bool src_mem = !IsReg(s_src) && !IsImmediate(s_src);
    bool dst_mem = !IsReg(s_dst);

    if (src_mem) {
      // [mem] -> *
      fins.push_back(op_mov_ + " " + src + ", " + acc_reg_);
      fins.push_back(op_mov_ + " (" + acc_reg_ + "), " + dst);
    } else {
      fins.push_back(op_mov_ + " (" + src + "), " + dst);
    }
  }

  if (op == "store") {
    // src should not be constant
    assert(!IsImmediate(s_dst));

    bool src_mem = !IsReg(s_src) && !IsImmediate(s_src);
    bool dst_mem = !IsReg(s_dst);

    if (dst_mem) {
      // * -> [mem]
      fins.push_back(op_mov_ + " " + dst + ", " + acc_reg_);
      fins.push_back(op_mov_ + src + ", (" + acc_reg_ + ")");
    } else {
      fins.push_back(op_mov_ + " " + src + ", (" + dst + ")");
    }
  }
}

void X86Translator::emit_binary_op(std::vector<std::string>& fins,
                                   const std::string& op,
                                   const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto s_dst = i.DST();
  auto src = format_operand(s_src);
  const auto dst = format_operand(s_dst);

  const auto check_move_src_2_reg = [&]() {
    // should not be [mem] -> [mem]
    if (IsInMemory(s_src) && IsInMemory(s_dst)) {
      // %eax is reserved
      assert(dst != acc_reg_);
      fins.push_back(op_mov_ + " " + src + ", " + acc_reg_);
      return acc_reg_;
    }
    return src;
  };

  if (op == "add" || op == "sub") {
    fins.push_back(op + suffix_ + " " + check_move_src_2_reg() + ", " + dst);
    return;
  }

  if (op == "band") {
    fins.push_back("and" + suffix_ + " " + check_move_src_2_reg() + ", " + dst);
    return;
  }

  if (op == "bor") {
    fins.push_back("or" + suffix_ + " " + check_move_src_2_reg() + ", " + dst);
    return;
  }

  if (op == "bxor") {
    fins.push_back("xor" + suffix_ + " " + check_move_src_2_reg() + ", " + dst);
    return;
  }

  if (op == "shl" || op == "shr" || op == "sal" || op == "sar") {
    if (!IsImmediate(s_src)) {
      // dst == %ecx, special case
      if (dst == count_reg_) {
        fins.push_back(op_mov_ + " " + dst + ", " + rem_reg_);
        fins.push_back(op_mov_ + " " + src + ", " + count_reg_);
        fins.push_back(op + suffix_ + " " + count_reg_low_ + ", " + rem_reg_);
        fins.push_back(op_mov_ + " " + rem_reg_ + ", " + dst);
        return;
      }

      // borrow %ecx
      fins.push_back(op_push_ + " " + count_reg_64_);
      fins.push_back(op_mov_ + " " + src + ", " + count_reg_);
      fins.push_back(op + suffix_ + " " + count_reg_low_ + ", " + dst);
      // bring back %ecx
      fins.push_back(op_pop_ + " " + count_reg_64_);
      return;
    }

    // shl imm -> $dst
    fins.push_back(op + suffix_ + " " + src + ", " + dst);
    return;
  }

  // alloc stack for immediate and mov immediate to stack memory
  if (IsImmediate(s_src)) {
    fins.push_back(op_mov_ + " " + src + ", " +
                   alloc_stack_for_immediate(s_src));
  }

  if (op == "mul") {
    fins.push_back(op_mov_ + " " + dst + ", " + acc_reg_);
    fins.push_back("imul" + suffix_ + " " + format_operand(s_src, true));
    fins.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
    return;
  }

  if (op == "div" || op == "mod") {
    fins.push_back(op_mov_ + " " + dst + ", " + acc_reg_);
    fins.emplace_back(r32_ ? "cltd" : "cqto");

    // src is already in mem or reg
    fins.push_back("idiv" + suffix_ + " " + format_operand(s_src, true));

    if (op == "mod") {
      fins.push_back(op_mov_ + " " + rem_reg_ + ", " + dst);
    } else {
      fins.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
    }
  } else {
    fins.push_back("# unsupported op: " + op);
  }
}

void X86Translator::emit_cmp_op(std::vector<std::string>& fins,
                                const std::string& op, const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto s_dst = i.DST();
  const auto s_src_2 = i.SRC(1);
  auto src = format_operand(s_src);
  auto dst = format_operand(s_dst);
  auto src_2 = format_operand(s_src_2);

  if (op == "cmp") {
    if (IsImmediate(s_dst) || (IsInMemory(s_src) && IsInMemory(s_dst))) {
      fins.push_back(op_mov_ + " " + dst + ", " + acc_reg_);
      dst = acc_reg_;
    }

    fins.push_back("cmp" + suffix_ + " " + src + ", " + dst);
    return;
  }

  if (op == "brz") {
    if (IsImmediate(s_src)) {
      fins.emplace_back(op_mov_ + " " + src + ", " + acc_reg_);
      fins.push_back("cmp" + suffix_ + " $0, " + acc_reg_);
    } else {
      fins.push_back("cmp" + suffix_ + " $0, " + src);
    }
    fins.push_back("je " + src_2);
    return;
  }

  if (op == "brnz") {
    if (!IsReg(s_src)) {
      fins.emplace_back(op_mov_ + " " + src + ", " + acc_reg_);
      fins.push_back("test" + suffix_ + " " + acc_reg_ + ", " + acc_reg_);
    } else {
      fins.push_back("test" + suffix_ + " " + src + ", " + src);
    }
    fins.push_back("jnz " + src_2);
    return;
  }

  fins.emplace_back(op_mov_ + " " + src + ", " + acc_reg_);
  fins.emplace_back("cmp" + suffix_ + " " + src_2 + ", " + acc_reg_);

  if (op == "eq") {
    fins.push_back("sete " + acc_reg_low_);
  }

  else if (op == "le") {
    fins.push_back("setle " + acc_reg_low_);
  }

  else if (op == "neq") {
    fins.push_back("setne " + acc_reg_low_);
  }

  else if (op == "lt") {
    fins.push_back("setl " + acc_reg_low_);
  }

  else if (op == "gt") {
    fins.push_back("setg " + acc_reg_low_);
  }

  else if (op == "ge") {
    fins.push_back("setge " + acc_reg_low_);
  }

  fins.push_back("movzx " + acc_reg_low_ + ", " + acc_reg_);
  fins.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
}

void X86Translator::emit_logic_op(std::vector<std::string>& fins,
                                  const std::string& op,
                                  const IRInstruction& i) {
  std::string suffix = r32_ ? "l" : "q";
  std::string acc_reg = r32_ ? "%eax" : "%rax";
  std::string acc_reg_low = "%al";
  std::string rem_reg = r32_ ? "%edx" : "%rdx";
  std::string rem_low_reg = "%dl";

  const auto s_src = i.SRC(0);
  const auto s_dst = i.DST();
  const auto src = format_operand(s_src);
  const auto dst = format_operand(s_dst);

  const auto op_mov = "mov" + suffix;

  if (op == "land") {
    fins.emplace_back(op_mov + " " + src + ", " + acc_reg);  // mov src -> %eax
    fins.emplace_back("test " + acc_reg + ", " + acc_reg);   // test %eax %eax
    fins.push_back("setne " + acc_reg_low);                  // test %al

    fins.emplace_back(op_mov + " " + dst + ", " + rem_reg);  // mov src -> %edx
    fins.emplace_back("test " + rem_reg + ", " + rem_reg);   // test %edx %edx
    fins.push_back("setne " + rem_low_reg);                  // test %dl

    fins.emplace_back("and " + rem_low_reg + ", " +
                      acc_reg_low);  // and %dl %al -> %al
    fins.push_back("movzx " + acc_reg_low + ", " +
                   acc_reg);  // movzx %al -> %eax
    fins.emplace_back(op_mov + " " + acc_reg + ", " + dst);  // mov %eax -> dst
  }

  if (op == "lor") {
    fins.emplace_back(op_mov + " " + src + ", " + acc_reg);  // mov src -> %eax
    fins.emplace_back("test " + acc_reg + ", " + acc_reg);   // test %eax %eax
    fins.push_back("setne " + acc_reg_low);                  // test %al

    fins.emplace_back(op_mov + " " + dst + ", " + rem_reg);  // mov src -> %edx
    fins.emplace_back("test " + rem_reg + ", " + rem_reg);   // test %edx %edx
    fins.push_back("setne " + rem_low_reg);                  // test %dl

    fins.emplace_back("or " + rem_low_reg + ", " +
                      acc_reg_low);  // and %dl %al -> %al
    fins.push_back("movzx " + acc_reg_low + ", " +
                   acc_reg);  // movzx %al -> %eax
    fins.emplace_back(op_mov + " " + acc_reg + ", " + dst);  // mov %eax -> dst
  }
}

void X86Translator::emit_unary_op(std::vector<std::string>& fins,
                                  const std::string& op,
                                  const IRInstruction& i) {
  const auto s_src = i.SRC(0);
  const auto s_dst = i.DST();
  const auto dst = format_operand(s_dst);

  if (op == "neg") {
    fins.push_back("neg" + suffix_ + " " + dst);
  }

  else if (op == "lnot") {
    fins.push_back("cmp" + suffix_ + " $0, " + dst);
    fins.push_back("sete " + acc_reg_low_);
    fins.push_back("movzx " + acc_reg_low_ + ", " + acc_reg_);
    fins.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
  }

  if (op == "bnot") {
    fins.push_back("not" + suffix_ + " " + dst);
  }
}

auto X86Translator::translate(const std::vector<IRInstructionPtr>& ir,
                              const std::set<std::string>& al_regs)
    -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& p_i : ir) {
    const auto& i = *p_i;

    auto op = i.Op()->Name();

    if (op == "mov" || op == "load" || op == "store") {
      emit_mov_op(ret, op, i);
    } else if (op == "add" || op == "sub" || op == "mul" || op == "div" ||
               op == "mod" || op == "band" || op == "bor" || op == "bxor" ||
               op == "shl" || op == "shr" || op == "sal" || op == "sar") {
      emit_binary_op(ret, op, i);
    } else if (op == "eq" || op == "neq" || op == "lt" || op == "le" ||
               op == "gt" || op == "ge" || op == "cmp" || op == "brz" ||
               op == "brnz") {
      emit_cmp_op(ret, op, i);
    } else if (op == "land" || op == "lor") {
      emit_logic_op(ret, op, i);
    } else if (op == "neg" || op == "lnot" || op == "bnot") {
      emit_unary_op(ret, op, i);
    } else if (op == "jmp" || op == "je" || op == "jne" || op == "jl" ||
               op == "jle" || op == "jg" || op == "jge" || op == "jnz") {
      emit_jmp_op(ret, op, i);
    } else if (op == "label" || op == "rtn") {
      emit_spz_op(ret, op, i);
    } else if (op == "call" || op == "param" || op == "arg") {
      emit_func_op(ret, op, i, al_regs);
    } else {
      ret.push_back("# unsupported op: " + op);
    }
  }

  return ret;
}

void X86Translator::HandleVariables(const std::set<SymbolPtr>& vars) {
  // cache
  vars_ = vars;
}

auto X86Translator::alloc_stack_for_immediate(const SymbolPtr& val)
    -> std::string {
  auto loc = SymLoc(val);
  if (!loc.empty()) return loc;

  stack_offset_ -= stack_offset_dt_;
  loc = std::to_string(stack_offset_) + "(" + bp_ + ")";
  val->MetaRef(SymbolMetaKey::kLOCATION) = loc;
  return loc;
}

auto X86Translator::alloc_stack() -> std::string {
  stack_offset_ -= stack_offset_dt_;
  return std::to_string(stack_offset_) + "(" + bp_ + ")";
}

auto X86Translator::gen_data_var_immediate_label(const SymbolPtr& val)
    -> std::string {
  std::string label = "const_";
  for (char ch : val->Name()) {
    if (ch == '-') {
      label += "n";
    } else {
      label += ch;
    }
  }
  return label;
}

auto X86Translator::pre_alloc_stack_memory() -> std::vector<std::string> {
  std::vector<std::string> ret;

  ret.push_back("push " + bp_);
  ret.push_back("mov " + sp_ + ", " + bp_);

  if (std::abs(stack_offset_) != 0) {
    ret.push_back("sub $" + std::to_string(std::abs(stack_offset_)) + ", " +
                  sp_);
  }

  return ret;
}

auto X86Translator::GenerateDataSegment() -> std::vector<std::string> {
  std::vector<std::string> ret;

  // external symbols
  ret.emplace_back(".extern fflush");
  ret.emplace_back(".extern getchar");
  ret.emplace_back(".extern putchar");
  ret.emplace_back(".extern stdout");
  ret.emplace_back(".extern calloc");

  for (const auto& c : in_data_sec_vars_) {
    if (IsImmediate(c)) {
      ret.push_back(gen_data_var_immediate_label(c) +
                    std::string{r32_ ? ": .int " : ": .quad "} + c->Name());
    } else {
      ret.push_back(c->Name() + std::string{r32_ ? ": .int 0" : ": .quad 0"});
    }
  }
  return ret;
}

auto X86Translator::GenerateTextSection(const std::vector<IRInstructionPtr>& ir,
                                        const std::set<std::string>& al_regs)
    -> std::vector<std::string> {
  stack_offset_ = 0;
  for (const auto& var : vars_) {
    if (!MetaGet<bool>(var, SymbolMetaKey::kIS_SPILLED)) continue;

    stack_offset_ -= stack_offset_dt_;
    var->MetaRef(SymbolMetaKey::kIN_STACK) = true;
    var->MetaRef(SymbolMetaKey::kLOCATION) =
        std::to_string(stack_offset_) + "(" + bp_ + ")";
  }

  // alloc stack memory for necessary regs
  for (const auto& reg : kCallerSavedReg) {
    f_regs_loc_[reg] = alloc_stack();
  }

  // alloc stack memory for %rsp
  f_regs_loc_["%rsp"] = alloc_stack_64();

  std::vector<std::string> need_save;
  for (const auto& reg : al_regs) {
    auto reg64 = To64BitReg(reg);
    if (kCalleeSavedReg.count(reg64) != 0U) need_save.push_back(reg64);
  }

  std::vector<std::string> save_ins;
  std::vector<std::string> restore_ins;
  save_ins.reserve(need_save.size());

  for (const auto& reg : need_save) {
    save_ins.push_back("push " + reg);
  }

  for (auto it = need_save.rbegin(); it != need_save.rend(); ++it) {
    restore_ins.push_back("pop " + *it);
  }

  auto main_ins = translate(ir, al_regs);

  // this function need stack size info
  // so run this after translate()
  auto stack_ins = pre_alloc_stack_memory();

  std::vector<std::string> ins;
  ins.push_back(main_ins.front());
  ins.insert(ins.end(), stack_ins.begin(), stack_ins.end());

  for (const auto& reg : req_args_regs_) {
    ins.push_back("mov " + reg + ", " + f_regs_loc_.at(To64BitReg(reg)) +
                  "   # save arg reg state");
  }

  ins.insert(ins.end(), save_ins.begin(), save_ins.end());
  ins.insert(ins.end(), main_ins.begin() + 1, main_ins.end());

  for (auto it = ins.begin(); it != ins.end(); /* note: no ++it here */) {
    if (*it == "leave") {
      it = ins.insert(it, restore_ins.begin(), restore_ins.end());
      std::advance(it, restore_ins.size() + 1);
    } else {
      ++it;
    }
  }

  Reset();
  return ins;
}

auto X86Translator::AvailableRegisters() const -> std::vector<std::string> {
  return r32_ ? kRegisters32 : kRegisters64;
}

void X86Translator::Reset() {
  vars_.clear();
  in_data_sec_vars_.clear();
  param_index_ = 0;
  stack_offset_ = 0;
  req_args_regs_.clear();
  f_regs_loc_.clear();
}

void X86Translator::emit_func_op(std::vector<std::string>& out,
                                 const std::string& op, const IRInstruction& i,
                                 const std::set<std::string>& al_regs) {
  const auto& f_regs = r32_ ? kFRegs32 : kFRegs64;

  if (op == "param") {
    auto s_arg = i.SRC(0);
    auto arg = format_operand(s_arg);

    if (param_index_ == 0) {
      auto ret = store_allocated_function_args(al_regs);
      out.insert(out.end(), ret.begin(), ret.end());
    }

    if (param_index_ < 6) {
      auto target = std::string(f_regs[param_index_]);

      if (IsReg(s_arg) && kFRegsS64.count(To64BitReg(arg)) != 0U) {
        out.push_back(op_mov_ + " " + f_regs_loc_.at(To64BitReg(arg)) + ", " +
                      target);
      } else {
        out.push_back(op_mov_ + " " + arg + ", " + target);
      }

    } else {
      stack_args_.push_back(s_arg);
    }

    param_index_++;
  }

  else if (op == "call") {
    auto func = format_operand(i.SRC(0));

    if (saved_f_regs_.empty() && !al_regs.empty()) {
      auto ret = store_allocated_function_args(al_regs);
      out.insert(out.end(), ret.begin(), ret.end());
    }

    out.push_back("mov " + sp_ + ", " + f_regs_loc_.at("%rsp"));
    out.push_back("and $-16, " + sp_);

    const bool need_pad = (!stack_args_.empty() && stack_args_.size() % 2 == 1);
    if (need_pad) out.emplace_back("sub $8, %rsp");

    for (auto it = stack_args_.rbegin(); it != stack_args_.rend(); ++it) {
      const auto& s_arg = *it;
      auto arg = format_operand(*it);

      if (IsReg(s_arg)) {
        const auto reg64 = To64BitReg(arg);
        if (kFRegsS64.count(reg64) != 0U) {
          out.push_back("push " + f_regs_loc_.at(reg64));
        } else {
          out.push_back("push " + reg64);
        }
      } else {
        out.push_back("push " + arg);
      }
    }

    if (func == "__func_print") {
      out.emplace_back("call putchar");
      out.push_back(op_mov_ + " $0," + acc_reg_);
    }

    else if (func == "__func_read") {
      out.emplace_back("call getchar");
    }

    else if (func == "__func_flush") {
      out.emplace_back("mov stdout(%rip), %rdi");
      out.emplace_back("call fflush");
      out.push_back(op_mov_ + " $0," + acc_reg_);
    }

    else if (func == "__func_alloc_array") {
      out.emplace_back("call calloc");
    }

    else {
      out.push_back("call " + DemangleFuncName(func));
    }

    std::size_t bytes_to_pop = (stack_args_.size() * 8) + (need_pad ? 8 : 0);
    if (bytes_to_pop != 0U) {
      out.emplace_back("add $" + std::to_string(bytes_to_pop) + ", %rsp");
    }

    out.emplace_back("mov " + f_regs_loc_.at("%rsp") + ", " + sp_);

    for (const auto& reg : saved_f_regs_) {
      auto reg64 = To64BitReg(reg);
      if (kCallerSavedReg.count(reg64) != 0U) {
        out.push_back(op_mov_ + " " + f_regs_loc_.at(reg64) + ", " + reg +
                      "    # restore reg state");
      }
    }

    auto s_ret = i.DST();
    auto dst = format_operand(s_ret);
    if (dst != acc_reg_) {
      out.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
    }

    param_index_ = 0;
    stack_args_.clear();
    saved_f_regs_.clear();
  }

  else if (op == "arg") {
    int idx = std::stoi(i.SRC(0)->Name());
    auto s_dst = i.DST();
    auto dst = format_operand(s_dst);

    if (idx < 6) {
      // load from the appropriate register
      const auto* const reg = f_regs[idx];
      if (IsReg(s_dst) && kFRegsS64.count(To64BitReg(reg)) != 0U) {
        out.push_back(op_mov_ + " " + f_regs_loc_.at(To64BitReg(reg)) + ", " +
                      dst);
        req_args_regs_.emplace_back(reg);
      } else {
        out.push_back(op_mov_ + " " + reg + ", " + dst);
      }
    } else {
      int stack_offset = 16 + (8 * (idx - 6));
      bool dst_mem = !IsReg(s_dst);
      if (dst_mem) {
        out.push_back(op_mov_ + " " + std::to_string(stack_offset) +
                      "(%rbp), " + acc_reg_);
        out.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
      } else {
        out.push_back(op_mov_ + " " + std::to_string(stack_offset) +
                      "(%rbp), " + dst);
      }
    }
  }
}

auto X86Translator::alloc_stack_64() -> std::string {
  stack_offset_ -= 8;
  return std::to_string(stack_offset_) + "(" + bp_ + ")";
}

auto X86Translator::store_allocated_function_args(
    const std::set<std::string>& al_regs) -> std::vector<std::string> {
  std::vector<std::string> ret;
  for (const auto& reg : al_regs) {
    auto reg64 = To64BitReg(reg);
    if (kCallerSavedReg.count(reg64) == 0U) continue;
    ret.push_back(op_mov_ + " " + reg + ", " + f_regs_loc_.at(reg64) +
                  "   # save reg state");
    saved_f_regs_.push_back(reg);
  }
  return ret;
}
