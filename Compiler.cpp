#include <ctime>
#include <iostream>

#include "ASMGenerator.h"
#include "AST.h"
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

    Lexer lexer(symbol_table);
    lexer.LoadTokenSpecs("TokenSpecs.txt");
    lexer.Lex(argv[1]);
    lexer.Print("PrintLexer.txt");

    end = clock();
    double times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::wcout << "Lexer Run time = " << times << "s MicroSeconds"
               << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    LR1Generator generator(lexer);
    generator.Generate("SyntaxInput.txt");
    generator.Print("PrintLR1Table.txt");
    generator.PrintCanonicalCollection("PrintCCollection.txt");

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "LR(1) Generator Run time = " << times << "s MicroSeconds"
              << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    SyntaxParser syntax_parser(lexer, generator);
    syntax_parser.Parse();
    syntax_parser.PrintAnalyse("PrintSyntaxAnalyse.txt");
    syntax_parser.PrintTree("PrintSyntaxTree.txt");

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Syntax Parser Run time = " << times << "s MicroSeconds "
              << " = " << times * 1000 << "ms" << "\n";

    start = clock();

    AST ast(symbol_table);
    ast.LoadBinding("AST.txt");
    ast.Build(syntax_parser.Tree());
    ast.Print("PrintAST.txt");

    end = clock();
    times = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "AST Builder Run time = " << times << "s MicroSeconds "
              << " = " << times * 1000 << "ms" << "\n";

    IRGenerator irg;
    auto ir = irg.Generate(ast);
    irg.Print3Addr("PrintIR3.txt");
    irg.Print2Addr("PrintIR2.txt");

    ASMGenerator asm_gen(ir);
    asm_gen.Generate("ASM.S");

  } catch (std::runtime_error &e) {
    std::cout << "Runtime Error: " << e.what() << "\n";
  }

  return 0;
}
