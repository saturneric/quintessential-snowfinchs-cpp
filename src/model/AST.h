#pragma once

#include "model/ASTDefs.h"
#include "model/Symbol.h"

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
