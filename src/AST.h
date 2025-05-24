#pragma once

#include "ASTDefs.h"
#include "SymbolTable.h"

enum class ASTNodeType : uint8_t;
enum class ASTNodeTag : uint8_t;

class ASTNode;
using ASTNodePtr = std::shared_ptr<ASTNode>;

class ASTNode {
 public:
  ASTNode();

  ~ASTNode();

  ASTNode(ASTNodeType type, SymbolPtr symbol);

  ASTNode(ASTNodeType type, SymbolPtr symbol,
          std::initializer_list<ASTNodePtr> children);

  auto Parent() -> ASTNode *;

  auto Children() -> std::vector<ASTNodePtr>;

  void AddChild(const ASTNodePtr &child, ASTNodeTag tag = ASTNodeTag::kNONE);

  auto Type() -> ASTNodeType;

  auto Tag() -> ASTNodeTag;

  auto Symbol() -> SymbolPtr;

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

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
