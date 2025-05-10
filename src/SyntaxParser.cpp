#include "SyntaxParser.h"

#include <iostream>

void SyntaxParser::Parse() {
  output_analyse_.clear();
  status_stack_.push(kEmptySymbolId);
  token_queue_ = lexer_.Tokens();

  // add EOF
  token_queue_.emplace(kTokenSpecEOF, kEOFSymbol);
  // add $
  token_queue_.emplace(kTokenSpecStop, kStopSymbol);

  while (!token_queue_.empty()) {
    const auto token = token_queue_.front();
    auto token_id = token.Id();

    const auto &p_step =
        generator_->FindActionStep(status_stack_.top(), token_id);

    if (p_step == nullptr) {
      print_error();
      return;
    }

    if (p_step->action == MOVE) {
      output_analyse_ << "MOVE IN" << "(AUTOMATA STATUS " << status_stack_.top()
                      << "): ";

      const auto token = token_queue_.front();
      print_symbol(token.Id());

      auto *node = new TreeNode(token.Id());
      node->AddValue(token.value);

      const auto *p_symbol = pool_->GetSymbol(token.Id());
      if (p_symbol->terminator) {
        node->AddInfo("terminator");
      }

      node->AddInfo(p_symbol->name);

      status_stack_.push(p_step->target.index);
      analyse_stack_.push(token.Id());
      tree_stack_.push(node);

      buffer_ << token.value << " ";
      token_queue_.pop();

    } else if (p_step->action == REDUCE) {
      const auto p_pdt = p_step->target.production;
      output_analyse_ << "REDUCE BY" << "(AUTOMATA STATUS "
                      << status_stack_.top() << "): [";
      print_production(p_pdt);
      output_analyse_ << "]";

      std::stack<TreeNode *> temp_stack;

      for (int i : p_pdt->right) {
        if (i == 0) continue;
        analyse_stack_.pop();
        status_stack_.pop();
        temp_stack.push(tree_stack_.top());
        tree_stack_.pop();
      }

      auto *father_node = new TreeNode(p_pdt->left);
      father_node->AddInfo(pool_->GetSymbol(p_pdt->left)->name);

      while (!temp_stack.empty()) {
        auto *node = temp_stack.top();
        const auto &child_info = node->GetInfoVec();
        if (child_info[0] == "terminator") {
          for (int i = 1; i < child_info.size(); i++) {
            father_node->AddInfo(child_info[i]);
          }

          for (const auto &value : node->GetValueVec()) {
            father_node->AddValue(value);
          }

          delete temp_stack.top();
        } else {
          temp_stack.top()->SetFather(father_node);
        }
        temp_stack.pop();
      }

      const auto &p_goto_step =
          generator_->FindGotoStep(status_stack_.top(), p_pdt->left);

      if (p_goto_step == nullptr) {
        print_error();
        return;
      }

      analyse_stack_.push(p_pdt->left);
      tree_stack_.push(father_node);
      status_stack_.push(p_goto_step->target.index);

    } else if (p_step->action == ACC) {
      syntax_tree_.setRoot(tree_stack_.top());
      tree_stack_.pop();
      output_analyse_ << "ACC";
      print_done();
      return;
    } else {
      print_error();
      return;
    }

    output_analyse_ << '\n';
  }
}

void SyntaxParser::print_production(const ProductionPtr &p_pdt) {
  output_analyse_ << pool_->GetSymbol(p_pdt->left)->name << " -> ";
  int i = 0;
  for (const auto &symbol_index : p_pdt->right) {
    if (i++ > 0) output_analyse_ << " ";

    print_symbol(symbol_index);
  }
}

void SyntaxParser::print_symbol(int symbol_index) {
  const auto *symbol = pool_->GetSymbol(symbol_index);

  if (symbol->index == 0) {
    output_analyse_ << "[Epsilon]";
    return;
  }

  if (!symbol->terminator) {
    output_analyse_ << pool_->GetSymbol(symbol_index)->name;
  } else {
    output_analyse_ << '"' << pool_->GetSymbol(symbol_index)->name << '"';
  }
}

void SyntaxParser::print_done() {
  output_analyse_ << '\n';
  output_analyse_ << "------------------------------------------------------"
                  << '\n';
  output_analyse_ << "Syntax Parser Work Done, No Error Found." << '\n' << '\n';
  output_analyse_.flush();
}

void SyntaxParser::print_error() {
  const auto token = token_queue_.front();
  std::string temp_line = buffer_.str();

  std::cout << "Syntax Parser Found Error: " << '\n'
            << "At: " << temp_line << "-> Next Token{"
            << pool_->GetSymbol(token.Id())->name << "}"
            << " " << "Value{" << token.value << "}" << '\n';

  std::cout << "AUTOMATA STATUS " << status_stack_.top() << '\n';
}

SyntaxParser::SyntaxParser(const Lexer &lexer, const LR1Generator &generator)
    : lexer_(lexer),
      pool_(generator.Pool()),
      generator_(generator.Generator()) {}

void SyntaxParser::PrintAnalyse(const std::string &path) {
  std::ofstream f(path);
  f << output_analyse_.str();
  f.close();
}

void SyntaxParser::PrintTree(const std::string &path) {
  std::ofstream f(path);
  syntax_tree_.print(f);
  f.close();
}

auto SyntaxParser::Tree() -> SyntaxTree & { return syntax_tree_; }
