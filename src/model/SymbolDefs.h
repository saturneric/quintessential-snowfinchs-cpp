#pragma once

enum class SymbolType : uint8_t {
  kNONE,
  kAST,
  kDEFINE,
  kIR,
  kFUNCTION,
};

using SymbolMetaKeySizeType = uint8_t;

enum class SymbolMetaKey : SymbolMetaKeySizeType {
  kNONE = 0,
  kSCOPE,
  kTYPE,
  kHAS_INIT,
  kIN_LOOP,
  kIS_SPILLED,
  kIN_REGISTER,
  kIN_STACK,
  kLOCATION,
  kSSA_VERSION,
  kSSA_LATEST_SYM,
  kSSA_ORIGIN_SYM,
  kCONTINUE_LABEL,
  kBREAK_LABEL,
  kDEF_SYMBOL,
  kWILL_RETURN,
  kWILL_BREAK,
  kPARAM_INDEX,
  kRETURN_TYPE,
  kPARAM_TYPES,
  kCOUNT,
};