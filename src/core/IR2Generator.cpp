#include "IR2Generator.h"

#include "model/ControlFlowGraphAlgos.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

IR2Generator::IR2Generator(SymbolTablePtr symbol_table,
                           std::vector<FuncInstructions> ir3)
    : ir3_(std::move(ir3)),
      symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kIR, symbol_table_) {}

auto IR2Generator::map_op(const std::string& name) -> SymbolPtr {
  return helper_.LookupSymbol(nullptr, name);
}

auto IR2Generator::map_sym(const std::string& name, const std::string& type)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kAST, name, type, true);
}

void IR2Generator::convert_ira3_2_ira2() {
  for (const auto& fi : ir3_) {
    std::vector<IRInstructionA2> res;
    for (const auto& i : fi.ins) {
      const auto& op = i->Op()->Name();
      const auto& op_type = i->Op()->Value();

      if (op_type == kBinOpType) {
        if (i->DST() != i->SRC(0)) {
          res.emplace_back(map_op("mov"), i->DST(), i->SRC(0));
        }
        res.emplace_back(i->Op(), i->DST(), i->SRC(1));
        continue;
      }

      if (op_type == kUnOpType) {
        if (i->DST() != i->SRC(0)) {
          res.emplace_back(map_op("mov"), i->DST(), i->SRC(0));
        }
        res.emplace_back(i->Op(), i->DST());
        continue;
      }

      if (op_type == kCmpOpType) {
        res.emplace_back(i->Op(), i->DST(), i->SRC(0));
        res.back().src_2 = i->SRC(1);
        continue;
      }

      if (op == "mov") {
        if (i->DST() != i->SRC(0))
          res.emplace_back(i->Op(), i->DST(), i->SRC(0));
        continue;
      }

      if (op == "rtn") {
        res.emplace_back(i->Op(), nullptr, i->SRC(0));
        continue;
      }

      if (op == "brz" || op == "brnz") {
        res.emplace_back(i->Op(), nullptr, i->SRC(0));
        res.back().src_2 = i->SRC(1);
        continue;
      }

      if (op == "jmp") {
        res.emplace_back(i->Op(), nullptr, i->SRC(0));
        continue;
      }

      if (op == "label") {
        res.emplace_back(i->Op(), nullptr, i->SRC(0));
        continue;
      }

      if (op == "call" || op == "arg") {
        res.emplace_back(i->Op(), i->DST(), i->SRC(0));
        continue;
      }

      if (op == "param") {
        res.emplace_back(i->Op(), nullptr, i->SRC(0));
        continue;
      }

      if (op == "dcl") continue;

      SPDLOG_ERROR("Unsupported op: {}", op);
    }

    ir2_.push_back({fi.func, {}});
    auto& ins = ir2_.back().ins;
    for (const auto& i : res) {
      ins.push_back(std::make_shared<IRInstructionA2>(i));
    }
  }
}

void IR2Generator::build_cfg() {
  for (const auto& fi : ir2_) {
    cfgs_.push_back({fi.func, std::make_shared<class ControlFlowGraph>()});

    auto& cfg = cfgs_.back().cfg;
    auto res = SplitToBasicBlocks(fi.ins, *cfg);
    BuildControlFlowEdges(*cfg, res.blocks, res.label2block);
    AnalyzeUseDefForBlocks(res.blocks);
    MarkReachableBlocks(*cfg);
  }
}

void IR2Generator::block_level_liveness_analyse() {
  for (const auto& fc : cfgs_) {
    BlockLevelLivenessAnalyse(*fc.cfg);
  }
}

void IR2Generator::instruction_level_liveness_analyse() {
  for (const auto& fc : cfgs_) {
    InstructionLevelLivenessAnalyse(*fc.cfg);
  }
}

void IR2Generator::Generate() {
  convert_ira3_2_ira2();
  build_cfg();
  block_level_liveness_analyse();
  instruction_level_liveness_analyse();
}

auto IR2Generator::ControlFlowGraph() -> std::vector<FuncCFG> { return cfgs_; }

void IR2Generator::PrintCFG(const std::string& path) {
  std::ofstream f(path);
  for (const auto& fc : cfgs_) {
    fc.cfg->Print(f);
  }
  f.close();
}

void IR2Generator::PrintAddr(const std::string& path) {
  if (ir2_.empty()) return;

  std::ofstream f(path);

  for (const auto& fc : ir2_) {
    PrintInstructions(f, fc.ins);
  }

  f.close();
}
