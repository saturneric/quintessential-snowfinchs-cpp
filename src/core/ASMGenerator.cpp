#include "ASMGenerator.h"

#include <boost/graph/sequential_vertex_coloring.hpp>
#include <regex>

#include "model/InterferenceGraphAlgos.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

ASMGenerator::ASMGenerator(SymbolTablePtr symbol_table,
                           TranslatorPtr translator,
                           const std::vector<FuncInstructions>& ir2)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kIR, symbol_table_),
      translator_(std::move(translator)),
      ir2_(ir2) {
  assert(translator_ != nullptr);

  // inject dependencies
  translator_->SetSymbolMapper(
      [=](const std::string& name, const std::string& value) {
        return this->map_sym(name, value);
      });
  // inject dependencies
  translator_->SetOpSearcher([=](const std::string& name) {
    return helper_.LookupSymbol(nullptr, name);
  });
}

void ASMGenerator::Generate(const std::string& path) {
  spdlog::stopwatch sw;

  for (const auto& fi : ir2_) {
    states_.push_back(RegAllocState{fi.func});
    auto& state = states_.back();

    build_inf_graph(state.inf, fi, state);
    spdlog::debug("Built Inf Graph: {:.3}s", sw);

    state.al_regs = alloc_register(state.inf);
    spdlog::debug("Allocated Register: {:.3}s", sw);

    ir2_final_.push_back(
        FuncInstructions{fi.func, translator_->Optimums(fi.ins)});
    spdlog::debug("Translator Optimums: {:.3}s", sw);
  }

  gen_final_asm_source(path);
  spdlog::debug("Generated ASM Source Code: {:.3}s", sw);
}

auto ASMGenerator::alloc_register(InterferenceGraph& inf_graph)
    -> std::set<std::string> {
  std::set<std::string> allocated_registers;
  auto registers = translator_->AvailableRegisters();
  auto result = GraphColorRegisterAlloc(inf_graph, registers);

  for (const auto& [var, reg] : result.reg_assignment) {
    var->MetaRef(SymbolMetaKey::kIN_REGISTER) = true;
    var->MetaRef(SymbolMetaKey::kLOCATION) = reg;
    allocated_registers.insert(reg);
  }

  for (const auto& var : result.spilled) {
    if (!var->MetaRef(SymbolMetaKey::kLOCATION).has_value()) {
      var->MetaRef(SymbolMetaKey::kIS_SPILLED) = true;
    }
  }
  return allocated_registers;
}

void ASMGenerator::build_inf_graph(InterferenceGraph& inf_graph,
                                   const FuncInstructions& fi,
                                   RegAllocState& ras) {
  for (const auto& instr : fi.ins) {
    for (const auto& sym : instr->Use()) {
      // no reg name yet
      // but have label
      if (sym && IsVariable(sym)) ras.vars.insert(sym);
    }
  }

  // insert all vars in to graph
  for (const auto& v : ras.vars) {
    inf_graph.AddVariable(v);
  }

  // build inf graph
  for (const auto& i : fi.ins) {
    if (i->DST() && IsVariable(i->DST())) {
      for (const auto& live_var : i->LiveOut()) {
        if (live_var != i->DST()) {
          inf_graph.AddEdge(i->DST(), live_var);
        }
      }
    }
  }
}

auto ASMGenerator::map_sym(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kIR, name, type, true);
}

void ASMGenerator::PrintIFG(const std::string& path) {
  std::ofstream f(path);

  for (const auto& s : states_) {
    s.inf.Print(f);
  }

  f.close();
}

void ASMGenerator::gen_final_asm_source(const std::string& path) {
  if (!translator_) return;

  // .data
  std::vector<std::string> lines;

  // We are on 64 bit platform!
  // always remember to access memory using 64 bit pointer
  lines.emplace_back(".code64");

  lines.emplace_back(".data");

  for (const auto& line : translator_->GenerateDataSegment()) {
    lines.emplace_back(line);
  }

  // .text
  lines.emplace_back("\n.text");
  lines.emplace_back(".globl main");

  int index = 0;
  for (const auto& fi : ir2_final_) {
    assert(fi.func == states_[index].func);
    translator_->HandleVariables(states_[index].vars);
    auto l = generate_text_section(fi.ins, states_[index++].al_regs);
    lines.insert(lines.end(), l.begin(), l.end());
  }

  std::ofstream f(path);
  for (const auto& line : lines) f << line << "\n";
  f.close();
}

auto Translator::mapSym(const std::string& name, const std::string& value)
    -> SymbolPtr {
  assert(symbol_mapper_);
  return symbol_mapper_(name, value);
}

auto Translator::mapReg(const std::string& name) -> SymbolPtr {
  auto sym = mapSym(name, "register");
  MetaSet(sym, SymbolMetaKey::kIN_REGISTER, true);
  return sym;
}

auto Translator::mapOp(const std::string& name) -> SymbolPtr {
  assert(symbol_mapper_);
  auto sym = op_searcher_(name);
  assert(sym != nullptr);
  if (sym == nullptr) SPDLOG_ERROR("op {} is not supported", name);
  return sym;
}

void Translator::SetSymbolMapper(SymbolMapper mapper) {
  symbol_mapper_ = std::move(mapper);
}

void Translator::SetOpSearcher(SymbolSearcher searcher) {
  op_searcher_ = std::move(searcher);
}

void ASMGenerator::PrintFinalIR(const std::string& path) {
  std::ofstream f(path);
  for (const auto& fi : ir2_final_) PrintInstructions(f, fi.ins);
  f.close();
}

auto ASMGenerator::generate_text_section(
    const std::vector<IRInstructionPtr>& irs,
    const std::set<std::string>& all_regs) -> std::vector<std::string> {
  static const std::regex kLabelRe(R"(^[A-Za-z_]\w*:$)");

  std::vector<std::string> lines;
  for (const auto& l : translator_->GenerateTextSection(irs, all_regs)) {
    auto s = l;
    s.erase(0, s.find_first_not_of(' '));
    s.erase(s.find_last_not_of(' ') + 1);

    if (std::regex_match(s, kLabelRe)) {
      lines.emplace_back(s);
    } else {
      lines.emplace_back("    " + s);
    }
  }
  return lines;
}
