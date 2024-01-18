/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2022 - kunitoki <kunitoki@gmail.com>
 *
 * popsicle is an open source library subject to commercial or open-source licensing.
 *
 * By using popsicle, you agree to the terms of the popsicle License Agreement, which can
 * be found at https://raw.githubusercontent.com/kunitoki/popsicle/master/LICENSE
 *
 * Or: You may also use this code under the terms of the GPL v3 (see www.gnu.org/licenses).
 *
 * POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER EXPRESSED
 * OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE DISCLAIMED.
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
    if (name.startsWith("class "))
    {
        char demangledName[1024] = { 0 };
        auto offset = name.startsWithChar (L'?') ? 1 : 0;
        __unDName(demangledName, name.toRawUTF8() + offset, juce::numElementsInArray(demangledName), malloc, free, 0x2800);
        name = juce::String::fromUTF8(demangledName).replace("class ", "");
    }

#endif

    return name;
}

//=================================================================================================

juce::String pythonizeClassName (juce::StringRef className)
{
    return demangleClassName (className)
        .replace ("juce::", "")
        .replace ("::", ".");
}

//=================================================================================================

juce::String pythonizeCompoundClassName (juce::StringRef prefixName, juce::StringRef className)
{
    const auto pythonizedName = pythonizeClassName (className);

    juce::String result;

    result
        << prefixName
        << pythonizedName.substring(0, 1).toUpperCase()
        << pythonizedName.substring(1);

    return result;
}

} // namespace popsicle::Helpers
