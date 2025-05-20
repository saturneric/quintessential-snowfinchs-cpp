#include "ASMGenerator.h"

#include <spdlog/fmt/ranges.h>

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp>

ASMGenerator::ASMGenerator(bool r32, const std::vector<IRInstructionA2>& ir)
    : r32_(r32), ir_(ir) {}

void ASMGenerator::Generate(const std::string& path) {
  build_inf_graph();

  mcs();

  alloc_reg();

  optimums();

  asm_.clear();

  if (stack_offset_ != 0) alloc_stack_memory();

  for (const auto& i : ir_opt_) translate(i);

  generate_gcc_asm(path);
}

namespace {

auto IsImmediate(const std::string& s) -> bool {
  size_t n = s.size();

  size_t pos = 0;
  if (s[pos] == '-') ++pos;
  if (pos >= n) return false;

  // base 16
  if (n - pos > 2 && (s[pos] == '0') &&
      (s[pos + 1] == 'x' || s[pos + 1] == 'X')) {
    pos += 2;
    if (pos >= n) return false;
    for (; pos < n; ++pos) {
      char c = s[pos];
      if ((c < '0' || c > '9') && (c < 'a' || c > 'f') &&
          (c < 'A' || c > 'F')) {
        return false;
      }
    }
    return true;
  }

  // base 10
  for (; pos < n; ++pos) {
    if (s[pos] < '0' || s[pos] > '9') return false;
  }
  return true;
}

inline auto IsStackAccess(const std::string& op) -> bool {
  // "-4(%esp)" or "-8(%rsp)"
  return (!op.empty() && op.back() == ')') &&
         (op.find('%') != std::string::npos);
}

inline auto IsReg(const std::string& s) -> bool {
  if (s.empty()) return false;
  if (s.front() == '%') return true;
  return false;
}

inline auto IsGlobalVar(const std::string& op) -> bool {
  return !op.empty() && !IsReg(op) && !IsImmediate(op) && !IsStackAccess(op);
}

inline auto IsInMemory(const std::string& op) -> bool {
  return IsStackAccess(op) || IsGlobalVar(op);
}

}  // namespace

void ASMGenerator::translate(const IRInstructionA2& i) {
  std::string suffix = r32_ ? "l" : "q";
  std::string acc_reg = r32_ ? "%eax" : "%rax";
  std::string rem_reg = r32_ ? "%edx" : "%rdx";

  const auto src_v = i.arg2;
  const auto dst_v = i.arg1;
  const auto src = format_operand(i.arg2);
  const auto dst = format_operand(i.arg1);

  const auto op_mov = "mov" + suffix;

  if (i.op == "mov") {
    bool src_mem = IsStackAccess(src_v) || IsGlobalVar(src_v);
    bool dst_mem = IsStackAccess(dst_v) || IsGlobalVar(dst_v);

    // [mem] -> [mem]
    if (src_mem && dst_mem) {
      asm_.push_back(op_mov + " " + src + ", " + acc_reg);
      asm_.push_back(op_mov + " " + acc_reg + ", " + dst);
    } else {
      asm_.push_back(op_mov + " " + src + ", " + dst);
    }
  } else if (i.op == "load") {
    asm_.push_back(op_mov + " " + src + ", " + dst);
  } else if (i.op == "store") {
    asm_.push_back(op_mov + " " + src + ", " + dst);
  } else if (i.op == "add") {
    emit_binary_op("add" + suffix, i);
  } else if (i.op == "sub") {
    emit_binary_op("sub" + suffix, i);
  } else if (i.op == "mul") {
    emit_binary_op("imul" + suffix, i);
  } else if (i.op == "div" || i.op == "mod") {
    asm_.push_back(op_mov + " " + dst + ", " + acc_reg);

    asm_.push_back(r32_ ? "cltd" : "cqto");
    if (IsImmediate(src_v)) {
      auto stack_operand = format_operand(alloc_stack_for_immediate(i.arg2));

      asm_.push_back(op_mov + " " + format_operand(i.arg2) + ", " +
                     stack_operand);
      asm_.push_back("idiv" + suffix + " " + stack_operand);
    } else {
      asm_.push_back("idiv" + suffix + " " + src);
    }

    if (i.op == "mod") {
      asm_.push_back(op_mov + " " + rem_reg + ", " + dst);
    } else {
      asm_.push_back(op_mov + " " + acc_reg + ", " + dst);
    }
  } else if (i.op == "neg") {
    asm_.push_back("neg" + suffix + " " + dst);
  } else if (i.op == "rtn") {
    asm_.push_back(op_mov + " " + dst +
                   std::string{r32_ ? ", %eax" : ", %rax"});
    if (stack_offset_ != 0) asm_.push_back("leave");
    asm_.push_back("ret");
  } else {
    asm_.push_back("# unsupported op: " + i.op);
  }
}

void ASMGenerator::emit_binary_op(const std::string& asm_op,
                                  const IRInstructionA2& instr) {
  asm_.push_back(asm_op + " " + format_operand(instr.arg2) + ", " +
                 format_operand(instr.arg1));
}

auto ASMGenerator::format_operand(const std::string& operand) -> std::string {
  if (operand.empty() || IsReg(operand) || IsStackAccess(operand)) {
    return operand;
  }

  // 1234 -1234 0x1234 0X1234
  if (IsImmediate(operand)) return "$" + operand;

  // data var
  return r32_ ? operand : operand + "(%rip)";
}

auto ASMGenerator::generate_data_segment() const -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& c : constants_) {
    if (IsImmediate(c)) {
      ret.push_back(gen_data_var_immediate_label(c) +
                    std::string{r32_ ? ": .int " : ": .quad "} + c);
    } else {
      ret.push_back(c + std::string{r32_ ? ": .int 0" : ": .quad 0"});
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
  reg_alloc_.clear();

  auto registers = r32_ ? kRegisters32 : kRegisters64;
  const auto num_registers = registers.size();

  using Graph = InterferenceGraph::Graph;
  const Graph& graph = inf_graph_.GetGraph();

  std::map<InterferenceGraph::Vertex, decltype(registers.size())>
      color_map_storage;
  auto colored_map = boost::make_assoc_property_map(color_map_storage);

  std::vector<InterferenceGraph::Vertex> vertex_order;
  for (const auto& var : mcs_order_) {
    auto v_opt = inf_graph_.GetVertexByName(var);
    if (v_opt) vertex_order.push_back(*v_opt);
  }

  // greedy coloring using MCS order
  boost::sequential_vertex_coloring(
      graph,
      boost::make_iterator_property_map(vertex_order.begin(),
                                        boost::identity_property_map(),
                                        vertex_order.size()),
      colored_map);

  std::set<std::string> spilled_vars;
  auto name_map = boost::get(boost::vertex_name, graph);

  for (const auto& [v, color] : color_map_storage) {
    const auto& var = name_map[v];
    if (color < num_registers) {
      reg_alloc_[var] = registers[color];
    } else {
      spilled_vars.insert(var);
    }
  }

  for (const auto& var : mcs_order_) {
    if (reg_alloc_.count(var) == 0) {
      spilled_vars.insert(var);
    }
  }

  ir_opt_ = handle_spling_var(spilled_vars);
}

void ASMGenerator::build_inf_graph() {
  std::set<std::string> live;
  std::set<std::string> all_vars;

  for (const auto& instr : ir_) {
    // no reg name yet
    if (!IsImmediate(instr.arg1)) all_vars.insert(instr.arg1);
    if (!IsImmediate(instr.arg2)) all_vars.insert(instr.arg2);
  }

  for (int i = static_cast<int>(ir_.size()) - 1; i >= 0; --i) {
    const auto& instr = ir_[i];

    std::string def = instr.arg1;
    std::vector<std::string> use;

    if (!IsImmediate(instr.arg2)) use.push_back(instr.arg2);

    if (instr.op != "mov" && !IsImmediate(instr.arg1)) {
      use.push_back(instr.arg1);
    }

    if (!IsImmediate(def)) {
      for (const auto& v : live) {
        if (v != def) {
          inf_graph_.AddEdge(def, v);
        }
      }
    }

    live.erase(def);
    for (const auto& u : use) {
      live.insert(u);
    }
  }

  for (const auto& var : all_vars) {
    inf_graph_.AddVariable(var);
  }
}

void ASMGenerator::generate_gcc_asm(const std::string& path) {
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

  for (const auto& line : asm_) {
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
  std::vector<std::string> order;

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
    auto v_opt = inf_graph_.GetVertexByName(var);
    if (v_opt && visited.count(*v_opt) == 0) {
      order.push_back(var);
    }
  }

  spdlog::debug("MCS Order: {}", fmt::join(order, " "));
  mcs_order_ = order;
}

void ASMGenerator::PrintIR(const std::string& path) {
  std::ofstream f(path);
  for (const auto& i : ir_opt_) {
    f << std::left << std::setw(6) << i.op;

    if (!i.arg1.empty()) {
      f << i.arg1;
      if (!i.arg2.empty()) f << ", " << i.arg2;
    }

    f << "\n";
  }
  f.close();
}

void ASMGenerator::optimums() {
  std::vector<IRInstructionA2> n_ir;

  for (const auto i : ir_opt_) {
    if (i.op == "mov" && i.arg1 == i.arg2) continue;
    n_ir.push_back(i);
  }

  ir_opt_ = n_ir;
}

auto ASMGenerator::bounded_reg(const std::string& operand) -> std::string {
  // rtn or unop ...
  if (operand.empty()) return operand;

  if (IsReg(operand)) return operand;
  if (IsImmediate(operand)) return operand;

  auto it = reg_alloc_.find(operand);

  // not found is bug
  assert(it != reg_alloc_.end());

  if (it != reg_alloc_.end()) return it->second;

  throw std::runtime_error("Operand not found in register allocation: " +
                           operand);
}

auto ASMGenerator::handle_spling_var(const std::set<std::string>& spilled_vars)
    -> std::vector<IRInstructionA2> {
  std::unordered_map<std::string, int> stack_offset;
  for (const auto& var : spilled_vars) {
    stack_offset_ -= stack_offset_dt_;
    stack_offset[var] = stack_offset_;
  }

  for (const auto& var : spilled_vars) {
    reg_alloc_[var] = std::to_string(stack_offset[var]) + "(" + bp_ + ")";
  }

  std::vector<IRInstructionA2> n_ir;
  auto new_temp = [&]() -> std::string {
    stack_offset_ -= stack_offset_dt_;
    return std::to_string(stack_offset_) + +"(" + bp_ + ")";
  };

  std::string tmp_backup_0 = new_temp();
  std::string tmp_backup_1 = new_temp();

  std::string reg_0 = r32_ ? "%edi" : "%r10";
  std::string reg_1 = r32_ ? "%esi" : "%r11";

  for (const auto& i : ir_) {
    bool src_mem = !IsImmediate(i.arg2) && spilled_vars.count(i.arg2) != 0U;
    bool dst_mem = !IsImmediate(i.arg1) && spilled_vars.count(i.arg1) != 0U;

    const auto reg_src = bounded_reg(i.arg2);
    const auto reg_dst = bounded_reg(i.arg1);

    // mov mem -> mem
    if (i.op == "mov" && src_mem && dst_mem) {
      // reg_0 -> mem
      n_ir.push_back({"store", tmp_backup_0, reg_0});
      // src_mem -> reg_0
      n_ir.push_back({"load", reg_0, reg_src});
      // sreg_ -> dst_mem
      n_ir.push_back({"mov", reg_dst, reg_0});
      n_ir.push_back({"load", reg_0, tmp_backup_0});
    }

    // op mem -> mem
    else if (i.op != "mov" && src_mem && dst_mem) {
      n_ir.push_back({"store", tmp_backup_0, reg_0});  // save %r10
      n_ir.push_back({"load", reg_0, reg_src});

      n_ir.push_back({"store", tmp_backup_1, reg_1});  // save %r11
      n_ir.push_back({"load", reg_1, reg_dst});

      n_ir.push_back({i.op, reg_1, reg_0});

      n_ir.push_back({"store", reg_dst, reg_1});
      n_ir.push_back({"load", reg_0, tmp_backup_0});  // restore %r10
      n_ir.push_back({"load", reg_1, tmp_backup_1});  // restore %r11
    }

    else if (i.op != "mov" && src_mem) {
      if (reg_1 == reg_dst) std::swap(reg_0, reg_1);

      n_ir.push_back({"store", tmp_backup_0, reg_1});  // save %r11
      n_ir.push_back({"load", reg_1, reg_src});
      n_ir.push_back({i.op, reg_dst, reg_1});
      n_ir.push_back({"load", reg_1, tmp_backup_0});  // restore %r11
    }

    else if (i.op != "mov" && dst_mem) {
      if (reg_1 == reg_src) std::swap(reg_0, reg_1);

      //  reg_1 -> mem
      n_ir.push_back({"store", tmp_backup_0, reg_1});  // save %r11
      //  dst -> reg_1
      n_ir.push_back({"load", reg_1, reg_dst});
      n_ir.push_back({i.op, reg_1, reg_src});
      n_ir.push_back({"store", reg_dst, reg_1});
      //  mem -> reg_1
      n_ir.push_back({"load", reg_1, tmp_backup_0});  // restore %r11
    }

    else {
      n_ir.push_back({i.op, reg_dst, reg_src});
    }
  }

  return n_ir;
}

auto ASMGenerator::alloc_stack_for_immediate(const std::string& val)
    -> std::string {
  auto it = const_stack_slots_.find(val);
  if (it != const_stack_slots_.end()) {
    return std::to_string(it->second) + +"(" + sp_ + ")";
  }

  stack_offset_ -= stack_offset_dt_;
  const_stack_slots_[val] = stack_offset_;
  return std::to_string(stack_offset_) + +"(" + sp_ + ")";
}

auto ASMGenerator::gen_data_var_immediate_label(const std::string& val)
    -> std::string {
  std::string label = "const_";
  for (char ch : val) {
    if (ch == '-') {
      label += "n";
    } else {
      label += ch;
    }
  }
  return label;
}

void ASMGenerator::alloc_stack_memory() {
  asm_.push_back("push " + bp_);
  asm_.push_back("mov " + sp_ + ", " + bp_);
  asm_.push_back("sub $" + std::to_string(std::abs(stack_offset_)) + ", " +
                 sp_);
}
