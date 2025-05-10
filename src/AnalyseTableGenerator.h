#pragma once

#include <GrammarResourcePool.h>
#include <ItemCollectionManager.h>

#include <fstream>
#include <iomanip>

using Action = enum { MOVE, REDUCE, ACC, STEP_GOTO };

class AnalyseTableGenerator {
 public:
  struct Step {
    const Action action;
    struct Target {
      int index;
      std::shared_ptr<Production> production;
    } target{};

    Step(Action action, int index) : action(action), target(Target{index}) {}
    Step(Action action, const std::shared_ptr<Production> &p_pdt)
        : action(action) {
      target.production = p_pdt;
    }
  };

  explicit AnalyseTableGenerator(
      const std::shared_ptr<GrammarResourcePool> p_pool,
      const std::shared_ptr<ItemCollectionManager> p_icm)
      : pool(p_pool), icm(p_icm) {}

  void generate();

  const Step *findActionStep(int index, int terminator_symbol) const;

  const Step *findGotoStep(int index, int non_terminator_symbol) const;

  void Print(const std::string &path) const;

 private:
  std::map<size_t, Step *> ACTION;

  std::map<size_t, Step *> GOTO;

  const std::shared_ptr<ItemCollectionManager> icm;

  const std::shared_ptr<GrammarResourcePool> pool;

  template <class T>
  void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  void add_action(int index, int terminator_symbol, Action action,
                  const std::shared_ptr<Production> &target_pdt);

  void add_action(int index, int terminator_symbol, Action action,
                  int target_index);

  void add_goto(int index, int non_terminator_symbol, int target_index);
};
