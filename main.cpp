#include <ctime>
#include <iostream>

#include "ASMGenerator.h"
#include "AST.h"
#include "Driver.hpp"
#include "IRGenerator.h"
#include "LR1Generator.h"
#include "Lexer.h"
#include "SyntaxParser.h"

auto main(int argc, const char *argv[]) -> int {
  try {
    std::cout << "Compiler Based on LR(1)"
              << "\n";

    if (argc < 2) {
      printf("Usage: <Input Path>\n");
      return -1;
    }

    clock_t start;
    clock_t end;
    start = clock();

    auto symbol_table = std::make_shared<SymbolTable>();

    Driver driver(symbol_table);
    driver.Parse(argv[1]);
    driver.Print("PrintAST.txt");

    end = clock();
    double times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Lexer & Parser Run time = " << times << "s MicroSeconds"
              << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    IRGenerator irg;
    auto ir = irg.Generate(driver.AST());
    irg.Print3Addr("PrintIR3.txt");
    irg.Print2Addr("PrintIR2.txt");

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "IR Generator Run time = " << times << "s MicroSeconds"
              << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    ASMGenerator asm_gen(ir);
    asm_gen.Generate("ASM.S");
    asm_gen.PrintIR("PrintIROpt.txt");

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "ASM Generator Run time = " << times << "s MicroSeconds "
              << " = " << times * 1000 << "ms" << "\n";

  } catch (std::runtime_error &e) {
    std::cout << "Runtime Error: " << e.what() << "\n";
  }

  return 0;
}
