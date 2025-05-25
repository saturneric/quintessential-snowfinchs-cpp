#include "Translators.h"

#include "X86Translator.h"

auto GetX86Translator(bool r32) -> TranslatorPtr {
  auto t = std::make_shared<X86Translator>(r32);
  return t;
}