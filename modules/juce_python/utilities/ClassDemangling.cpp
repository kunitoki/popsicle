/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "ClassDemangling.h"

#if __clang__ || GNUC
#include <cxxabi.h>
#else
#include <windows.h>
#include <dbghelp.h>
#pragma comment (lib, "dbghelp.lib")
extern char* __unDName (char*, const char*, int, void*, void*, int);
#endif

namespace jucepy::Helpers {

//=================================================================================================

juce::String demangleClassName (juce::StringRef className)
{
    juce::String name = className;

#if __clang__ || GNUC
    int status = -1;
    char* demangledName = abi::__cxa_demangle (name.toUTF8(), nullptr, nullptr, &status);
    name = demangledName;
    std::free (demangledName);
#else
    char demangledName [1024] = { 0 };
    __unDName (demangledName, name.toUTF8() + 1, juce::numElementsInArray (demangledName), malloc, free, 0x2800);
    name = demangledName;
#endif

    return name;
}

} // namespace jucepy::Helpers
