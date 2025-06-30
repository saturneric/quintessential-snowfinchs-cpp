#pragma once

enum class ASTNodeType : uint8_t {
  kPROGRAM,
  kBLOCK,
  kASSIGN,
  kDECLARE,
  kBIN_OP,
  kUN_OP,
  kRETURN,
  kTYPE,
  kVALUE,
  kIDENT,
  kIF,
  kCOND_EXP,
  kWHILE,
  kBREAK,
  kCONTINUE,
  kFUNCTION,
  kPARAM_LIST,
  kPARAM,
  kCALL,
  kARG_LIST,
};

enum class ASTNodeTag : uint8_t {
  kNONE = 0,
  kINIT,
  kCOND,
  kSTEP,
  kBODY,
  kPARAMS,
};