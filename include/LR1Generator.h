//
// Created by Administrator on 2021/4/30.
//

#ifndef SYNTAXPARSER_LR1GENERATOR_H
#define SYNTAXPARSER_LR1GENERATOR_H

#include <AnalyseTableGenerator.h>
#include <GrammarResourcePool.h>
#include <ItemCollectionManager.h>

class LR1Generator {
  std::ifstream input;

  GrammarResourcePool *pool;

  ItemCollectionManager *icm;

  AnalyseTableGenerator *atg;

 public:
  LR1Generator()
      : input("SyntaxInput.txt", std::ios::binary),
        pool(new GrammarResourcePool()),
        icm(new ItemCollectionManager(pool)),
        atg(new AnalyseTableGenerator(pool, icm)) {
    input.imbue(input.getloc());
  }

  ~LR1Generator() { input.close(); }

  void run() {
    pool->FOLLOW();
    icm->buildItems();
    atg->generate();
    atg->print();
  }

  void getProductions();

  void output(const GrammarResourcePool *&m_pool,
              const AnalyseTableGenerator *&m_atg) {
    m_pool = this->pool;
    m_atg = this->atg;
  }
};

#endif  // SYNTAXPARSER_LR1GENERATOR_H
