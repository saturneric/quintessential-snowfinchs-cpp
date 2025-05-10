#pragma once

#include <map>
#include <string>
#include <utility>

enum class SymbolType : uint8_t {
  kTOKEN_SPEC,
  kTOKEN,
  kSYNTAX,
  kAST,
};

class Symbol {
 public:
  Symbol(SymbolType type, int index, std::string name, std::string value);

  [[nodiscard]] auto Type() const -> SymbolType;

  [[nodiscard]] auto Index() const -> int;

  [[nodiscard]] auto Name() const -> std::string;

  [[nodiscard]] auto Value() const -> std::string;

  [[nodiscard]] auto MetaData(const std::string& key) const -> std::string;

  void SetName(std::string name);

  void SetMetaData(std::string key, std::string value);

  void RemoveMetaData(const std::string& key);

  void SetTerminator(bool terminator);

  void SetStartSymbol(bool start_symbol);

  [[nodiscard]] auto IsTerminator() const -> bool;

  [[nodiscard]] auto IsSyntaxStartSymbol() const -> bool;

 private:
  int index_;
  SymbolType type_;
  std::string name_;
  std::string value_;
  std::map<std::string, std::string> meta_data_;
};

using SymbolPtr = std::shared_ptr<Symbol>;
