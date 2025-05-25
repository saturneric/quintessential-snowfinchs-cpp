#include "ControlFlowGraphAlgos.h"

#include <algorithm>
#include <queue>

#include "CFGBasicBlock.h"
#include "Utils.h"

auto SplitToBasicBlocks(const std::vector<IRInstructionPtr>& ir,
                        ControlFlowGraph& cfg) -> BasicBlockBuildResult {
  LabelMap label2block;
  int current_block_id = 0;
  auto new_block = [&]() -> CFGBasicBlockPtr {
    auto bb = std::make_shared<CFGBasicBlock>(current_block_id++);
    cfg.AddBlock(bb);
    return bb;
  };

  bool need_new_block = true;
  CFGBasicBlockPtr bb = nullptr;
  std::vector<CFGBasicBlockPtr> blocks;

  for (const auto& i : ir) {
    const auto& ins = i;
    auto opn = ins->Op()->Name();

    if (need_new_block && opn != "label") {
      bb = new_block();
      need_new_block = false;
      blocks.push_back(bb);
    }

    if (opn == "label") {
      bb = new_block();
      if (ins->SRC(0)) {
        bb->label = ins->SRC(0)->Name();
        label2block[bb->label] = bb->id;
      }
      need_new_block = false;
      blocks.push_back(bb);
    }

    if (!bb) {
      bb = new_block();
      blocks.push_back(bb);
    }

    bb->Instrs().push_back(i);

    if (IsJump(ins->Op())) need_new_block = true;
    if (opn == "rtn") {
      bb->has_return = true;
      bb->will_return = true;
      need_new_block = true;
    }
  }

  return {blocks, label2block};
}

void BuildControlFlowEdges(ControlFlowGraph& cfg,
                           const std::vector<CFGBasicBlockPtr>& blocks,
                           const LabelMap& label2block) {
  for (size_t idx = 0; idx < blocks.size(); ++idx) {
    const auto& bb = blocks[idx];
    if (bb->Instrs().empty()) continue;
    const auto& last_ins = bb->Instrs().back();

    auto jump_label = [&](const SymbolPtr& label) -> int {
      assert(label != nullptr);
      auto it = label2block.find(label->Name());
      if (it != label2block.end()) return it->second;
      return -1;
    };

    if (bb->has_return) continue;

    auto op = last_ins->Op()->Name();
    if (op == "jmp") {
      int succ_id = jump_label(last_ins->SRC(0));
      if (succ_id != -1) cfg.AddEdge(bb->id, succ_id);
    } else if (IsCondJump(last_ins->Op())) {
      int succ1 = jump_label(last_ins->SRC((0)) && op != "brz" && op != "brnz"
                                 ? last_ins->SRC(0)
                                 : last_ins->SRC(1));
      if (succ1 != -1) cfg.AddEdge(bb->id, succ1);
      if (idx + 1 < blocks.size()) cfg.AddEdge(bb->id, blocks[idx + 1]->id);
    } else {
      if (idx + 1 < blocks.size()) cfg.AddEdge(bb->id, blocks[idx + 1]->id);
    }
  }
}

void AnalyzeUseDefForBlocks(std::vector<CFGBasicBlockPtr>& blocks) {
  for (auto& block : blocks) {
    std::set<SymbolPtr> use;
    std::set<SymbolPtr> def;
    for (const auto& ins : block->Instrs()) {
      if (ins->Op()->Name() == "label") continue;
      for (auto& s : ins->Use()) {
        if (!IsVariable(s)) continue;
        if (def.count(s) == 0) use.insert(s);
      }
      if (ins->DST() && IsVariable(ins->DST())) def.insert(ins->DST());
    }
    block->use = std::move(use);
    block->def = std::move(def);
    block->live_in.clear();
    block->live_out.clear();
  }
}

void MarkReachableBlocks(ControlFlowGraph& cfg, int entry_id) {
  std::queue<CFGBasicBlockPtr> q;
  auto entry = cfg.BlockByBlockId(entry_id);
  entry->reachable = true;
  q.push(entry);

  while (!q.empty()) {
    auto bb = q.front();
    q.pop();
    for (const auto& succ : cfg.Successors(bb->id)) {
      if (!succ->reachable) {
        succ->reachable = true;
        q.push(succ);
      }
    }
  }
}

void BlockLevelLivenessAnalyse(ControlFlowGraph& cfg) {
  auto block_list = cfg.Blocks();

  bool changed = true;
  while (changed) {
    changed = false;

    std::map<int, std::set<SymbolPtr>> next_in;
    std::map<int, std::set<SymbolPtr>> next_out;

    for (auto& block : block_list) {
      auto& use = block->use;
      auto& def = block->def;

      // new_out = ⋃ IN[succ]
      std::set<SymbolPtr> new_out;
      for (auto& succ : cfg.Successors(block->id)) {
        new_out.insert(succ->live_in.begin(), succ->live_in.end());
      }

      // new_in = use ∪ (new_out – def)
      std::set<SymbolPtr> new_in = use;
      std::set<SymbolPtr> diff;
      std::set_difference(new_out.begin(), new_out.end(), def.begin(),
                          def.end(), std::inserter(diff, diff.end()));
      new_in.insert(diff.begin(), diff.end());

      next_in[block->id] = std::move(new_in);
      next_out[block->id] = std::move(new_out);
    }

    for (auto& block : block_list) {
      if (block->live_in != next_in[block->id] ||
          block->live_out != next_out[block->id]) {
        changed = true;
        block->live_in = std::move(next_in[block->id]);
        block->live_out = std::move(next_out[block->id]);
      }
    }
  }
}

void BlockLevelDefAnalyse(ControlFlowGraph& cfg) {
  cfg.BuildDominatorTree();

  std::queue<CFGBasicBlockPtr> worklist;
  auto blocks = cfg.Blocks();

  for (auto& bb : blocks) {
    bb->def_out = bb->def;
    worklist.push(bb);
  }

  while (!worklist.empty()) {
    auto bb = worklist.front();
    worklist.pop();

    if (!bb->reachable) continue;

    std::vector<CFGBasicBlockPtr> real_preds;
    auto bb_v = cfg.VertexByBlockId(bb->id);
    for (auto& p : cfg.Predecessors(bb->id)) {
      if (!p->reachable) continue;
      auto p_v = cfg.VertexByBlockId(p->id);
      if (!cfg.Dominates(p_v, bb_v)) {
        real_preds.push_back(p);
      }
    }

    std::set<SymbolPtr> new_in;
    if (real_preds.empty()) {
      new_in = bb->def;
    } else {
      new_in = cfg.BlockByBlockId(real_preds[0]->id)->def_out;
      for (size_t i = 1; i < real_preds.size(); ++i) {
        std::set<SymbolPtr> tmp;
        auto& other_out = cfg.BlockByBlockId(real_preds[i]->id)->def_out;
        std::set_intersection(new_in.begin(), new_in.end(), other_out.begin(),
                              other_out.end(), std::inserter(tmp, tmp.begin()));
        new_in.swap(tmp);
      }
    }

    std::set<SymbolPtr> new_out = new_in;
    new_out.insert(bb->def.begin(), bb->def.end());

    if (new_in != bb->def_in || new_out != bb->def_out) {
      bb->def_in = std::move(new_in);
      bb->def_out = std::move(new_out);
      for (auto& succ : cfg.Successors(bb->id)) {
        worklist.push(cfg.BlockByBlockId(succ->id));
      }
    }
  }
}

void InstructionLevelLivenessAnalyse(ControlFlowGraph& cfg) {
  auto blocks = cfg.Blocks();

  std::unordered_map<int, std::vector<IRInstructionPtr>> block_instrs;
  for (auto& bb : blocks) {
    block_instrs[bb->id] = bb->Instrs();
  }

  bool changed = true;
  while (changed) {
    changed = false;

    for (auto& bb : blocks) {
      auto& idxs = block_instrs[bb->id];
      if (idxs.empty()) continue;

      // last ins out = block out
      auto last = idxs.back();
      auto old_out = last->LiveOut();
      last->LiveOut() = bb->live_out;
      if (last->LiveOut() != old_out) changed = true;

      // iterate downward
      for (int k = static_cast<int>(idxs.size()) - 1; k >= 0; --k) {
        const auto& i = idxs[k];
        // live_in = use ∪ (live_out - def)
        std::set<SymbolPtr> new_in;
        // use & def of ins
        std::vector<SymbolPtr> use;
        std::vector<SymbolPtr> def;
        // ins.Use() return dst/srcs
        for (auto& s : i->Use()) {
          if (IsVariable(s)) use.push_back(s);
        }

        if (i->DST() && IsVariable(i->DST())) def.push_back(i->DST());

        // out->in diff
        std::set<SymbolPtr> diff;
        std::set_difference(i->LiveOut().begin(), i->LiveOut().end(),
                            def.begin(), def.end(),
                            std::inserter(diff, diff.end()));
        // use ∪ diff
        new_in.insert(use.begin(), use.end());
        new_in.insert(diff.begin(), diff.end());

        if (new_in != i->LiveIn()) {
          i->LiveIn() = std::move(new_in);
          changed = true;
        }

        // live_out[i-1] = live_in[i]
        if (k > 0) {
          const auto& prev = idxs[k - 1];
          if (prev->LiveOut() != i->LiveIn()) {
            prev->LiveOut() = i->LiveIn();
            changed = true;
          }
        }
      }
    }
  }
}
