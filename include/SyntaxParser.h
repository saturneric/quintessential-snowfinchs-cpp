//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_SYNTAXPARSER_H
#define SYNTAXPARSER_SYNTAXPARSER_H

#include <AnalyseTableGenerator.h>
#include <GrammarResourcePool.h>
#include <SyntaxTree.h>

#include <codecvt>
#include <queue>
#include <regex>
#include <stack>

class SyntaxParser {
  // �ļ�����
  std::ifstream input;

  std::ofstream output;

  std::ofstream treeOutput;

  const GrammarResourcePool *pool;

  const AnalyseTableGenerator *atg;

  std::queue<int> tokens_queue;

  std::stack<int> analyse_stack;

  std::stack<int> status_stack;

  std::stack<TreeNode *> tree_stack;

  std::vector<size_t> lines_index;

  std::stringstream string_buffer;

  size_t now_line = 1;

  static std::vector<std::string> ws_split(const std::string &in,
                                           const std::string &delim);

  static std::pair<std::string, std::string> get_token_info(
      const std::string &token);

  SyntaxTree syntaxTree;

 public:
  SyntaxParser(const GrammarResourcePool *pool,
               const AnalyseTableGenerator *atg)
      : input("tokenOut.txt", std::ios::binary),
        pool(pool),
        atg(atg),
        output("AnalyseOut.txt", std::ios::binary),
        treeOutput("SyntaxOut.txt", std::ios::binary) {
    input.imbue(input.getloc());
    output.imbue(output.getloc());
    treeOutput.imbue(treeOutput.getloc());
  }

  ~SyntaxParser() {
    input.close();
    output.close();
    treeOutput.close();
  }

  // �õ����еĲ���ʽ
  void getToken();

  void printSymbol(int symbol_index);

  void printProduction(const Production *p_pdt);

  // �Ե������﷨����
  void parse();

  void printError(std::ofstream &errOutput);

  void printError();

  void printDone();
};

#endif  // SYNTAXPARSER_SYNTAXPARSER_H
