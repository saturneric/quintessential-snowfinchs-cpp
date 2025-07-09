# L4Compiler

L4Compiler is a simple L4 language compiler written in C++17, implementing the
full compilation pipeline—from lexical analysis to assembly code emission.
Developed through [four lab stages](labs/), it progressively adds support for
advanced language features, optimization passes, and runtime checks.

## Features

* Flex based L4 lexer (`Lexer.l`).
* Bison-generated LALR(1) parser (`Parser.y`) producing an Abstract Syntax Tree
  (AST).
* Static type checking, scope management, symbol tables, and memory-safety
  validation.
* Custom IR with Control‑Flow Graph (CFG), supporting optimization passes such
  as function inlining, dead code elimination, and variable coalescing.
* Register Allocation: Graph‑coloring allocator with spill handling and
  calling‑convention support.
* IR-to-assembly translation via X86Translator.
* ASMGenerator emits AT&T‑syntax x86-32 / x86-64 code with Position‑Independent
  Code (PIC) support.
* Perform simple optimizations during code generation
* Generate AT&T-syntax x86-32 and x86-64 assembly output
* Provide a command-line interface for compiling source files
* Include unit tests powered by GoogleTest
* Support JSON-based configuration and resource loading for individual unit test
  cases

## Repository Structure

```
L4Compiler/
├── .github/              # CI workflows (GitHub Actions)
├── build.sh              # Convenience script: configure & build
├── run.sh                # Wrapper: compile source → binary
├── test.sh               # Wrapper: run unit tests
├── CMakeLists.txt        # Root CMake configuration
├── include/              # Vendored headers (FlexLexer, cxxopts, nlohmann/json, spdlog, Boost)
├── src/                  # Core compiler implementation
│   ├── core/             # Parser, AST, CFG, IR, semantic analyzer, ASMGenerator
│   ├── model/            # AST & IR data structures, CFG basic blocks
│   ├── handlers/         # IR generation and translation handlers
│   ├── driver/           # CLI entrypoint & orchestration (API.cpp)
│   ├── L4Compiler.cpp    # Main wrapper
│   └── CMakeLists.txt    # Add subdirectories & define `compiler` target
├── test/                 # GoogleTest suite (FetchContent for gtest)
├── resources/            # JSON configs and test inputs
└── COPYING               # GPL-3.0 License
```

## Prerequisites

* A C++17-compatible compiler (e.g., g++, clang++)
* CMake 3.17 or higher
* Bash (for the provided build and run scripts)

## Installation

1. Clone the repository

2. Build with CMake:

   ```bash
   mkdir build
   cd build
   cmake -DBUILD_TEST_CASES=ON [-DBUILD_UNITY=ON] .. 
   make
   ```

3. Or use the helper scripts:

   ```bash
   ./build.sh                # Configure and build
   ```

## Usage

Compile a source file:

```bash
# Direct invocation:
build/bin/compiler -i path/to/source.txt -o path/to/output_binary [--r64] [-d]

# Using provided wrapper scripts:
./run.sh path/to/source.txt path/to/output_binary
./test.sh
```

Run the full test suite:

```bash
./test.sh
```

## Configuration

JSON Tests: Located under `resources/tests/`, where each file defines inputs and
expected outputs.

## Dependencies

* [cxxopts](https://github.com/jarro2783/cxxopts) – Lightweight C++ command line
  parser
* [nlohmann/json](https://github.com/nlohmann/json) – JSON library for modern
  C++
* [spdlog](https://github.com/gabime/spdlog) – Fast C++ logging library
* [fmtlib](https://github.com/fmtlib/fmt) – Formatting library (bundled with
  spdlog)
* [GNU Flex](https://github.com/westes/flex) – Lexical analyzer generator
* [GNU Bison](https://www.gnu.org/software/bison/) – Parser generator
* [GoogleTest](https://github.com/google/googletest) – Unit testing framework
* [Boost](https://www.boost.org/) – A collection of peer-reviewed, portable C++
  libraries covering common tasks such as smart pointers, data structures,
  algorithms, threading, serialization, and more.

## Resources

All test cases and their associated configurations are located in the
`resources/` folder.

## Contributing

Contributions, issues, and feature requests are welcome! Please open an issue or
submit a pull request on GitHub.
