#include "ASMGenerator.h"

#include <spdlog/fmt/ranges.h>

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp>

#include "Utils.h"

namespace {

template <typename... Args>
auto MakeIRA2(Args&&... args) -> std::shared_ptr<IRInstructionA2> {
  return std::make_shared<IRInstructionA2>(std::forward<Args>(args)...);
}

}  // namespace

ASMGenerator::ASMGenerator(SymbolTablePtr symbol_table, bool r32,
                           const std::vector<IRInstructionA2Ptr>& ir2)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kIR, symbol_table_),
      r32_(r32),
      ir2_(ir2) {}

void ASMGenerator::Generate(const std::string& path) {
  spdlog::stopwatch sw;

  build_inf_graph();

  spdlog::debug("Built Inf Graph: {:.3}s", sw);

  mcs();

  spdlog::debug("Run MCS: {:.3}s", sw);

  alloc_reg();

  spdlog::debug("Allocated Register: {:.3}s", sw);

  handle_spling_var();

  spdlog::debug("Handled Spilled Variables: {:.3}s", sw);

  optimums();

  gen_final_asm_source(path);

  spdlog::debug("Generated ASM Source Code: {:.3}s", sw);
}

namespace {

auto IsImmediate(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->Value() == "immediate";
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

}  // namespace

auto ASMGenerator::translate(const std::vector<IRInstructionA2Ptr>& ir)
    -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& p_i : ir) {
    const auto& i = *p_i;

    auto op = i.op->Name();
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
    } else {
      ret.push_back("# unsupported op: " + op);
    }
  }

  return ret;
}

void ASMGenerator::emit_jmp_op(std::vector<std::string>& fins,
                               const std::string& op,
                               const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto src = format_operand(s_src);
  fins.push_back(op + " " + src);
}

void ASMGenerator::emit_spz_op(std::vector<std::string>& fins,
                               const std::string& op,
                               const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto src = format_operand(s_src);

  if (op == "rtn") {
    if (src != acc_reg_) fins.push_back(op_mov_ + " " + src + ", " + acc_reg_);
    fins.emplace_back("leave");
    fins.emplace_back("ret");
  }

  if (op == "label") {
    fins.push_back(src + ": ");
  }
}

void ASMGenerator::emit_mov_op(std::vector<std::string>& fins,
                               const std::string& op,
                               const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto s_dst = i.dst;
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

void ASMGenerator::emit_binary_op(std::vector<std::string>& fins,
                                  const std::string& op,
                                  const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto s_dst = i.dst;
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

void ASMGenerator::emit_cmp_op(std::vector<std::string>& fins,
                               const std::string& op,
                               const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto s_dst = i.dst;
  const auto s_src_2 = i.src_2;
  auto src = format_operand(s_src);
  auto dst = format_operand(s_dst);
  auto src_2 = format_operand(s_src_2);

  if (op == "cmp") {
    if (IsImmediate(s_dst)) {
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

void ASMGenerator::emit_logic_op(std::vector<std::string>& fins,
                                 const std::string& op,
                                 const IRInstructionA2& i) {
  std::string suffix = r32_ ? "l" : "q";
  std::string acc_reg = r32_ ? "%eax" : "%rax";
  std::string acc_reg_low = "%al";
  std::string rem_reg = r32_ ? "%edx" : "%rdx";
  std::string rem_low_reg = "%dl";

  const auto s_src = i.src;
  const auto s_dst = i.dst;
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

void ASMGenerator::emit_unary_op(std::vector<std::string>& fins,
                                 const std::string& op,
                                 const IRInstructionA2& i) {
  const auto s_src = i.src;
  const auto s_dst = i.dst;
  const auto dst = format_operand(s_dst);

  if (op == "neg") {
    fins.push_back("neg" + suffix_ + " " + dst);
  }

  else if (op == "lnot") {
    fins.push_back("cmp" + suffix_ + " $0, " + dst);
    fins.push_back("sete " + acc_reg_low_);
    fins.push_back("movzx " + acc_reg_low_ + ", " + dst);
  }

  if (op == "bnot") {
    fins.push_back("not" + suffix_ + " " + dst);
  }
}

auto ASMGenerator::format_operand(const SymbolPtr& opr, bool force_use_loc)
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

auto ASMGenerator::generate_data_segment() const -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& c : constants_) {
    if (IsImmediate(c)) {
      ret.push_back(gen_data_var_immediate_label(c) +
                    std::string{r32_ ? ": .int " : ": .quad "} + c->Name());
    } else {
      ret.push_back(c->Name() + std::string{r32_ ? ": .int 0" : ": .quad 0"});
    }
  }
  return ret;
}

const std::vector<std::string> kRegisters64 = {
    "%rbx", "%rcx", "%rsi", "%rdi", "%r8",  "%r9",
    "%r10", "%r11", "%r12", "%r13", "%r14", "%r15",
};

const std::vector<std::string> kRegisters32 = {
    "%ebx",
    "%ecx",
    "%esi",
    "%edi",
};

void ASMGenerator::alloc_reg() {
  auto registers = r32_ ? kRegisters32 : kRegisters64;
  const auto num_registers = registers.size();

  using Graph = InterferenceGraph::Graph;
  const Graph& graph = inf_graph_.GetGraph();

  std::map<InterferenceGraph::Vertex, decltype(registers.size())>
      color_map_storage;
  auto colored_map = boost::make_assoc_property_map(color_map_storage);

  std::vector<InterferenceGraph::Vertex> vertex_order;
  for (const auto& var : mcs_order_) {
    auto v_opt = inf_graph_.GetVertexBySymbol(var);
    assert(v_opt);

    if (v_opt) vertex_order.push_back(*v_opt);
  }

  // greedy coloring using MCS order
  boost::sequential_vertex_coloring(
      graph,
      boost::make_iterator_property_map(vertex_order.begin(),
                                        boost::identity_property_map(),
                                        vertex_order.size()),
      colored_map);

  auto name_map = boost::get(boost::vertex_name, graph);

  for (const auto& [v, color] : color_map_storage) {
    const auto& var = name_map[v];
    assert(var != nullptr);

    if (color < num_registers) {
      var->MetaRef(SymbolMetaKey::kIN_REGISTER) = true;
      var->MetaRef(SymbolMetaKey::kLOCATION) = registers[color];
    } else {
      var->MetaRef(SymbolMetaKey::kIS_SPILLED) = true;
      spilled_vars_.insert(var);
    }
  }

  for (const auto& var : mcs_order_) {
    if (!var->MetaRef(SymbolMetaKey::kLOCATION).has_value()) {
      var->MetaRef(SymbolMetaKey::kIS_SPILLED) = true;
      spilled_vars_.insert(var);
    }
  }
}

void ASMGenerator::build_inf_graph() {
  vars_.clear();

  for (const auto& instr : ir2_) {
    for (const auto& sym : instr->Use()) {
      // no reg name yet
      // but have label
      if (sym && IsVariable(sym)) vars_.insert(sym);
    }
  }

  // insert all vars in to graph
  for (const auto& v : vars_) {
    inf_graph_.AddVariable(v);
  }

  // build inf graph
  for (const auto& i : ir2_) {
    if (i->dst && IsVariable(i->dst)) {
      for (const auto& live_var : i->LiveOut) {
        if (live_var != i->dst) {
          inf_graph_.AddEdge(i->dst, live_var);
        }
      }
    }
  }
}

void ASMGenerator::generate_gcc_asm(const std::string& path,
                                    const std::vector<std::string>& ins) {
  std::set<std::string> data_vars;

  // .data
  std::vector<std::string> lines;

  // We are on 64 bit platform!
  // always remember to access memory using 64 bit pointer
  lines.emplace_back(".code64");

  lines.emplace_back(".data");
  for (const auto& var : data_vars) {
    lines.emplace_back(var + ":" + std::string{r32_ ? ".int 0" : " .quad 0"});
  }
  for (const auto& line : generate_data_segment()) {
    lines.emplace_back(line);
  }

  // .text
  lines.emplace_back("\n.text");

  lines.emplace_back(".globl main");
  lines.emplace_back("main:");

  for (const auto& line : ins) {
    lines.emplace_back("    " + line);
  }

  std::ofstream f(path);
  for (const auto& line : lines) f << line << "\n";
  f.close();
}

void ASMGenerator::mcs() {
  using Graph = InterferenceGraph::Graph;
  const Graph& graph = inf_graph_.GetGraph();
  auto name_map = boost::get(boost::vertex_name, graph);

  std::vector<InterferenceGraph::Vertex> nodes;
  for (auto [v, v_end] = boost::vertices(graph); v != v_end; ++v) {
    nodes.push_back(*v);
  }

  std::unordered_map<InterferenceGraph::Vertex, int> weights;
  std::unordered_set<InterferenceGraph::Vertex> visited;
  std::vector<SymbolPtr> order;

  for (auto v : nodes) {
    weights[v] = 0;
  }

  while (visited.size() < nodes.size()) {
    std::optional<InterferenceGraph::Vertex> max_node;
    int max_weight = -1;

    for (auto v : nodes) {
      if (visited.count(v) == 0 && weights[v] > max_weight) {
        max_weight = weights[v];
        max_node = v;
      }
    }

    if (!max_node) break;

    visited.insert(*max_node);
    order.push_back(name_map[*max_node]);

    for (auto [adj, adj_end] = boost::adjacent_vertices(*max_node, graph);
         adj != adj_end; ++adj) {
      if (visited.count(*adj) == 0) {
        weights[*adj]++;
      }
    }
  }

  for (const auto& var : vars_) {
    auto v_opt = inf_graph_.GetVertexBySymbol(var);
    if (v_opt && visited.count(*v_opt) == 0) {
      order.push_back(var);
    }
  }

  mcs_order_ = order;
}

void ASMGenerator::PrintFinalIR(const std::string& path) {
  std::ofstream f(path);
  PrintInstructionA2s(f, ir_final_);
  f.close();
}

void ASMGenerator::optimums() {
  std::vector<IRInstructionA2Ptr> n_ir;

  IRInstructionA2Ptr l_ir = nullptr;
  for (const auto& ir : ir_final_) {
    const auto op = ir->op->Name();
    if (op == "mov" && ir->dst == ir->src) continue;

    if (op == "brz" && l_ir && l_ir->op->Value() == kCmpOpType &&
        l_ir->dst == ir->src) {
      n_ir.pop_back();
      n_ir.push_back(MakeIRA2(map_op("cmp"), l_ir->src, l_ir->src_2));

      const auto op = l_ir->op->Name();

      if (op == "eq") {
        n_ir.push_back(MakeIRA2(map_op("jne"), nullptr, ir->src_2));
      }
      if (op == "neq") {
        n_ir.push_back(MakeIRA2(map_op("je"), nullptr, ir->src_2));
      }
      if (op == "lt") {
        n_ir.push_back(MakeIRA2(map_op("jge"), nullptr, ir->src_2));
      }
      if (op == "le") {
        n_ir.push_back(MakeIRA2(map_op("jg"), nullptr, ir->src_2));
      }
      if (op == "gt") {
        n_ir.push_back(MakeIRA2(map_op("jle"), nullptr, ir->src_2));
      }
      if (op == "ge") {
        n_ir.push_back(MakeIRA2(map_op("jl"), nullptr, ir->src_2));
      }

      l_ir = ir;
      continue;
    }

    if (op == "brz" && l_ir && l_ir->op->Name() == "lnot" &&
        l_ir->dst == ir->src) {
      n_ir.pop_back();
      n_ir.push_back(MakeIRA2(map_op("brnz"), nullptr, l_ir->dst, ir->src_2));
      l_ir = ir;
      continue;
    }

    l_ir = ir;
    n_ir.push_back(ir);
  }

  ir_final_ = n_ir;
}

void ASMGenerator::handle_spling_var() {
  std::unordered_map<SymbolPtr, int> stack_offset;

  for (const auto& var : spilled_vars_) {
    stack_offset_ -= stack_offset_dt_;
    stack_offset[var] = stack_offset_;
  }

  for (const auto& var : spilled_vars_) {
    var->MetaRef(SymbolMetaKey::kIN_STACK) = true;
    var->MetaRef(SymbolMetaKey::kLOCATION) =
        std::to_string(stack_offset[var]) + "(" + bp_ + ")";
  }

  ir_final_ = ir2_;
}

auto ASMGenerator::alloc_stack_for_immediate(const SymbolPtr& val)
    -> std::string {
  auto loc = SymLoc(val);
  if (!loc.empty()) return loc;

  stack_offset_ -= stack_offset_dt_;
  loc = std::to_string(stack_offset_) + +"(" + bp_ + ")";
  val->MetaRef(SymbolMetaKey::kLOCATION) = loc;
  return loc;
}

auto ASMGenerator::gen_data_var_immediate_label(const SymbolPtr& val)
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

auto ASMGenerator::alloc_stack_memory() -> std::vector<std::string> {
  std::vector<std::string> ret;

  ret.push_back("push " + bp_);
  ret.push_back("mov " + sp_ + ", " + bp_);

  if (std::abs(stack_offset_) != 0) {
    ret.push_back("sub $" + std::to_string(std::abs(stack_offset_)) + ", " +
                  sp_);
  }

  return ret;
}

auto ASMGenerator::map_sym(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kAST, name, type, true);
}

auto ASMGenerator::map_reg(const std::string& name) -> SymbolPtr {
  auto sym = symbol_table_->AddSymbol(SymbolType::kAST, name, "register", true);
  MetaSet(sym, SymbolMetaKey::kIN_REGISTER, true);
  return sym;
}

auto ASMGenerator::map_op(const std::string& name) -> SymbolPtr {
  auto sym = helper_.LookupSymbol(nullptr, name);
  assert(sym != nullptr);
  return sym;
}

void ASMGenerator::PrintIFG(const std::string& path) {
  std::ofstream f(path);
  inf_graph_.Print(f);
  f.close();
}

void ASMGenerator::gen_final_asm_source(const std::string& path) {
  auto main_ins = translate(ir_final_);

  // this function need stack size info
  // so run this after translate()
  auto stack_ins = alloc_stack_memory();

  std::vector<std::string> ins;
  ins.insert(ins.end(), stack_ins.begin(), stack_ins.end());
  ins.insert(ins.end(), main_ins.begin(), main_ins.end());

  generate_gcc_asm(path, ins);
}
