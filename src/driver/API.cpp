#include "API.h"

#include "core/ASMGenerator.h"
#include "core/BinaryGenerator.h"
#include "core/CFSemanticAnalyzer.h"
#include "core/Driver.hpp"
#include "core/IR2Generator.h"
#include "core/IRGenerator.h"
#include "core/IRVariableOptimizer.h"
#include "core/SemanticAnalyzer.h"
#include "core/Utils.h"

// handlers

#include "handlers/IGHandlers.h"
#include "handlers/SemanticHandlers.h"
#include "handlers/Translators.h"

// cxxpots
#include <cxxopts.hpp>

// std
#include <iostream>

auto CheckInputOutput(const cxxopts::ParseResult& result, std::string& input,
                      std::string& output) -> bool {
  // Check input
  if (result.count("input") == 0U) {
    SPDLOG_ERROR("Missing required --input argument.");
    return false;
  }
  input = result["input"].as<std::string>();
  if (!fs::exists(input)) {
    SPDLOG_ERROR("input file [{}] does not exist.", input);
    return false;
  }
  if (!fs::is_regular_file(input)) {
    SPDLOG_ERROR("input [{}] is not a regular file.", input);
    return false;
  }
  std::ifstream infile(input);
  if (!infile.is_open()) {
    SPDLOG_ERROR("input file [{}] is not readable.", input);
    return false;
  }

  // Check output
  if (result.count("output") == 0U) {
    SPDLOG_ERROR("missing required --output argument.");
    return false;
  }
  output = result["output"].as<std::string>();
  fs::path output_path(output);
  fs::path parent_dir = output_path.parent_path();
  if (!parent_dir.empty() && !fs::exists(parent_dir)) {
    SPDLOG_ERROR("the output directory [{}] does not exist.",
                 parent_dir.string());
    return false;
  }

  // Try writing a temporary file to check if directory is writable
  if (!parent_dir.empty() && fs::exists(parent_dir)) {
    std::string testfile = (parent_dir / ".__testwrite__").string();
    std::ofstream ofs(testfile);
    if (!ofs.is_open()) {
      SPDLOG_ERROR("the output directory [{}] is not writable.",
                   parent_dir.string());
      return false;
    }
    ofs.close();
    fs::remove(testfile);
  }
  return true;
}

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

  SemanticAnalyzer semantic(symbol_table, GetSMHandlersMap());

  ret = RunOperation("Semantic Analyzer", [&]() {
    auto ret = semantic.Analyze(driver.AST());
    if (debug) semantic.PrintSymbolTable(runtime_dir / "PrintSmtTable.txt");
    return ret;
  });

  if (!ret) return 7;

  IRGenerator irg(symbol_table, GetIRHandlersMap());

  ret = RunOperation("IR Generator", [&]() {
    irg.Generate(driver.AST());
    if (debug) irg.PrintAddr(runtime_dir / "PrintIR3.txt");
    if (debug) irg.PrintCFG(runtime_dir / "PrintCFG(IR3).txt");
    return true;
  });

  auto fcs = irg.ControlFlowGraph();

  for (const auto& fc : fcs) {
    CFSemanticAnalyzer cf_semantic(fc.cfg);

    ret = RunOperation("CF Semantic Analyzer", [&]() {
      auto ret = cf_semantic.Analyse();
      if (debug) irg.PrintCFG(runtime_dir / "PrintCFG(SEMANTIC).txt");
      return ret;
    });

    if (!ret) return 7;

    IRVariableOptimizer irv_opt(symbol_table, fc.cfg);

    ret = RunOperation("IRVariableOptimizer", [&]() {
      irv_opt.AnalyzeLiveRanges();
      irv_opt.AllocateSlots();
      irv_opt.RewriteInstructions();
      if (debug) {
        irv_opt.PrintLiveRanges(runtime_dir / "PrintVarLiveRanges.txt");
      }
      if (debug) irv_opt.PrintSlotMap(runtime_dir / "PrintVar2Slot.txt");
      return true;
    });
  }

  std::vector<FuncInstructions> fis;
  fis.reserve(fcs.size());
  for (const auto& fc : fcs) {
    fis.push_back({fc.func, fc.cfg->Instructions()});
  }

  IR2Generator ir2g(symbol_table, fis);

  ret = RunOperation("IR2 Generator", [&]() {
    ir2g.Generate();
    if (debug) ir2g.PrintAddr(runtime_dir / "PrintIR2.txt");
    if (debug) ir2g.PrintCFG(runtime_dir / "PrintCFG(IR2).txt");
    return true;
  });

  fis.clear();
  fcs = ir2g.ControlFlowGraph();
  for (const auto& fc : fcs) {
    fis.push_back({fc.func, fc.cfg->Instructions()});
  }

  auto translator = GetX86Translator(!r64);
  ASMGenerator asm_gen(symbol_table, translator, fis);

  ret = RunOperation("ASM Generator", [&]() {
    asm_gen.Generate(runtime_dir / "ASM.S");
    if (debug) asm_gen.PrintIFG(runtime_dir / "PrintIFG.txt");
    if (debug) asm_gen.PrintFinalIR(runtime_dir / "PrintFinal.txt");
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

auto MainProxy(int argc, const char* argv[]) -> int {
  try {
    spdlog::set_pattern("[%H:%M:%S.%e] [%l] [%s:%#] %v");
    cxxopts::Options options("L1 Compiler", "A toy compiler using Bison");

    options.add_options()("d,debug", "Enable debug output")(
        "r64", "64 Bit Mode")("i,input", "Input file",
                              cxxopts::value<std::string>())(
        "o,output", "Output file", cxxopts::value<std::string>())(
        "h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help") != 0U) {
      std::cout << options.help() << '\n';
      return 0;
    }

    auto debug = result["debug"].as<bool>();
    auto r64 = result["r64"].as<bool>();
    auto input = result["input"].as<std::string>();
    auto output = result["output"].as<std::string>();

    if (debug) {
      spdlog::set_level(spdlog::level::debug);
    }

    auto [ret, runtime_path] = MakeRuntimeDirectory();
    if (!ret) return -1;

    if (!CheckInputOutput(result, input, output)) return -1;

    return CompileSourceCode(runtime_path, input, output, debug, r64);

  } catch (std::runtime_error& e) {
    SPDLOG_ERROR("runtime error: {}", e.what());
    return -1;
  }

  return 0;
}
