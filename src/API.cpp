#include "API.h"

#include "ASMGenerator.h"
#include "BinaryGenerator.h"
#include "Driver.hpp"
#include "IRGenerator.h"
#include "SemanticAnalyzer.h"
#include "Utils.h"

auto CompileSourceCode(std::filesystem::path runtime_dir,
                       std::filesystem::path input,
                       std::filesystem::path output, bool debug, bool r64)
    -> int {
  auto symbol_table = std::make_shared<SymbolTable>();
  Driver driver(symbol_table);

  auto ret = RunOperation("Lexer & Syntax Parser", [&]() {
    auto ret = driver.Parse(input);
    if (debug) driver.Print(runtime_dir / "PrintAST.txt");
    return ret == 0;
  });

  if (!ret) return 42;

  SemanticAnalyzer semantic(symbol_table);

  ret = RunOperation("Semantic Analyzer", [&]() {
    auto ret = semantic.Analyze(driver.AST());
    if (debug) semantic.PrintSymbolTable(runtime_dir / "SemanticTable.txt");
    return ret;
  });

  if (!ret) return 7;

  IRGenerator irg(symbol_table);
  std::vector<IRInstructionA2Ptr> ir;

  ret = RunOperation("IR Generator", [&]() {
    ir = irg.Generate(driver.AST());
    if (debug) irg.Print3Addr(runtime_dir / "PrintIR3.txt");
    if (debug) irg.Print2Addr(runtime_dir / "PrintIR2.txt");
    if (debug) irg.PrintCFG(runtime_dir / "PrintCFG.txt");
    return true;
  });

  ASMGenerator asm_gen(symbol_table, !r64, ir, irg.ControlFlowGraph());

  ret = RunOperation("ASM Generator", [&]() {
    asm_gen.Generate(runtime_dir / "ASM.S");
    if (debug) asm_gen.PrintIR(runtime_dir / "PrintIROpt.txt");
    return true;
  });

  BinaryGenerator generator;
  ret = RunOperation("Binary Exec Generation", [&]() {
    return generator.Generate(runtime_dir / "ASM.S", output);
  });

  return 0;
}
auto MakeRuntimeDirectory() -> std::tuple<bool, fs::path> {
  auto current_dir = fs::current_path();
  auto runtime_dir = current_dir / ".runtime";

  if (!fs::exists(runtime_dir)) {
    try {
      fs::create_directory(runtime_dir);
      spdlog::info("runtime directory created: {}", runtime_dir.string());
    } catch (const fs::filesystem_error& e) {
      spdlog::info("create runtime directory failed: {}", e.what());
      return {false, {}};
    }
  }
  return {true, runtime_dir};
}
