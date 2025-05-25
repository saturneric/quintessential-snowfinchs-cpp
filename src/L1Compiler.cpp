#include "driver/API.h"

// cxxpots
#include <cxxopts.hpp>

// spdlog
#include <spdlog/spdlog.h>

// std
#include <fstream>
#include <iostream>

namespace {

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
}  // namespace

auto main(int argc, const char* argv[]) -> int {
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
