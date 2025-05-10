//
// Created by Administrator on 2021/4/30.
//

#include "LR1Generator.h"

#include <iostream>

void LR1Generator::Generate(const std::string& path) {
  std::ifstream infile(path);
  if (!infile) {
    std::cout << "Failed to open source file: " << path << "\n";
    return;
  }
  std::string line;

  while (getline(infile, line)) {
    if (line.size() > 2 && line.front() != '#') {
      pool_->ParseProductionStringLine(line);
    }
  }

  pool_->FOLLOW();
  icm_->buildItems();
  atg_->generate();
}

LR1Generator::LR1Generator(const Lexer& lexer)
    : pool_(std::make_shared<GrammarResourcePool>()),
      icm_(std::make_shared<ItemCollectionManager>(pool_)),
      atg_(std::make_shared<AnalyseTableGenerator>(pool_, icm_)) {
  for (const auto& spec : lexer.TokenSpecs()) {
    pool_->AddSymbolToken(spec.id, spec.name);
  }
}

void LR1Generator::Print(const std::string& path) { atg_->Print(path); }

auto LR1Generator::Pool() const -> std::shared_ptr<GrammarResourcePool> {
  return pool_;
}

auto LR1Generator::Generator() const -> std::shared_ptr<AnalyseTableGenerator> {
  return atg_;
}
