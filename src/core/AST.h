#pragma once

#include "core/SymbolTable.h"
#include "model/AST.h"

class AST {
 public:
  explicit AST(SymbolTablePtr symbol_table);

  ~AST();

  void SetRoot(const ASTNodePtr &root);

  void Print(const std::string &path);

  [[nodiscard]] auto Root() const -> ASTNodePtr;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void do_ast_node_print(const ASTNodePtr &node, std::ofstream &stream);
};