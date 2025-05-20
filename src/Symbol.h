#pragma once

#include "Scope.h"

enum class SymbolType {
  kNONE,
  kAST,
  kDEFINE,
};

const std::string kSymMDHasInit = "initialization";

class Symbol {
 public:
  Symbol(SymbolType type, int index, ScopePtr scope, std::string name,
         std::string value);

  [[nodiscard]] auto Type() const -> SymbolType;

  [[nodiscard]] auto Index() const -> int;

  [[nodiscard]] auto Name() const -> std::string;

  [[nodiscard]] auto Value() const -> std::string;

  [[nodiscard]] auto Scope() const -> ScopePtr;

  [[nodiscard]] auto ScopeId() const -> int;

  [[nodiscard]] auto MetaData(const std::string& key) const -> std::string;

  void SetName(std::string name);

  void SetScope(ScopePtr scope);

  void SetMetaData(std::string key, std::string value);

  void RemoveMetaData(const std::string& key);

  void SetTerminator(bool terminator);

  void SetStartSymbol(bool start_symbol);

  [[nodiscard]] auto IsTerminator() const -> bool;

  [[nodiscard]] auto IsSyntaxStartSymbol() const -> bool;

 private:
  int index_;
  ScopePtr scope_;
  SymbolType type_;
  std::string name_;
  std::string value_;
  std::map<std::string, std::string> meta_data_;
};

using SymbolPtr = std::shared_ptr<Symbol>;
