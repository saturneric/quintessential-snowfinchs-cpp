#pragma once

#include <sstream>
#include <stack>

#include "AnalyseTableGenerator.h"
#include "GrammarResourcePool.h"
#include "LR1Generator.h"
#include "Lexer.h"
#include "SyntaxTree.h"

class SyntaxParser {
 public:
  explicit SyntaxParser(const Lexer &lexer, const LR1Generator &generator);

  void Parse();

  auto Tree() -> SyntaxTree &;

  void PrintAnalyse(const std::string &path);

  void PrintTree(const std::string &path);

 private:
  std::shared_ptr<GrammarResourcePool> pool_;
  std::shared_ptr<AnalyseTableGenerator> generator_;

  std::queue<LexerToken> token_queue_;
  std::stack<int> analyse_stack_;
  std::stack<int> status_stack_;
  std::stack<TreeNode *> tree_stack_;
  std::stringstream buffer_;

  std::stringstream output_analyse_;

  SyntaxTree syntax_tree_;
  const Lexer &lexer_;

  void print_error();

  void print_done();

  void print_symbol(int symbol_index);

  void print_production(const std::shared_ptr<Production> &p_pdt);
};
