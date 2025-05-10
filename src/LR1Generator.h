#pragma once

#include <AnalyseTableGenerator.h>
#include <GrammarResourcePool.h>
#include <ItemCollectionManager.h>

#include "Lexer.h"

class LR1Generator {
 public:
  explicit LR1Generator(const Lexer& lexer);

  void Generate(const std::string& path);

  void Print(const std::string& path);

  [[nodiscard]] auto Pool() const -> std::shared_ptr<GrammarResourcePool>;

  [[nodiscard]] auto Generator() const
      -> std::shared_ptr<AnalyseTableGenerator>;

 private:
  std::shared_ptr<GrammarResourcePool> pool_;
  std::shared_ptr<ItemCollectionManager> icm_;
  std::shared_ptr<AnalyseTableGenerator> atg_;
};
