#pragma once

#include <any>

#include "Scope.h"

enum class SymbolType : uint8_t {
  kNONE,
  kAST,
  kDEFINE,
  kIR,
};

const std::string kSymMDHasInit = "initialization";

class Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;

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

  [[nodiscard]] auto MetaData(const std::string& key) const -> std::any;

  void SetName(std::string name);

  void SetScope(ScopePtr scope);

  auto RefMetaData(const std::string& key) -> std::any&;

  void SetMetaData(const std::string& key, std::any value);

  void RemoveMetaData(const std::string& key);

 private:
  int index_;
  ScopePtr scope_;
  SymbolType type_;
  std::string name_;
  std::string value_;
  std::map<std::string, std::any> meta_data_;
};
