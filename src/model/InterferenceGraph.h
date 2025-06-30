#pragma once

#include "model/Symbol.h"

using INFHandler = int;

class InterferenceGraph {
 public:
  InterferenceGraph();

  ~InterferenceGraph();

  void AddVariable(const SymbolPtr& symbol);

  void AddEdge(const SymbolPtr& var1, const SymbolPtr& var2);

  void RemoveEdge(const SymbolPtr& var1, const SymbolPtr& var2);

  [[nodiscard]] auto Neighbors(const SymbolPtr& var) const
      -> std::set<SymbolPtr>;

  [[nodiscard]] auto Variables() const -> std::vector<SymbolPtr>;

  [[nodiscard]] auto GetVertexBySymbol(const SymbolPtr& symbol) const
      -> std::optional<INFHandler>;

  void Print(std::ostream& os) const;

  [[nodiscard]] auto MCSOrder() const -> std::vector<SymbolPtr>;

  [[nodiscard]] auto Coloring() const -> std::map<SymbolPtr, int>;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};