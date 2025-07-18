#include "SemanticAnalyzer.h"

#include <algorithm>
#include <utility>

#include "model/SymbolDefs.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTablePtr symbol_table,
                                   SMHandlerMapping mapping)
    : symbol_table_(std::move(symbol_table)),
      helper_(SymbolType::kAST, symbol_table_),
      def_sym_helper_(SymbolType::kDEFINE, symbol_table_),
      type_desc_sym_helper_(SymbolType::kTYPEDESC, symbol_table_),
      node_handler_register_(std::move(mapping)) {
  // map predefined types with their sizes
  MapType("int", "4");
  MapType("bool", "4");
  MapType("type", "0");
  MapType("nullptr", "0");
};

auto SemanticAnalyzer::visit(const ASTNodePtr& node) -> ASTNodePtr {
  if (node == nullptr) return node;
  if (node_handler_register_.count(node->Type()) == 0) return {};

  if (node && (node->Parent() != nullptr)) {
    node->Symbol()->Inheritance(node->Parent()->Symbol());
  }

  return node_handler_register_[node->Type()](
      this, [=](const ASTNodePtr& n) -> ASTNodePtr { return this->visit(n); },
      node);
}

auto SemanticAnalyzer::LookupSymbol(const SymbolPtr& symbol) -> SymbolPtr {
  return def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());
}

auto SemanticAnalyzer::is_integer_literal(const std::string& s) -> bool {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void SemanticAnalyzer::Error(const ASTNodePtr& /*node*/,
                             const std::string& msg) {
  SPDLOG_ERROR("Semantic Error: {}", msg);
  succ_ = false;
}

auto SemanticAnalyzer::MapSymbol(int scope_id, const std::string& name,
                                 const std::string& value) -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kDEFINE, name, value, true,
                                  scope_id);
}

auto SemanticAnalyzer::MapFunction(int scope_id, const std::string& name,
                                   const std::string& value) -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kFUNCTION, name, value, true,
                                  scope_id);
}

auto SemanticAnalyzer::MapStruct(int scope_id, const std::string& name,
                                 const std::string& value) -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kSTRUCT, name, value, true,
                                  scope_id);
}

auto SemanticAnalyzer::MapType(const std::string& id, const std::string& extra)
    -> SymbolPtr {
  return symbol_table_->AddSymbol(SymbolType::kTYPEDESC, id, extra, true);
}

auto SemanticAnalyzer::RecordSymbol(const SymbolPtr& symbol)
    -> std::tuple<bool, SymbolPtr> {
  // we are not allow variable-shadowing
  auto sym = def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());

  // should not check twice
  if (sym != nullptr) return {false, sym};

  const auto in_var = "ins_" + std::to_string(symbol->ScopeId()) + "_" +
                      std::to_string(inner_var_index_++);

  sym = symbol_table_->AddSymbol(SymbolType::kDEFINE, symbol->Name(), in_var,
                                 true, symbol->ScopeId());
  return {true, sym};
}

auto SemanticAnalyzer::RecordRealSymbol(const SymbolPtr& symbol)
    -> std::tuple<bool, SymbolPtr> {
  // we are not allow variable-shadowing
  auto sym = def_sym_helper_.LookupSymbol(symbol->Scope(), symbol->Name());

  // should not check twice
  if (sym != nullptr) return {false, sym};

  sym = symbol_table_->AddSymbol(SymbolType::kDEFINE, symbol->Name(),
                                 symbol->Name(), true, symbol->ScopeId());
  return {true, sym};
}

auto SemanticAnalyzer::Analyze(const AST& ast) -> bool {
  succ_ = true;

  auto root = ast.Root();
  if (!root) {
    Error(nullptr, "The AST is empty");
  }

  ast_root_ = root;
  visit(root);

  if (root && meta_data_.count("has_return") == 0) {
    Error(ast.Root(), "Return statement not found");
  }

  return succ_;
}

void SemanticAnalyzer::SetMeta(const std::string& key, std::any value) {
  meta_data_[key] = std::move(value);
}

auto SemanticAnalyzer::Meta(const std::string& key) -> std::any& {
  return meta_data_[key];
}

void SemanticAnalyzer::PrintSymbolTable(const std::string& path) {
  if (symbol_table_ == nullptr) return;

  std::ofstream f(path);
  symbol_table_->PrintByType(SymbolType::kDEFINE, f);
  f.close();
}

auto SemanticAnalyzer::VisibleDefineSymbols(const ScopePtr& scope)
    -> std::vector<SymbolPtr> {
  return def_sym_helper_.LookupSymbols(scope);
}

auto SemanticAnalyzer::GetRootScopeId() const -> int {
  return ast_root_ ? ast_root_->Symbol()->ScopeId() : kNonScopeId;
}

auto SemanticAnalyzer::LookupType(const std::string& type_name) -> SymbolPtr {
  return type_desc_sym_helper_.LookupSymbolWithoutScope(type_name);
}

auto SemanticAnalyzer::LookupSymbol(const ScopePtr& scope,
                                    const std::string& name) -> SymbolPtr {
  return def_sym_helper_.LookupSymbol(scope, name);
}
auto SemanticAnalyzer::GetRootScope() const -> ScopePtr {
  return ast_root_ ? ast_root_->Symbol()->Scope() : nullptr;
}
