#include "CFSemanticAnalyzer.h"

#include "model/CFGBasicBlock.h"
#include "model/SymbolDefs.h"

CFSemanticAnalyzer::CFSemanticAnalyzer(ControlFlowGraphPtr cfg)
    : cfg_(std::move(cfg)) {}

auto CFSemanticAnalyzer::Analyse() -> bool {
  check_definite_assignment();
  // check_all_paths_return();
  return succ_;
}

void CFSemanticAnalyzer::check_all_paths_return() {
  cfg_->BuildDominatorTree();
  auto blocks = cfg_->Blocks();

  bool changed;
  do {
    changed = false;
    for (const auto& bb : blocks) {
      if (bb->will_return || !bb->reachable) continue;

      std::vector<CFGBasicBlockPtr> filtered_succs;
      auto u_v = cfg_->BlockByBlockId(bb->id);
      for (auto& succ : cfg_->Successors(bb->id)) {
        if (!succ->reachable) continue;
        auto v_v = cfg_->BlockByBlockId(succ->id);
        if (!cfg_->Dominates(u_v->id, v_v->id)) {
          filtered_succs.push_back(succ);
        }
      }

      if (filtered_succs.empty()) {
        bb->will_return = true;
        changed = true;
        continue;
      }

      bool all_succ_return = true;
      for (auto& succ : filtered_succs) {
        if (!succ->will_return) {
          all_succ_return = false;
          break;
        }
      }
      if (all_succ_return) {
        bb->will_return = true;
        changed = true;
      }
    }
  } while (changed);

  auto entry = cfg_->BlockByBlockId(0);
  if (entry && !entry->will_return) {
    fail();

    SPDLOG_ERROR("Semantic Error: Not all code paths return a value!");
  }
}

void CFSemanticAnalyzer::check_definite_assignment() {
  auto blocks = cfg_->Blocks();

  for (auto& bb : blocks) {
    if (!bb->reachable) continue;

    std::set<SymbolPtr> in_vars = bb->def_in;
    std::set<SymbolPtr> local_def = bb->def;

    for (const auto& var : bb->use) {
      // var used not in IN collection and not def as well
      if (in_vars.find(var) == in_vars.end() &&
          local_def.find(var) == local_def.end()) {
        fail();

        auto sym = MetaGet<SymbolPtr>(var, SymbolMetaKey::kDEF_SYMBOL, nullptr);

        SPDLOG_ERROR(
            "Semantic Error: Variable '{}' may be used uninitialized in "
            "block.",
            sym ? sym->Name() : var->Name());
      }
    }
  }
}

void CFSemanticAnalyzer::fail() { succ_ = false; }
