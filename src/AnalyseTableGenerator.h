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
      ProductionPtr production;
    } target{};

    Step(Action action, int index);
    Step(Action action, const ProductionPtr &p_pdt);
  };

  using StepPtr = std::shared_ptr<Step>;

  explicit AnalyseTableGenerator(
      const std::shared_ptr<GrammarResourcePool> &p_pool,
      const std::shared_ptr<ItemCollectionManager> &p_icm)
      : pool_(p_pool), icm_(p_icm) {}

  void Generate();

  [[nodiscard]] auto FindActionStep(int index, int terminator_symbol) const
      -> StepPtr;

  [[nodiscard]] auto FindGotoStep(int index, int non_terminator_symbol) const
      -> StepPtr;

  void Print(const std::string &path) const;

 private:
  std::map<size_t, StepPtr> ACTION_;
  std::map<size_t, StepPtr> GOTO_;

  const std::shared_ptr<ItemCollectionManager> icm_;
  const std::shared_ptr<GrammarResourcePool> pool_;

  template <class T>
  void hash_combine(std::size_t &seed, const T &v) const {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  void add_action(int index, int terminator_symbol, Action action,
                  const ProductionPtr &target_pdt);

  void add_action(int index, int terminator_symbol, Action action,
                  int target_index);

  void add_goto(int index, int non_terminator_symbol, int target_index);
};
