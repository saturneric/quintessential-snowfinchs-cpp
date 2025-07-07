#pragma once

#include "model/IRInstruction.h"
#include "model/Symbol.h"

auto IsImmediate(const SymbolPtr& s) -> bool;

auto IsVariable(const SymbolPtr& s) -> bool;

auto SymLoc(const SymbolPtr& op) -> std::string;

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstructionPtr>& instructions);

auto Trim(const std::string& str) -> std::string;

auto EndsWith(const std::string& str, const std::string& suffix) -> bool;
