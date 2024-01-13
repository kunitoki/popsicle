/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#include "ClassDemangling.h"

#include <cstddef>
#include <ciso646>

#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(__GLIBCPP__)
#include <cxxabi.h>
#else
#include <windows.h>
#include <dbghelp.h>

#pragma comment (lib, "dbghelp.lib")

using malloc_func_t = void* (*)(size_t);
using free_func_t = void (*)(void*);

extern "C" char* __unDName (char*, const char*, int, malloc_func_t, free_func_t, unsigned short int);
#endif

namespace popsicle::Helpers {

//=================================================================================================

juce::String demangleClassName (juce::StringRef className)
{
    juce::String name = className;

#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(__GLIBCPP__)
    int status = -1;
    char* demangledName = abi::__cxa_demangle (name.toUTF8(), nullptr, nullptr, &status);
    name = juce::String::fromUTF8 (demangledName);
    std::free (demangledName);

#else
    char demangledName [1024] = { 0 };
    __unDName (demangledName, name.toUTF8() + 1, juce::numElementsInArray (demangledName), malloc, free, 0x2800);
    name = juce::String::fromUTF8 (demangledName);

#endif

    return name;
}

} // namespace popsicle::Helpers
