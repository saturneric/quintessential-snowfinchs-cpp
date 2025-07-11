#pragma once

#include "core/ScopedSymbolTable.h"
#include "core/SymbolTable.h"
#include "model/IRInstruction.h"
#include "model/InterferenceGraph.h"

using SymbolMapper =
    std::function<SymbolPtr(const std::string&, const std::string&)>;
using SymbolSearcher = std::function<SymbolPtr(const std::string&)>;

class Translator {
 public:
  virtual void HandleVariables(const std::set<SymbolPtr>& vars) = 0;

  virtual auto Optimums(const std::vector<IRInstructionPtr>& ir)
      -> std::vector<IRInstructionPtr> = 0;

  virtual auto GenerateTextSection(const std::vector<IRInstructionPtr>& ir,
                                   const std::set<std::string>& all_regs)
      -> std::vector<std::string> = 0;

  virtual auto GenerateDataSegment() -> std::vector<std::string> = 0;

  virtual void Reset() = 0;

  [[nodiscard]] virtual auto AvailableRegisters() const
      -> std::vector<std::string> = 0;

  void SetSymbolMapper(SymbolMapper mapper);

  void SetOpSearcher(SymbolSearcher searcher);

 protected:
  auto mapSym(const std::string& name, const std::string& value) -> SymbolPtr;

  auto mapReg(const std::string& name) -> SymbolPtr;

  auto mapOp(const std::string& name) -> SymbolPtr;

 private:
  SymbolMapper symbol_mapper_;
  SymbolSearcher op_searcher_;
};

using TranslatorPtr = std::shared_ptr<Translator>;

class ASMGenerator {
 public:
  explicit ASMGenerator(SymbolTablePtr symbol_table, TranslatorPtr translator,
                        const std::vector<FuncInstructions>& ir2);

  void Generate(const std::string& path);

  void PrintFinalIR(const std::string& path);

  void PrintIFG(const std::string& path);

 private:
  struct RegAllocState {
    std::string func;
    InterferenceGraph inf;
    std::set<std::string> al_regs;
    std::set<SymbolPtr> vars;
  };

  TranslatorPtr translator_;
  SymbolTablePtr symbol_table_;
  ScopedSymbolLookUpHelper helper_;
  std::vector<RegAllocState> states_;
  std::vector<FuncInstructions> ir2_;
  std::vector<FuncInstructions> ir2_final_;

  void gen_final_asm_source(const std::string& path);

  auto alloc_register(InterferenceGraph& inf_graph) -> std::set<std::string>;

  void build_inf_graph(InterferenceGraph& inf_graph, const FuncInstructions& fi,
                       RegAllocState& ras);

  auto map_sym(const std::string& name, const std::string& type) -> SymbolPtr;

  auto generate_text_section(const std::vector<IRInstructionPtr>& irs,
                             const std::set<std::string>& all_regs)
      -> std::vector<std::string>;
};