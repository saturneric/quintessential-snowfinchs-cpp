#include "IR2Generator.h"

#include "model/ControlFlowGraphAlgos.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

IR2Generator::IR2Generator(SymbolTablePtr symbol_table,
                           std::vector<IRInstructionPtr> ir3)
    : cfg_(std::make_shared<class ControlFlowGraph>()),
      ir3_(std::move(ir3)),
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
  std::vector<IRInstructionA2> res;

  for (const auto& i : ir3_) {
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
      if (i->DST() != i->SRC(0)) res.emplace_back(i->Op(), i->DST(), i->SRC(0));
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

    SPDLOG_ERROR("Unsupported op: {}", op);
  }

  for (const auto& i : res) {
    ir2_.push_back(std::make_shared<IRInstructionA2>(i));
  }
}

void IR2Generator::build_cfg() {
  std::vector<IRInstructionPtr> ir;

  for (const auto i : ir2_) {
    ir.push_back(i);
  }

  auto res = SplitToBasicBlocks(ir, *cfg_);
  BuildControlFlowEdges(*cfg_, res.blocks, res.label2block);
  AnalyzeUseDefForBlocks(res.blocks);
  MarkReachableBlocks(*cfg_);
}

void IR2Generator::block_level_liveness_analyse() {
  BlockLevelLivenessAnalyse(*cfg_);
}

void IR2Generator::instruction_level_liveness_analyse() {
  InstructionLevelLivenessAnalyse(*cfg_);
}

void IR2Generator::Generate() {
  convert_ira3_2_ira2();
  build_cfg();
  block_level_liveness_analyse();
  instruction_level_liveness_analyse();
}

auto IR2Generator::ControlFlowGraph() -> ControlFlowGraphPtr { return cfg_; }

void IR2Generator::PrintCFG(const std::string& path) {
  std::ofstream f(path);
  cfg_->Print(f);
  f.close();
}

void IR2Generator::PrintAddr(const std::string& path) {
  if (ir2_.empty()) return;

  std::ofstream f(path);

  PrintInstructions(f, ir2_);
  f.close();
}
