#include "ASMGenerator.h"

#include <spdlog/fmt/ranges.h>

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp>
#include <utility>

ASMGenerator::ASMGenerator(SymbolTablePtr symbol_table, bool r32,
                           const std::vector<IRInstructionA2>& ir)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kIR, symbol_table_),
      r32_(r32),
      ir_(ir) {}

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

auto IsImmediate(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->Value() == "immediate";
}

inline auto IsStackAccess(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  // "-4(%esp)" or "-8(%rsp)"
  return s->MetaData("in_stack").has_value();
}

inline auto IsReg(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return s->MetaData("in_register").has_value();
}

inline auto IsGlobalVar(const SymbolPtr& s) -> bool {
  return s != nullptr && !IsReg(s) && !IsImmediate(s) && !IsStackAccess(s);
}

inline auto IsInMemory(const SymbolPtr& s) -> bool {
  if (s == nullptr) return false;  // empty symbol
  return IsStackAccess(s) || IsGlobalVar(s);
}

auto SymLoc(const SymbolPtr& op) -> std::string {
  if (op == nullptr) return {};
  auto loc = op->MetaData("location");
  if (!loc.has_value()) return "";
  return std::any_cast<std::string>(loc);
}

}  // namespace

void ASMGenerator::translate(const IRInstructionA2& i) {
  std::string suffix = r32_ ? "l" : "q";
  std::string acc_reg = r32_ ? "%eax" : "%rax";
  std::string rem_reg = r32_ ? "%edx" : "%rdx";

  const auto s_src = i.src;
  const auto s_dst = i.dst;
  const auto src = format_operand(s_src);
  const auto dst = format_operand(s_dst);

  const auto op_mov = "mov" + suffix;

  auto op = i.op->Name();
  if (op == "mov") {
    bool src_mem = IsStackAccess(s_src) || IsGlobalVar(s_src);
    bool dst_mem = IsStackAccess(s_dst) || IsGlobalVar(s_dst);

    // [mem] -> [mem]
    if (src_mem && dst_mem) {
      asm_.push_back(op_mov + " " + src + ", " + acc_reg);
      asm_.push_back(op_mov + " " + acc_reg + ", " + dst);
    } else {
      asm_.push_back(op_mov + " " + src + ", " + dst);
    }
  } else if (op == "load") {
    asm_.push_back(op_mov + " " + src + ", " + dst);
  } else if (op == "store") {
    asm_.push_back(op_mov + " " + src + ", " + dst);
  } else if (op == "add") {
    emit_binary_op("add" + suffix, i);
  } else if (op == "sub") {
    emit_binary_op("sub" + suffix, i);
  } else if (op == "mul") {
    emit_binary_op("imul" + suffix, i);
  } else if (op == "div" || op == "mod") {
    asm_.push_back(op_mov + " " + dst + ", " + acc_reg);

    asm_.push_back(r32_ ? "cltd" : "cqto");
    if (IsImmediate(s_src)) {
      alloc_stack_for_immediate(s_src);

      asm_.push_back(op_mov + " " + src + ", " + SymLoc(s_src));
      asm_.push_back("idiv" + suffix + " " + SymLoc(s_src));
    } else {
      asm_.push_back("idiv" + suffix + " " + src);
    }

    if (op == "mod") {
      asm_.push_back(op_mov + " " + rem_reg + ", " + dst);
    } else {
      asm_.push_back(op_mov + " " + acc_reg + ", " + dst);
    }
  } else if (op == "neg") {
    asm_.push_back("neg" + suffix + " " + dst);
  } else if (op == "rtn") {
    asm_.push_back(op_mov + " " + src +
                   std::string{r32_ ? ", %eax" : ", %rax"});
    if (stack_offset_ != 0) asm_.push_back("leave");
    asm_.push_back("ret");
  } else {
    asm_.push_back("# unsupported op: " + op);
  }
}

void ASMGenerator::emit_binary_op(const std::string& asm_op,
                                  const IRInstructionA2& instr) {
  asm_.push_back(asm_op + " " + format_operand(instr.src) + ", " +
                 format_operand(instr.dst));
}

auto ASMGenerator::format_operand(const SymbolPtr& opr) -> std::string {
  if (opr == nullptr) return {};

  // is register
  if (opr->Value() == "register") return opr->Name();

  // have location
  auto loc = SymLoc(opr);
  if (!loc.empty()) return loc;

  // 1234 -1234 0x1234 0X1234
  if (IsImmediate(opr)) return "$" + opr->Name();

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

  std::set<SymbolPtr> spilled_vars;
  auto name_map = boost::get(boost::vertex_name, graph);

  for (const auto& [v, color] : color_map_storage) {
    const auto& var = name_map[v];
    assert(var != nullptr);

    if (color < num_registers) {
      var->RefMetaData("in_register") = true;
      var->RefMetaData("location") = registers[color];
    } else {
      var->RefMetaData("is_spilled_variable") = true;
      spilled_vars.insert(var);
    }
  }

  for (const auto& var : mcs_order_) {
    if (!var->RefMetaData("location").has_value()) {
      var->RefMetaData("is_spilled_variable") = true;
      spilled_vars.insert(var);
    }
  }

  ir_opt_ = handle_spling_var(spilled_vars);
}

void ASMGenerator::build_inf_graph() {
  std::set<SymbolPtr> live;
  std::set<SymbolPtr> all_vars;

  for (const auto& instr : ir_) {
    // no reg name yet
    if (instr.src && !IsImmediate(instr.src)) all_vars.insert(instr.src);
    if (instr.dst && !IsImmediate(instr.dst)) all_vars.insert(instr.dst);
  }

  for (int i = static_cast<int>(ir_.size()) - 1; i >= 0; --i) {
    const auto& ins = ir_[i];

    auto def = ins.dst;
    std::vector<SymbolPtr> use;

    if (ins.src && !IsImmediate(ins.src)) use.push_back(ins.src);

    if (ins.dst && ins.op->Name() != "mov" && !IsImmediate(ins.dst)) {
      use.push_back(ins.dst);
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

void ASMGenerator::PrintIR(const std::string& path) {
  std::ofstream f(path);
  for (const auto& i : ir_opt_) {
    f << std::left << std::setw(6) << i.op->Name();

    if (i.dst) {
      f << i.dst->Name();
      if (i.src) f << ", " << i.src->Name();
    } else if (i.src) {
      f << i.src->Name();
    }

    f << "\n";
  }
  f.close();
}

void ASMGenerator::optimums() {
  std::vector<IRInstructionA2> n_ir;

  for (const auto i : ir_opt_) {
    if (i.op->Name() == "mov" && i.dst == i.src) continue;
    n_ir.push_back(i);
  }

  ir_opt_ = n_ir;
}

auto ASMGenerator::handle_spling_var(const std::set<SymbolPtr>& spilled_vars)
    -> std::vector<IRInstructionA2> {
  std::unordered_map<SymbolPtr, int> stack_offset;
  for (const auto& var : spilled_vars) {
    stack_offset_ -= stack_offset_dt_;
    stack_offset[var] = stack_offset_;
  }

  for (const auto& var : spilled_vars) {
    var->RefMetaData("in_stack") = true;
    var->RefMetaData("location") =
        std::to_string(stack_offset[var]) + "(" + bp_ + ")";
  }

  std::vector<IRInstructionA2> n_ir;
  auto new_temp = [&]() -> SymbolPtr {
    stack_offset_ -= stack_offset_dt_;
    auto sym = symbol_table_->AddSymbol(
        SymbolType::kIR,
        "tmp_tack_stack_offset_" + std::to_string(std::abs(stack_offset_)), {},
        true);
    sym->RefMetaData("location") =
        std::to_string(stack_offset_) + +"(" + bp_ + ")";
    return sym;
  };

  auto tmp_backup_0 = new_temp();
  auto tmp_backup_1 = new_temp();

  auto reg_0 = r32_ ? map_reg("%edi") : map_reg("%r10");
  auto reg_1 = r32_ ? map_reg("%esi") : map_reg("%r11");

  const auto op_mov = helper_.LookupSymbol(nullptr, "mov");

  for (const auto& i : ir_) {
    bool sp_src =
        i.src && i.src->RefMetaData("is_spilled_variable").has_value();
    bool sp_dst =
        i.dst && i.dst->RefMetaData("is_spilled_variable").has_value();

    const auto op = i.op->Name();
    const auto src = i.src;
    const auto dst = i.dst;

    // mov mem -> mem
    if (op == "mov" && sp_src && sp_dst) {
      // reg_0 -> mem
      n_ir.emplace_back(op_mov, tmp_backup_0, reg_0);
      // src_mem -> reg_0
      n_ir.emplace_back(op_mov, reg_0, src);
      // sreg_ -> dst_mem
      n_ir.emplace_back(op_mov, dst, reg_0);
      n_ir.emplace_back(op_mov, reg_0, tmp_backup_0);
    }

    // op mem -> mem
    else if (op != "mov" && sp_src && sp_dst) {
      n_ir.emplace_back(op_mov, tmp_backup_0, reg_0);  // save %r10
      n_ir.emplace_back(op_mov, reg_0, src);

      n_ir.emplace_back(op_mov, tmp_backup_1, reg_1);  // save %r11
      n_ir.emplace_back(op_mov, reg_1, dst);

      n_ir.emplace_back(i.op, reg_1, reg_0);

      n_ir.emplace_back(op_mov, dst, reg_1);
      n_ir.emplace_back(op_mov, reg_0, tmp_backup_0);  // restore %r10
      n_ir.emplace_back(op_mov, reg_1, tmp_backup_1);  // restore %r11
    }

    else if (op != "mov" && sp_src) {
      if (reg_1->Name() == SymLoc(dst)) std::swap(reg_0, reg_1);

      n_ir.emplace_back(op_mov, tmp_backup_0, reg_1);  // save %r11
      n_ir.emplace_back(op_mov, reg_1, src);
      n_ir.emplace_back(i.op, dst, reg_1);
      n_ir.emplace_back(op_mov, reg_1, tmp_backup_0);  // restore %r11
    }

    else if (op != "mov" && sp_dst) {
      if (reg_1->Name() == SymLoc(src)) std::swap(reg_0, reg_1);

      //  reg_1 -> mem
      n_ir.emplace_back(op_mov, tmp_backup_0, reg_1);  // save %r11
      //  dst -> reg_1
      n_ir.emplace_back(op_mov, reg_1, dst);
      n_ir.emplace_back(i.op, reg_1, src);
      n_ir.emplace_back(op_mov, dst, reg_1);
      //  mem -> reg_1
      n_ir.emplace_back(op_mov, reg_1, tmp_backup_0);  // restore %r11
    }

    else {
      n_ir.emplace_back(i.op, dst, src);
    }
  }

  return n_ir;
}

void ASMGenerator::alloc_stack_for_immediate(const SymbolPtr& val) {
  // TODO(eric): make sure an immediate should only have one symbol
  if (SymLoc(val).empty()) {
    stack_offset_ -= stack_offset_dt_;
    val->RefMetaData("location") =
        std::to_string(stack_offset_) + +"(" + sp_ + ")";
  }
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

void ASMGenerator::alloc_stack_memory() {
  asm_.push_back("push " + bp_);
  asm_.push_back("mov " + sp_ + ", " + bp_);
  asm_.push_back("sub $" + std::to_string(std::abs(stack_offset_)) + ", " +
                 sp_);
}

auto ASMGenerator::map_sym(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kAST, name, type, true);
}

auto ASMGenerator::map_reg(const std::string& name) -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kAST, name, "register", true);
}
