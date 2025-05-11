#include <ctime>
#include <iostream>

//
#include <cxxopts.hpp>

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
  std::cout << "\n";

  return ret;
}
}  // namespace

auto main(int argc, const char *argv[]) -> int {
  try {
    cxxopts::Options options("L1 Compiler", "A toy compiler using Bison");

    options.add_options()("d,debug", "Enable debug output")(
        "i,input", "Input file", cxxopts::value<std::string>())(
        "o,output", "Output file", cxxopts::value<std::string>())(
        "h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help") != 0U) {
      std::cout << options.help() << '\n';
      return 0;
    }

    auto debug = result["debug"].as<bool>();
    auto input = result["input"].as<std::string>();
    auto output = result["output"].as<std::string>();

    auto symbol_table = std::make_shared<SymbolTable>();
    Driver driver(symbol_table);

    auto ret = RunOperation("Lexer & Syntax Parser", [&]() {
      auto ret = driver.Parse(input);
      if (debug) driver.Print("PrintAST.txt");
      return ret == 0;
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
      if (debug) irg.Print3Addr("PrintIR3.txt");
      if (debug) irg.Print2Addr("PrintIR2.txt");
      return true;
    });

    ASMGenerator asm_gen(ir);

    ret = RunOperation("ASM Generator", [&]() {
      asm_gen.Generate(output);
      if (debug) asm_gen.PrintIR("PrintIROpt.txt");
      return true;
    });

  } catch (std::runtime_error &e) {
    std::cout << "Runtime Error: " << e.what() << "\n";
    return -1;
  }

  return 0;
}
