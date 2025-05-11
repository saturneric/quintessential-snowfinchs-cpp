#include "ASMGenerator.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <set>

ASMGenerator::ASMGenerator(const std::vector<IRInstructionA2>& ir) : ir_(ir) {}

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
  if (i.op == "mov") {
    asm_.push_back("movq " + format_operand(i.arg2) + ", " +
                   format_operand(i.arg1));
  } else if (i.op == "load") {
    constants_.insert(i.arg2);
    asm_.push_back("movq " + format_operand(i.arg2) + ", " +
                   format_operand(i.arg1));
  } else if (i.op == "store") {
    constants_.insert(i.arg1);
    asm_.push_back("movq " + format_operand(i.arg2) + ", " +
                   format_operand(i.arg1));
  } else if (i.op == "add") {
    emit_binary_op("addq", i);
  } else if (i.op == "sub") {
    emit_binary_op("subq", i);
  } else if (i.op == "mul") {
    emit_binary_op("imulq", i);
  } else if (i.op == "div") {
    asm_.push_back("movq " + format_operand(i.arg1) + ", %rax");
    asm_.push_back("cqto");
    if (!is_variable(i.arg2)) {
      std::string const_label = "const_" + i.arg2;
      constants_.insert(i.arg2);
      asm_.push_back("idivq " + format_operand(const_label));
    } else {
      asm_.push_back("idivq " + format_operand(i.arg2));
    }
    asm_.push_back("movq %rax, " + format_operand(i.arg1));
  } else if (i.op == "neg") {
    asm_.push_back("negq " + format_operand(i.arg1));
  } else if (i.op == "rtn") {
    asm_.push_back("movq " + format_operand(i.arg1) + ", %rax");
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
  return operand + "(%rip)";
}

auto ASMGenerator::is_variable(const std::string& s) -> bool {
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
    ret.emplace_back("const_" + c + ": .quad " + c);
  }
  return ret;
}

void ASMGenerator::alloc_reg() {
  const std::vector<std::string> registers = {
      "%rbx", "%rcx", "%rsi", "%rdi", "%r8",  "%r9",
      "%r10", "%r11", "%r12", "%r13", "%r14", "%r15",
  };

  reg_alloc_.clear();

  std::cout << "[MCS Order]: ";
  for (const auto& var : mcs_order_) {
    std::cout << var << " ";
  }
  std::cout << '\n';

  std::set<std::string> spilled_vars;

  for (const auto& var : mcs_order_) {
    std::set<std::string> used_regs;
    for (const auto& neighbor : inf_graph_.Neighbors(var)) {
      if (reg_alloc_.count(neighbor) != 0) {
        used_regs.insert(reg_alloc_[neighbor]);
      }
    }

    for (const auto& reg : registers) {
      if (used_regs.count(reg) == 0) {
        reg_alloc_[var] = reg;
        break;
      }
    }

    if (reg_alloc_.count(var) == 0) {
      reg_alloc_[var] = "spill_" + var;
      spilled_vars.insert(var);
    }
  }

  for (const auto& [var, reg] : reg_alloc_) {
    std::cout << "Variable: " << var << " => Register: " << reg << "\n";
  }

  std::vector<IRInstructionA2> n_ir;

  int temp_counter = 0;
  auto new_temp = [&]() {
    return "spill_tmp" + std::to_string(temp_counter++);
  };

  for (const auto& i : ir_) {
    bool use_spilled = is_variable(i.arg2) && spilled_vars.count(i.arg2) != 0U;
    bool def_spilled = is_variable(i.arg1) && spilled_vars.count(i.arg1) != 0U;

    if (use_spilled && def_spilled) {
      std::string tmp_backup_0 = new_temp();
      n_ir.push_back({"store", tmp_backup_0, "%r10"});  // save %r10
      n_ir.push_back({"load", "%r10", reg_alloc_[i.arg2]});

      std::string tmp_backup_1 = new_temp();
      n_ir.push_back({"store", tmp_backup_1, "%r11"});  // save %r11
      n_ir.push_back({"load", "%r11", reg_alloc_[i.arg1]});

      n_ir.push_back({i.op, "%r11", "%r10"});

      n_ir.push_back({"store", reg_alloc_[i.arg1], "%r11"});
      n_ir.push_back({"load", "%r10", tmp_backup_0});  // restore %r10
      n_ir.push_back({"load", "%r11", tmp_backup_1});  // restore %r11
    }

    else if (use_spilled) {
      std::string tmp_backup = new_temp();
      n_ir.push_back({"store", tmp_backup, "%r11"});  // save %r11
      n_ir.push_back({"load", "%r11", reg_alloc_[i.arg2]});
      n_ir.push_back({i.op, reg_alloc_[i.arg1], "%r11"});
      n_ir.push_back({"load", "%r11", tmp_backup});  // restore %r11
    }

    else if (def_spilled) {
      std::string tmp_backup = new_temp();
      n_ir.push_back({"store", tmp_backup, "%r11"});  // save %r11
      n_ir.push_back({"load", "%r11", reg_alloc_[i.arg1]});
      n_ir.push_back({i.op, "%r11", reg_alloc_[i.arg2]});
      n_ir.push_back({"store", reg_alloc_[i.arg1], "%r11"});
      n_ir.push_back({"load", "%r11", tmp_backup});  // restore %r11
    }

    else {
      n_ir.push_back({i.op, is_variable(i.arg1) ? reg_alloc_[i.arg1] : i.arg1,
                      is_variable(i.arg2) ? reg_alloc_[i.arg2] : i.arg2});
    }
  }

  ir_opt_ = n_ir;
}

void InterferenceGraph::AddVariable(const std::string& var) { adj_list_[var]; }

void InterferenceGraph::AddEdge(const std::string& var1,
                                const std::string& var2) {
  if (var1 == var2) return;
  adj_list_[var1].insert(var2);
  adj_list_[var2].insert(var1);
}

void InterferenceGraph::RemoveEdge(const std::string& var1,
                                   const std::string& var2) {
  if (var1 == var2) return;
  adj_list_[var1].erase(var2);
  adj_list_[var2].erase(var1);
}

auto InterferenceGraph::Neighbors(const std::string& var) const
    -> std::unordered_set<std::string> {
  auto it = adj_list_.find(var);
  return it != adj_list_.end() ? it->second : std::unordered_set<std::string>{};
}

auto InterferenceGraph::Variables() const -> std::vector<std::string> {
  std::vector<std::string> vars;
  vars.reserve(adj_list_.size());
  for (const auto& [key, _] : adj_list_) {
    vars.push_back(key);
  }
  return vars;
}

void InterferenceGraph::Print() const {
  for (const auto& [var, neighbors] : adj_list_) {
    std::cout << var << " inf: ";
    for (const auto& n : neighbors) std::cout << n << " ";
    std::cout << "\n";
  }
}

void ASMGenerator::build_inf_graph() {
  std::set<std::string> live;

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
}

void ASMGenerator::generate_gcc_asm(const std::string& path) {
  std::set<std::string> data_vars;

  // .data
  std::vector<std::string> lines;
  lines.emplace_back(".data");
  for (const auto& var : data_vars) {
    lines.emplace_back(var + ":" + " .quad 0");
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
  const auto& graph = inf_graph_;
  auto nodes = graph.Variables();
  std::unordered_map<std::string, int> weights;
  std::unordered_set<std::string> visited;
  std::vector<std::string> order;

  for (const auto& node : nodes) {
    weights[node] = 0;
  }

  while (visited.size() < nodes.size()) {
    std::string max_node;
    int max_weight = -1;

    for (const auto& node : nodes) {
      if (visited.count(node) == 0 && weights[node] > max_weight) {
        max_weight = weights[node];
        max_node = node;
      }
    }

    if (max_node.empty()) break;

    visited.insert(max_node);
    order.push_back(max_node);

    for (const auto& neighbor : graph.Neighbors(max_node)) {
      if (visited.count(neighbor) == 0) {
        weights[neighbor]++;
      }
    }
  }

  for (const auto& var : vars_) {
    if (visited.count(var) == 0) {
      order.push_back(var);
    }
  }

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
