#include <ctime>
#include <iostream>

#include "ASMGenerator.h"
#include "Driver.hpp"
#include "IRGenerator.h"
#include "SemanticAnalyzer.h"

using Opera = std::function<bool()>;

namespace {
auto RunOperation(const std::string &name, Opera f) -> bool {
  auto start = clock();

  auto ret = f();

  auto end = clock();
  auto times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
  std::cout << name << " " << "Run time = " << times << "s MicroSeconds"
            << " = " << times * 1000 << "ms" << "\n";
  std::cout << name << " " << "Ret = " << ret << "\n";

  return ret;
}
}  // namespace

auto main(int argc, const char *argv[]) -> int {
  try {
    std::cout << "L1 Compiler Based on Bison"
              << "\n";

    if (argc < 2) {
      printf("Usage: <Input Path>\n");
      return -1;
    }

    const auto target = std::string{argv[1]};

    auto symbol_table = std::make_shared<SymbolTable>();
    Driver driver(symbol_table);

    auto ret = RunOperation("Lexer & Syntax Parser", [&]() {
      driver.Parse(target);
      driver.Print("PrintAST.txt");
      return true;
    });

    if (!ret) return 42;

    SemanticAnalyzer semantic(symbol_table);

    ret = RunOperation("Semantic Analyzer",
                       [&]() { return semantic.Analyze(driver.AST()); });

    if (!ret) return 7;

    IRGenerator irg;
    std::vector<IRInstructionA2> ir;

    ret = RunOperation("IR Generator", [&]() {
      ir = irg.Generate(driver.AST());
      irg.Print3Addr("PrintIR3.txt");
      irg.Print2Addr("PrintIR2.txt");
      return true;
    });

    ASMGenerator asm_gen(ir);

    ret = RunOperation("ASM Generator", [&]() {
      asm_gen.Generate("ASM.S");
      asm_gen.PrintIR("PrintIROpt.txt");
      return true;
    });

  } catch (std::runtime_error &e) {
    std::cout << "Runtime Error: " << e.what() << "\n";
    return -1;
  }

  return 0;
}
