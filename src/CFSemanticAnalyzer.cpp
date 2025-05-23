#include "CFSemanticAnalyzer.h"

#include <boost/graph/dominator_tree.hpp>
#include <boost/property_map/property_map.hpp>

CFSemanticAnalyzer::CFSemanticAnalyzer(ControlFlowGraphPtr cfg)
    : cfg_(std::move(cfg)) {}

auto CFSemanticAnalyzer::Analyse() -> bool {
  check_definite_assignment();
  // check_all_paths_return();
  return succ_;
}

void CFSemanticAnalyzer::check_all_paths_return() {
  auto blocks = cfg_->Blocks();
  auto graph = cfg_->Graph();
  auto filtered_graph = cfg_->FilteredGraph();

  // calculate dominator tree to detect cycle
  std::vector<Vertex> dom_tree(num_vertices(graph));
  Vertex entry_v = cfg_->VertexByBlockId(0);
  lengauer_tarjan_dominator_tree(
      *filtered_graph, entry_v,
      make_iterator_property_map(dom_tree.begin(),
                                 get(boost::vertex_index, graph)));

  auto dominates = [&](Vertex u_node, Vertex v_dom) {
    Vertex x = u_node;
    while (true) {
      if (x == v_dom) return true;
      if (x == entry_v) break;
      x = dom_tree[x];
    }
    return false;
  };

  bool changed;
  do {
    changed = false;
    for (const auto& bb : blocks) {
      if (bb->will_return || !bb->reachable) continue;

      std::vector<CFGBasicBlockPtr> filtered_succs;
      Vertex u_v = cfg_->VertexByBlockId(bb->id);
      for (auto& succ : cfg_->Successors(bb->id)) {
        if (!succ->reachable) continue;
        Vertex v_v = cfg_->VertexByBlockId(succ->id);
        if (!dominates(u_v, v_v)) {
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
