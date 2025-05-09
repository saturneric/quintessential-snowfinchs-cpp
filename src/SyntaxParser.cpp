//
// Created by Administrator on 2021/4/30.
//

#include "SyntaxParser.h"

void SyntaxParser::parse() {
  status_stack.push(0);

  now_line = 1;
  size_t line_index = 0;
  size_t max_line_index = lines_index[now_line - 1];
  while (!tokens_queue.empty()) {
    const auto *p_step =
        atg->findActionStep(status_stack.top(), tokens_queue.front());

    if (p_step == nullptr) {
      printError();
      return;
    }

    if (p_step->action == MOVE) {
      output << "MOVE IN" << "(AUTOMATA STATUS " << status_stack.top() << "): ";
      printSymbol(tokens_queue.front());

      auto *node = new TreeNode(tokens_queue.front());

      const auto *p_symbol = pool->getSymbol(tokens_queue.front());
      if (p_symbol->terminator) {
        node->addInfo("terminator");
      }
      node->addInfo(p_symbol->name);

      status_stack.push(p_step->target.index);
      analyse_stack.push(tokens_queue.front());
      tree_stack.push(node);

      if (line_index > max_line_index) {
        string_buffer.str("");
        string_buffer.clear();
        max_line_index = lines_index[now_line++];
      }
      string_buffer << p_symbol->name << " ";
      tokens_queue.pop();
      line_index++;
    } else if (p_step->action == REDUCE) {
      const auto *p_pdt = p_step->target.production;
      output << "REDUCE BY" << "(AUTOMATA STATUS " << status_stack.top()
             << "): [";
      printProduction(p_pdt);
      output << "]";

      std::stack<TreeNode *> temp_stack;

      for (int i : p_pdt->right) {
        if (i == 0) continue;
        analyse_stack.pop();
        status_stack.pop();
        temp_stack.push(tree_stack.top());
        tree_stack.pop();
      }

      auto *father_node = new TreeNode(p_pdt->left);

      father_node->addInfo(pool->getSymbol(p_pdt->left)->name);

      // std::cout << fatherNode->getInfoVec()[0] << std::endl;

      while (!temp_stack.empty()) {
        // std::cout << temp_stack.top()->getInfoVec()[0] << std::endl;

        const auto &child_info = temp_stack.top()->getInfoVec();
        if (child_info[0] == "terminator") {
          for (int i = 1; i < child_info.size(); i++) {
            father_node->addInfo(child_info[i]);
          }
          delete temp_stack.top();
        } else {
          temp_stack.top()->setFather(father_node);
        }
        temp_stack.pop();
      }

      // std::cout << std::endl;

      const auto *p_goto_step =
          atg->findGotoStep(status_stack.top(), p_pdt->left);

      if (p_goto_step == nullptr) {
        printError();
        return;
      }

      analyse_stack.push(p_pdt->left);
      tree_stack.push(father_node);
      status_stack.push(p_goto_step->target.index);

    } else if (p_step->action == ACC) {
      syntaxTree.setRoot(tree_stack.top());
      tree_stack.pop();
      output << "ACC";
      printDone();
      return;
    } else {
      printError();
      return;
    }

    output << '\n';
  }
}

void SyntaxParser::printProduction(const Production *p_pdt) {
  output << pool->getSymbol(p_pdt->left)->name << " -> ";
  int i = 0;
  for (const auto &symbol_index : p_pdt->right) {
    if (i++ > 0) output << " ";

    printSymbol(symbol_index);
  }
}

void SyntaxParser::printSymbol(int symbol_index) {
  const auto *symbol = pool->getSymbol(symbol_index);

  if (symbol->index == 0) {
    output << "[Epsilon]";
    return;
  }
  if (!symbol->terminator) {
    output << pool->getSymbol(symbol_index)->name;
  } else {
    output << '"' << pool->getSymbol(symbol_index)->name << '"';
  }
}

void SyntaxParser::getToken() {
  std::string temp_line;

  size_t line_index = 0;
  while (getline(input, temp_line)) {
    if (temp_line.size() > 2 && temp_line[0] != '#') {
      std::vector<std::string> tokens = ws_split(temp_line, " ");

      for (int i = 1; i < tokens.size(); i++) {
        if (tokens[i] == "\r") continue;
        ;
        auto token_info = get_token_info(tokens[i]);
        tokens_queue.push(pool->getSymbolIndex(token_info.first));
        line_index++;
      }
      lines_index.push_back(line_index - 1);
    }
  }

  tokens_queue.push(-1);
}

std::vector<std::string> SyntaxParser::ws_split(const std::string &in,
                                                const std::string &delim) {
  std::regex re{delim};
  return std::vector<std::string>{
      std::sregex_token_iterator(in.begin(), in.end(), re, -1),
      std::sregex_token_iterator()};
}

std::pair<std::string, std::string> SyntaxParser::get_token_info(
    const std::string &token) {
  auto pre_index = token.find(L'(');

  auto back_index = token.find(L')');

  std::string name = token.substr(0, pre_index);
  std::string value = token.substr(pre_index + 1, back_index - pre_index - 1);

  return std::pair<std::string, std::string>(name, value);
}

void SyntaxParser::printDone() {
  output << '\n';
  output << "------------------------------------------------------" << '\n';
  output << "Syntax Parser Work Done, No Error Found." << '\n' << '\n';

  syntaxTree.print(treeOutput);
}

void SyntaxParser::printError(std::ofstream &errOutput) {
  std::string temp_line = string_buffer.str();
  errOutput << '\n';
  errOutput << "------------------------------------------------------" << '\n';
  errOutput.fill('-');
  errOutput.width(24);
  errOutput << "Syntax Parser Found Error: " << '\n'
            << "At [Line " << now_line << "]: " << temp_line << "<- Next Token{"
            << pool->getSymbol(tokens_queue.front())->name << "}" << '\n';
  errOutput << "AUTOMATA STATUS " << status_stack.top() << '\n';
}

void SyntaxParser::printError() {
  printError(output);
  printError(treeOutput);
}
