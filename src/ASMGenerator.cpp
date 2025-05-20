#include "ASMGenerator.h"

#include <spdlog/fmt/ranges.h>

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp>

const std::string kSpillPrefix = "sp_";

ASMGenerator::ASMGenerator(bool r32, const std::vector<IRInstructionA2>& ir)
    : r32_(r32), ir_(ir) {}

void ASMGenerator::Generate(const std::string& path) {
  all_vars();

  build_inf_graph();

  mcs();

  alloc_reg();

  optimums();

  asm_.clear();
  for (const auto& i : ir_opt_) translate(i);

  generate_gcc_asm(path);
}

static auto is_immediate(const std::string& s) -> bool {
  static const std::regex kImmRegex(R"(^\$\-?(0x[0-9a-fA-F]+|\d+)$)");
  return std::regex_match(s, kImmRegex);
}

void ASMGenerator::translate(const IRInstructionA2& i) {
  std::string suffix = r32_ ? "l" : "q";
  std::string acc_reg = r32_ ? "%eax" : "%rax";
  std::string rem_reg = r32_ ? "%edx" : "%rdx";

  if (i.op == "mov") {
    if (r32_ && !is_variable(i.arg2) && !is_reg(i.arg1)) {
      asm_.push_back("mov" + suffix + " " + format_operand(i.arg2) + ", " +
                     acc_reg);
      asm_.push_back("mov" + suffix + " " + acc_reg + ", " +
                     format_operand(i.arg1));
    } else {
      asm_.push_back("mov" + suffix + " " + format_operand(i.arg2) + ", " +
                     format_operand(i.arg1));
    }

  } else if (i.op == "load") {
    constants_.insert(i.arg2);
    asm_.push_back("mov" + suffix + " " + format_operand(i.arg2) + ", " +
                   format_operand(i.arg1));
  } else if (i.op == "store") {
    constants_.insert(i.arg1);
    asm_.push_back("mov" + suffix + " " + format_operand(i.arg2) + ", " +
                   format_operand(i.arg1));
  } else if (i.op == "add") {
    emit_binary_op("add" + suffix, i);
  } else if (i.op == "sub") {
    emit_binary_op("sub" + suffix, i);
  } else if (i.op == "mul") {
    emit_binary_op("imul" + suffix, i);
  } else if (i.op == "div" || i.op == "mod") {
    asm_.push_back("mov" + suffix + " " + format_operand(i.arg1) + ", " +
                   acc_reg);

    asm_.push_back(r32_ ? "cltd" : "cqto");
    if (!is_variable(i.arg2)) {
      constants_.insert(i.arg2);
      asm_.push_back("idiv" + suffix + " " +
                     format_operand(gen_safe_var_label(i.arg2)));
    } else {
      asm_.push_back("idiv" + suffix + " " + format_operand(i.arg2));
    }

    if (i.op == "mod") {
      asm_.push_back("mov" + suffix + " " + rem_reg + ", " +
                     format_operand(i.arg1));
    } else {
      asm_.push_back("mov" + suffix + " " + acc_reg + ", " +
                     format_operand(i.arg1));
    }
  } else if (i.op == "neg") {
    asm_.push_back("neg" + suffix + " " + format_operand(i.arg1));
  } else if (i.op == "rtn") {
    asm_.push_back("mov" + suffix + " " + format_operand(i.arg1) +
                   std::string{r32_ ? ", %eax" : ", %rax"});
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
  if (operand.empty() || is_reg(operand)) return operand;
  if (std::isdigit(operand[0]) || operand[0] == '-') {
    return "$" + operand;
  }
  return r32_ ? operand : operand + "(%rip)";
}

auto ASMGenerator::is_variable(const std::string& s) const -> bool {
  if (s.empty()) return false;
  if (std::isdigit(s[0]) || s[0] == '-') return false;
  return true;
}

auto ASMGenerator::is_reg(const std::string& s) -> bool {
  if (s.empty()) return false;
  if (s[0] == '%') return true;
  return false;
}

auto ASMGenerator::generate_data_segment() const -> std::vector<std::string> {
  std::vector<std::string> ret;

  for (const auto& c : constants_) {
    if (!is_variable(c)) {
      ret.push_back(gen_safe_var_label(c) +
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
      reg_alloc_[var] = kSpillPrefix + var;
      constants_.insert(reg_alloc_[var]);
      spilled_vars.insert(var);
    }
  }

  for (const auto& var : mcs_order_) {
    if (reg_alloc_.count(var) == 0) {
      reg_alloc_[var] = kSpillPrefix + var;
      constants_.insert(reg_alloc_[var]);
      spilled_vars.insert(var);
    }
  }

  ir_opt_ = handle_spling_var(spilled_vars);
}

void ASMGenerator::build_inf_graph() {
  std::set<std::string> live;
  std::set<std::string> all_vars;

  for (const auto& instr : ir_) {
    if (is_variable(instr.arg1)) all_vars.insert(instr.arg1);
    if (is_variable(instr.arg2)) all_vars.insert(instr.arg2);
  }

  for (int i = static_cast<int>(ir_.size()) - 1; i >= 0; --i) {
    const auto& instr = ir_[i];

    std::string def = instr.arg1;
    std::vector<std::string> use;

    if (is_variable(instr.arg2)) use.push_back(instr.arg2);

    if (instr.op != "mov" && is_variable(instr.arg1)) {
      use.push_back(instr.arg1);
    }

    if (is_variable(def)) {
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
  lines.emplace_back(".data");
  for (const auto& var : data_vars) {
    lines.emplace_back(var + ":" + std::string{r32_ ? ".int 0" : " .quad 0"});
  }
  for (const auto& line : generate_data_segment()) {
    lines.emplace_back(line);
  }

  // .text
  lines.emplace_back("\n.text");
  lines.emplace_back(".globl main\nmain:");

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

void ASMGenerator::all_vars() {
  for (const auto& i : ir_) {
    if (is_variable(i.arg1)) vars_.insert(i.arg1);
    if (is_variable(i.arg2)) vars_.insert(i.arg2);
  }
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
  if (operand.empty()) return operand;

  if (is_reg(operand)) return operand;
  if (!is_variable(operand)) return operand;

  auto it = reg_alloc_.find(operand);
  if (it != reg_alloc_.end()) return it->second;

  throw std::runtime_error("Operand not found in register allocation: " +
                           operand);
}

auto ASMGenerator::handle_spling_var(const std::set<std::string>& spilled_vars)
    -> std::vector<IRInstructionA2> {
  std::vector<IRInstructionA2> n_ir;
  int temp_counter = 0;
  auto new_temp = [&]() {
    return "spill_tmp" + std::to_string(temp_counter++);
  };

  std::string tmp_backup = new_temp();
  std::string tmp_backup_0 = new_temp();
  std::string tmp_backup_1 = new_temp();

  std::string reg_0 = r32_ ? "%edi" : "%r10";
  std::string reg_1 = r32_ ? "%esi" : "%r11";

  for (const auto& i : ir_) {
    bool use_spilled = is_variable(i.arg2) && spilled_vars.count(i.arg2) != 0U;
    bool def_spilled = is_variable(i.arg1) && spilled_vars.count(i.arg1) != 0U;

    if (i.op == "mov" && use_spilled && def_spilled) {
      n_ir.push_back({"store", tmp_backup, reg_0});
      n_ir.push_back({"load", reg_0, bounded_reg(i.arg2)});
      n_ir.push_back({"mov", bounded_reg(i.arg1), reg_0});
      n_ir.push_back({"load", reg_0, tmp_backup});
    }

    else if (i.op != "mov" && use_spilled && def_spilled) {
      n_ir.push_back({"store", tmp_backup_0, reg_0});  // save %r10
      n_ir.push_back({"load", reg_0, bounded_reg(i.arg2)});

      n_ir.push_back({"store", tmp_backup_1, reg_1});  // save %r11
      n_ir.push_back({"load", reg_1, bounded_reg(i.arg1)});

      n_ir.push_back({i.op, reg_1, reg_0});

      n_ir.push_back({"store", bounded_reg(i.arg1), reg_1});
      n_ir.push_back({"load", reg_0, tmp_backup_0});  // restore %r10
      n_ir.push_back({"load", reg_1, tmp_backup_1});  // restore %r11
    }

    else if (i.op != "mov" && use_spilled) {
      if (reg_1 == bounded_reg(i.arg1)) std::swap(reg_0, reg_1);

      n_ir.push_back({"store", tmp_backup, reg_1});  // save %r11
      n_ir.push_back({"load", reg_1, bounded_reg(i.arg2)});
      n_ir.push_back({i.op, bounded_reg(i.arg1), reg_1});
      n_ir.push_back({"load", reg_1, tmp_backup});  // restore %r11
    }

    else if (i.op != "mov" && def_spilled) {
      if (reg_1 == bounded_reg(i.arg2)) std::swap(reg_0, reg_1);

      n_ir.push_back({"store", tmp_backup, reg_1});  // save %r11
      n_ir.push_back({"load", reg_1, bounded_reg(i.arg1)});
      n_ir.push_back({i.op, reg_1, bounded_reg(i.arg2)});
      n_ir.push_back({"store", bounded_reg(i.arg1), reg_1});
      n_ir.push_back({"load", reg_1, tmp_backup});  // restore %r11
    }

    else {
      n_ir.push_back({i.op, bounded_reg(i.arg1), bounded_reg(i.arg2)});
    }
  }

  return n_ir;
}

auto ASMGenerator::gen_safe_var_label(const std::string& val) -> std::string {
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
