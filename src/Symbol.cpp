#include "Symbol.h"

const std::string kSymbolMetaDataSyntaxTerminator = "syntax-terminator";
const std::string kSymbolMetaDataSyntaxStartSymbol = "syntax-start-symbol";

Symbol::Symbol(SymbolType type, int index, std::string name, std::string value)
    : type_(type),
      index_(index),
      name_(std::move(name)),
      value_(std::move(value)) {}

void Symbol::SetMetaData(std::string key, std::string value) {
  meta_data_.insert({std::move(key), std::move(value)});
}

auto Symbol::MetaData(const std::string& key) const -> std::string {
  if (meta_data_.count(key) == 0) return {};
  return meta_data_.at(key);
}

void Symbol::RemoveMetaData(const std::string& key) { meta_data_.erase(key); }

void Symbol::SetTerminator(bool terminator) {
  if (terminator) {
    SetMetaData(kSymbolMetaDataSyntaxTerminator, "1");
  } else {
    RemoveMetaData(kSymbolMetaDataSyntaxTerminator);
  }
}

void Symbol::SetStartSymbol(bool start_symbol) {
  if (start_symbol) {
    SetMetaData(kSymbolMetaDataSyntaxStartSymbol, "1");
  } else {
    RemoveMetaData(kSymbolMetaDataSyntaxStartSymbol);
  }
}

auto Symbol::IsTerminator() const -> bool {
  return meta_data_.count(kSymbolMetaDataSyntaxTerminator) != 0;
}

auto Symbol::IsSyntaxStartSymbol() const -> bool {
  return meta_data_.count(kSymbolMetaDataSyntaxStartSymbol) != 0;
}

void Symbol::SetName(std::string name) { name_ = std::move(name); }

auto Symbol::Type() const -> SymbolType { return type_; }

auto Symbol::Index() const -> int { return index_; }

auto Symbol::Name() const -> std::string { return name_; }

auto Symbol::Value() const -> std::string { return value_; }
