#pragma once

#include "IRInstruction.h"
#include "Symbol.h"

using Opera = std::function<bool()>;

auto IsVariable(const SymbolPtr& s) -> bool;

auto SafeParseInt(const std::string& text, int& result) -> bool;

auto RunOperation(const std::string& name, const Opera& f) -> bool;

auto SymLoc(const SymbolPtr& op) -> std::string;

void PrintInstructions(std::ostream& f,
                       const std::vector<IRInstructionPtr>& instructions);

void PrintInstructionA2s(std::ostream& f,
                         const std::vector<IRInstructionA2Ptr>& instructions);