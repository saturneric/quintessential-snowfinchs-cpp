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

const std::array<const char*, 8> kCallerSavedReg = {
    "%rcx", "%rdx", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r11"};
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

  if (op != "mov") return;

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
  if (IsImmediate(s_src) && SymLoc(s_src).empty()) {
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

auto X86Translator::translate(const std::vector<IRInstructionPtr>& ir)
    -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& p_i : ir) {
    const auto& i = *p_i;

    auto op = i.Op()->Name();

    if (op == "mov") {
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
      emit_func_op(ret, op, i);
    } else {
      ret.push_back("# unsupported op: " + op);
    }
  }

  return ret;
}

void X86Translator::HandleVariables(const std::set<SymbolPtr>& vars) {
  for (const auto& var : vars) {
    if (!MetaGet<bool>(var, SymbolMetaKey::kIS_SPILLED)) continue;

    stack_offset_ -= stack_offset_dt_;
    var->MetaRef(SymbolMetaKey::kIN_STACK) = true;
    var->MetaRef(SymbolMetaKey::kLOCATION) =
        std::to_string(stack_offset_) + "(" + bp_ + ")";
  }

  // cache
  vars_ = vars;
}

auto X86Translator::alloc_stack_for_immediate(const SymbolPtr& val)
    -> std::string {
  auto loc = SymLoc(val);
  if (!loc.empty()) return loc;

  stack_offset_ -= stack_offset_dt_;
  loc = std::to_string(stack_offset_) + +"(" + bp_ + ")";
  val->MetaRef(SymbolMetaKey::kLOCATION) = loc;
  return loc;
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

auto X86Translator::alloc_stack_memory() -> std::vector<std::string> {
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

auto X86Translator::GenerateTextSection(const std::vector<IRInstructionPtr>& ir)
    -> std::vector<std::string> {
  auto main_ins = translate(ir);

  // this function need stack size info
  // so run this after translate()
  auto stack_ins = alloc_stack_memory();

  std::vector<std::string> ins;
  ins.push_back(main_ins.front());
  ins.insert(ins.end(), stack_ins.begin(), stack_ins.end());
  ins.insert(ins.end(), main_ins.begin() + 1, main_ins.end());

  return ins;
}

auto X86Translator::AvailableRegisters() const -> std::vector<std::string> {
  return r32_ ? kRegisters32 : kRegisters64;
}

void X86Translator::Reset() {
  vars_.clear();
  in_data_sec_vars_.clear();
  param_index_ = 0;
}

void X86Translator::emit_func_op(std::vector<std::string>& out,
                                 const std::string& op,
                                 const IRInstruction& i) {
  if (op == "param") {
    auto s_arg = i.SRC(0);
    auto arg = format_operand(s_arg);

    if (param_index_ < 6) {
      out.push_back(op_mov_ + " " + arg + ", " +
                    (r32_ ? kFRegs32[param_index_] : kFRegs64[param_index_]));
    } else {
      out.push_back("push " + arg);
    }
    param_index_++;
  }

  else if (op == "call") {
    param_index_ = 0;
    auto func = format_operand(i.SRC(0));

    if (func == "__func_print") {
      out.emplace_back("call putchar");
      if (i.DST()) {
        auto dst = format_operand(i.DST());
        out.push_back(op_mov_ + " $0, " + dst);
      }
      return;
    }

    if (func == "__func_read") {
      out.emplace_back("call   getchar");
      if (i.DST()) {
        auto dst = format_operand(i.DST());
        out.push_back(op_mov_ + " %eax, " + dst);
      }
      return;
    }

    if (func == "__func_flush") {
      out.emplace_back("lea stdout(%rip), %rdi");
      out.emplace_back("call fflush");
      if (i.DST()) {
        auto dst = format_operand(i.DST());
        out.push_back(op_mov_ + " %eax, " + dst);
      }
      return;
    }

    // for (const auto& r : kCallerSavedReg) {
    //   out.push_back("push " + std::string(r));
    // }

    out.push_back("call " + DemangleFuncName(func));

    // for (auto it = kCallerSavedReg.rbegin(); it != kCallerSavedReg.rend();
    //      ++it) {
    //   out.push_back("pop " + std::string(*it));
    // }

    auto s_ret = i.DST();
    auto dst = format_operand(s_ret);
    if (dst != acc_reg_) {
      out.push_back(op_mov_ + " " + acc_reg_ + ", " + dst);
    }
  }

  else if (op == "arg") {
    int idx = std::stoi(i.SRC(0)->Name());
    auto dst = format_operand(i.DST());

    if (idx < 6) {
      // load from the appropriate register
      out.push_back(op_mov_ + " " + (r32_ ? kFRegs32[idx] : kFRegs64[idx]) +
                    ", " + dst);
    } else {
      int stack_offset = 8 * (idx - 6 + 1);
      out.push_back(op_mov_ + " " + std::to_string(stack_offset) + "(%rbp), " +
                    dst);
    }
  }
}
