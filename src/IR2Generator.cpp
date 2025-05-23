#include "IR2Generator.h"

#include <queue>

#include "Utils.h"

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
  std::vector<CFGBasicBlockPtr> blocks;
  std::map<std::string, int> label2block;

  int current_block_id = 0;
  auto new_block = [&]() -> CFGBasicBlockPtr {
    auto bb = std::make_shared<CFGBasicBlock>(current_block_id++, "", true);
    blocks.push_back(bb);
    return bb;
  };

  bool need_new_block = true;
  CFGBasicBlockPtr bb = nullptr;
  auto ir = ir2_;

  for (const auto& i : ir) {
    const auto& ins = i;
    auto opn = ins->op->Name();

    // last ins as jmp
    if (need_new_block && opn != "label") {
      bb = new_block();
      need_new_block = false;
    }

    // label need jmp
    if (opn == "label") {
      bb = new_block();
      // label in ins.src not ins.dst
      if (ins->src) {
        bb->label = ins->src->Name();
        label2block[bb->label] = bb->id;
      }
      need_new_block = false;
    }

    // ensure bb for ins
    if (!bb) bb = new_block();

    bb->Instr2As().push_back(i);

    // jmps need new block
    if (IsJump(ins->op)) need_new_block = true;
  }

  std::map<int, Vertex> bbid2vertex;

  // add all block to cfg
  for (auto& bb : blocks) {
    bbid2vertex[bb->id] = cfg_->AddBlock(bb);
  }

  // add edge
  for (size_t idx = 0; idx < blocks.size(); ++idx) {
    auto& bb = blocks[idx];
    if (bb->Instr2As().empty()) continue;
    const auto& last_ins = bb->Instr2As().back();

    auto jump_label = [&](const SymbolPtr& label) -> int {
      auto it = label2block.find(label->Name());
      if (it != label2block.end()) return it->second;
      return -1;
    };

    auto op = last_ins->op->Name();
    if (op == "jmp") {
      // jmp
      int succ_id = jump_label(last_ins->src);
      if (succ_id != -1) cfg_->AddEdge(bb->id, succ_id);
    } else if (IsCondJump(last_ins->op)) {
      // cond jmp
      int succ1 = jump_label(last_ins->src && op != "brz" && op != "brnz"
                                 ? last_ins->src
                                 : last_ins->src_2);
      if (succ1 != -1) cfg_->AddEdge(bb->id, succ1);

      // fall through
      if (idx + 1 < blocks.size()) cfg_->AddEdge(bb->id, blocks[idx + 1]->id);
    } else {
      // fall through
      if (idx + 1 < blocks.size()) cfg_->AddEdge(bb->id, blocks[idx + 1]->id);
    }
  }

  // reachable analyse
  std::queue<CFGBasicBlockPtr> q;
  auto entry = cfg_->BlockByBlockId(0);
  entry->reachable = true;
  q.push(entry);

  while (!q.empty()) {
    auto bb = q.front();
    q.pop();
    for (const auto& succ : cfg_->Successors(bb->id)) {
      if (!succ->reachable) {
        succ->reachable = true;
        q.push(succ);
      }
    }
  }
}

void IR2Generator::block_level_liveness_analyse() {
  auto ir = cfg_->Instruction2As();
  auto block_list = cfg_->Blocks();

  for (auto& block : block_list) {
    std::set<SymbolPtr> use;
    std::set<SymbolPtr> def;
    for (const auto& ins : block->Instr2As()) {
      // skip label
      if (ins->op->Name() == "label") continue;

      // not def yet
      for (auto& s : ins->Use()) {
        if (!IsVariable(s)) continue;
        if (def.count(s) == 0) use.insert(s);
      }
      if (ins->dst && IsVariable(ins->dst)) def.insert(ins->dst);
    }
    block->use = use;
    block->def = def;
    block->live_in = {};
    block->live_out = {};
  }

  std::unordered_map<int, CFGBasicBlockPtr> id2block;
  for (auto& bb : block_list) {
    id2block[bb->id] = bb;
  }

  bool changed = true;
  while (changed) {
    changed = false;

    // temp store
    std::map<int, std::set<SymbolPtr>> next_in;
    std::map<int, std::set<SymbolPtr>> next_out;

    // calculate all new_in/new_out
    for (auto& block : block_list) {
      auto& use = block->use;
      auto& def = block->def;

      // new_out = ⋃ IN[succ]
      std::set<SymbolPtr> new_out;
      for (auto& succ : cfg_->Successors(block->id)) {
        auto succ_bb = id2block[succ->id];
        new_out.insert(succ_bb->live_in.begin(), succ_bb->live_in.end());
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

    // write out all
    for (auto& block : block_list) {
      if (block->live_in != next_in[block->id] ||
          block->live_out != next_out[block->id]) {
        changed = true;
        block->live_in = next_in[block->id];
        block->live_out = next_out[block->id];
      }
    }
  }
}

void IR2Generator::instruction_level_liveness_analyse() {
  auto blocks = cfg_->Blocks();

  std::unordered_map<int, std::vector<IRInstructionA2Ptr>> block_instrs;
  for (auto& bb : blocks) {
    block_instrs[bb->id] = bb->Instr2As();
  }

  bool changed = true;
  while (changed) {
    changed = false;

    for (auto& bb : blocks) {
      auto& idxs = block_instrs[bb->id];
      if (idxs.empty()) continue;

      // last ins out = block out
      auto last = idxs.back();
      auto old_out = last->LiveOut;
      last->LiveOut = bb->live_out;
      if (last->LiveOut != old_out) changed = true;

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

        if (i->dst && IsVariable(i->dst)) def.push_back(i->dst);

        // out->in diff
        std::set<SymbolPtr> diff;
        std::set_difference(i->LiveOut.begin(), i->LiveOut.end(), def.begin(),
                            def.end(), std::inserter(diff, diff.end()));
        // use ∪ diff
        new_in.insert(use.begin(), use.end());
        new_in.insert(diff.begin(), diff.end());

        if (new_in != i->LiveIn) {
          i->LiveIn = std::move(new_in);
          changed = true;
        }

        // live_out[i-1] = live_in[i]
        if (k > 0) {
          const auto& prev = idxs[k - 1];
          if (prev->LiveOut != i->LiveIn) {
            prev->LiveOut = i->LiveIn;
            changed = true;
          }
        }
      }
    }
  }
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
  PrintInstructionA2s(f, ir2_);
  f.close();
}
