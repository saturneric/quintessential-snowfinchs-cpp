#include "IRVariableOptimizer.h"

#include "model/CFGBasicBlock.h"
#include "model/SymbolDefs.h"
#include "model/Utils.h"

IRVariableOptimizer::IRVariableOptimizer(SymbolTablePtr symbol_table,
                                         ControlFlowGraphPtr cfg)
    : symbol_table_(std::move(symbol_table)), cfg_(std::move(cfg)) {}

void IRVariableOptimizer::AnalyzeLiveRanges() {
  live_ranges_.clear();

  // save all ins in bb
  struct InstInfo {
    CFGBasicBlockPtr bb;
    IRInstructionPtr instr;
    int idx_in_bb;
  };
  std::vector<InstInfo> insts;
  for (auto& bb : cfg_->Blocks()) {
    for (int j = 0; j < static_cast<int>(bb->Instrs().size()); ++j) {
      insts.push_back({bb, bb->Instrs()[j], j});
    }
  }

  for (int idx = 0; idx < static_cast<int>(insts.size()); ++idx) {
    auto& info = insts[idx];
    auto& bb = info.bb;
    auto& instr = info.instr;
    int j = info.idx_in_bb;
    int n = bb->Instrs().size();

    // 1) bb head live_in
    if (j == 0) {
      for (const auto& v : bb->live_in) {
        if (!IsVariable(v)) continue;
        auto& r = live_ranges_[v];
        r.start = std::min(r.start, idx);
        r.end = std::max(r.end, idx);
      }
    }
    // 2) use/def
    for (auto& v : instr->Use()) {
      if (!IsVariable(v)) continue;
      auto& r = live_ranges_[v];
      r.start = std::min(r.start, idx);
      r.end = std::max(r.end, idx);
    }
    if (auto d = instr->DST()) {
      auto& r = live_ranges_[d];
      r.start = std::min(r.start, idx);
      r.end = std::max(r.end, idx);
    }
    // 3) bb tail live_out
    if (j == n - 1) {
      for (const auto& v : bb->live_out) {
        if (!IsVariable(v)) continue;
        auto& r = live_ranges_[v];
        r.start = std::min(r.start, idx);
        r.end = std::max(r.end, idx);
      }
    }
  }
}

void IRVariableOptimizer::AllocateSlots() {
  struct RangeItem {
    int start, end;
    SymbolPtr var;
  };
  std::vector<RangeItem> items;
  items.reserve(live_ranges_.size());

  for (auto& [var, r] : live_ranges_) {
    if (r.start <= r.end) {
      items.push_back({r.start, r.end, var});
    }
  }
  // first oder as start
  // end oder as end
  std::sort(items.begin(), items.end(), [](auto& a, auto& b) {
    if (a.start != b.start) return a.start < b.start;
    return a.end < b.end;
  });

  // slots still in active range
  struct ActiveSlot {
    SymbolPtr sym;
    int end;
  };

  std::vector<ActiveSlot> active;
  std::deque<SymbolPtr> free_slots;

  var_to_slot_.clear();
  int slot_count = 0;

  for (auto& it : items) {
    // 1) destroy all slot out of active range
    int now = it.start;
    for (auto itr = active.begin(); itr != active.end();) {
      if (itr->end < now) {
        free_slots.push_back(itr->sym);
        itr = active.erase(itr);
      } else {
        ++itr;
      }
    }

    // 2) alloc a slot
    SymbolPtr slot;
    if (!free_slots.empty()) {
      slot = free_slots.front();
      free_slots.pop_front();
    } else {
      slot = map_slot("ir_slot_" + std::to_string(slot_count++));
    }
    var_to_slot_[it.var] = slot;

    // 3) insert active
    active.push_back({slot, it.end});
  }
}

void IRVariableOptimizer::RewriteInstructions() {
  for (const auto& bb : cfg_->Blocks()) {
    for (const auto& instr : bb->Instrs()) {
      std::set<SymbolPtr> vars;
      if (instr->DST() != nullptr) vars.insert(instr->DST());
      for (const auto& v : instr->Use()) {
        if (!v) continue;
        vars.insert(v);
      }

      for (const auto& v : vars) {
        if (!v || !var_to_slot_[v]) continue;
        instr->RenameVariable(v, var_to_slot_[v]);
      }
    }
  }
}

auto IRVariableOptimizer::map_slot(const std::string& name) -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kIR, name, "variable", true);
}

void IRVariableOptimizer::PrintSlotMap(const std::string& path) const {
  std::ofstream f(path);

  f << "==== Variable to Slot Mapping ====\n";
  for (const auto& [var, slot] : var_to_slot_) {
    if (!var || !slot) continue;
    f << var->Name() << "  ==>  " << slot->Name() << "\n";
  }
  f << "==================================\n";

  f.close();
}

void IRVariableOptimizer::PrintLiveRanges(const std::string& path) const {
  std::ofstream f(path);
  f << "==== Variable Live Ranges ====\n";
  for (const auto& [var, range] : live_ranges_) {
    if (!var) continue;
    f << var->Name() << "  [" << range.start << ", " << range.end << "]\n";
  }
  f << "==============================\n";
  f.close();
}

auto IRVariableOptimizer::GetSlotVariable(const SymbolPtr& var) -> SymbolPtr {
  return var_to_slot_.at(var);
}
