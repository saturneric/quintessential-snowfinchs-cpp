# L1Compiler

A simple L1 Compiler written in C++.

## Features

* Perform lexical analysis with Flex
* Generate LALR(1) parsers using Bison
* Provide a command-line interface for compiling source files
* Support JSON configuration and resource loading
* Include unit tests powered by GoogleTest

## Prerequisites

* A C++17-compatible compiler (e.g., g++, clang++)
* CMake 3.17 or higher
* Flex (lexical analyzer generator)
* Bison (parser generator)
* Bash (for the provided build and run scripts)

## Installation

1. Clone the repository

2. Build with CMake:

   ```bash
   mkdir build
   cd build
   cmake .. -DBUILD_TEST_CASES=ON
   make
   ```

3. Or use the helper scripts:

   ```bash
   ./build.sh       # Configure and build
   ./run.sh <file>  # Compile a source file
   ./test.sh        # Run unit tests
   ```

## Usage

Compile a source file:

```bash
./run.sh path/to/source.txt path/to/binary
```

Run the full test suite:

```bash
./test.sh
```

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

## Resources

All grammar definitions and test inputs are located in the `resources/` folder.

## Contributing

Contributions, issues, and feature requests are welcome! Please open an issue or
submit a pull request on GitHub.
