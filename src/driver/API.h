#pragma once

#include <filesystem>

namespace fs = std::filesystem;

auto MakeRuntimeDirectory() -> std::tuple<bool, fs::path>;

auto CompileSourceCode(fs::path runtime_dir, fs::path input, fs::path output,
                       bool debug, bool r64) -> int;

auto MainProxy(int argc, const char* argv[]) -> int;