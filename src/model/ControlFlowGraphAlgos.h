#pragma once

#include "model/ControlFlowGraph.h"

using LabelMap = std::map<std::string, int>;

struct BasicBlockBuildResult {
  std::vector<CFGBasicBlockPtr> blocks;
  LabelMap label2block;
};

auto SplitToBasicBlocks(const std::vector<IRInstructionPtr>& ir,
                        ControlFlowGraph& cfg) -> BasicBlockBuildResult;

void BuildControlFlowEdges(ControlFlowGraph& cfg,
                           const std::vector<CFGBasicBlockPtr>& blocks,
                           const LabelMap& label2block);

void AnalyzeUseDefForBlocks(std::vector<CFGBasicBlockPtr>& blocks);

void MarkReachableBlocks(ControlFlowGraph& cfg, int entry_id = 0);

void BlockLevelLivenessAnalyse(ControlFlowGraph& cfg);

void BlockLevelDefAnalyse(ControlFlowGraph& cfg);

void InstructionLevelLivenessAnalyse(ControlFlowGraph& cfg);
