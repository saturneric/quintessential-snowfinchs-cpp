#include <AnalyseTableGenerator.h>
#include <Automata.h>
#include <GrammarResourcePool.h>
#include <LR1Generator.h>
#include <SymbolTable.h>
#include <SyntaxParser.h>

#include <ctime>
#include <iostream>

auto main(int argc, const char *argv[]) -> int {
  try {
    std::cout << "Compile Program Based on LR(1) Written By Saturneric(???? "
                 "2018303206)"
              << "\n";

    if (argc < 2) {
      printf("Usage: <Input Path>\n");
      return -1;
    }

    clock_t start;
    clock_t end;
    start = clock();

    Automata atm(argv[1]);

    atm.parse();

    atm.output();

    end = clock();
    double times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::wcout << "Token Automata Run time = " << times << "s MicroSeconds"
               << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    const GrammarResourcePool *pool;

    const AnalyseTableGenerator *atg;

    LR1Generator generator;

    generator.getProductions();

    generator.run();

    generator.output(pool, atg);

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "LR(1) Generator Run time = " << times << "s MicroSeconds"
              << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    SyntaxParser syntax_parser(pool, atg);

    syntax_parser.getToken();

    syntax_parser.parse();

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Syntax Parser Run time = " << times << "s MicroSeconds "
              << " = " << times * 1000 << "ms" << "\n";

  } catch (std::runtime_error &e) {
    std::cout << "Runtime Error: " << e.what() << "\n";
  }

  return 0;
}
