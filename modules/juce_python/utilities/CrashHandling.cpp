/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
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

#include "CrashHandling.h"

#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(__GLIBCPP__)
#include <cxxabi.h>
#include <execinfo.h>
#include <dlfcn.h>
#else
#include "WindowsIncludes.h"
#endif

namespace popsicle::Helpers {

// =================================================================================================

juce::String getStackBacktrace()
{
    juce::String result;

   #if JUCE_WINDOWS
    HANDLE process = GetCurrentProcess();
    SymInitialize (process, nullptr, TRUE);

    void* stack[128];
    int frames = static_cast<int> (CaptureStackBackTrace (0, juce::numElementsInArray (stack), stack, nullptr));

    juce::HeapBlock<SYMBOL_INFO> symbol;
    symbol.calloc (sizeof (SYMBOL_INFO) + 256, 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof (SYMBOL_INFO);

    for (int i = 0; i < frames; ++i)
    {
        DWORD64 displacement = 0;

        if (SymFromAddr (process, reinterpret_cast<DWORD64> (stack[i]), &displacement, symbol))
        {
            result << i << ": ";

            IMAGEHLP_MODULE64 moduleInfo;
            juce::zerostruct (moduleInfo);
            moduleInfo.SizeOfStruct = sizeof (moduleInfo);

            if (::SymGetModuleInfo64 (process, symbol->ModBase, &moduleInfo))
                result << moduleInfo.ModuleName << ": ";

            result << symbol->Name << " + 0x" << juce::String::toHexString (static_cast<juce::int64> (displacement)) << juce::newLine;
        }
    }

   #else
    void* stack[128];
    auto frames = backtrace (stack, juce::numElementsInArray (stack));
    char** frameStrings = backtrace_symbols (stack, frames);

    for (int i = 0; i < frames; ++i)
    {
        Dl_info info;
        if (dladdr (stack[i], &info))
        {
            int status = 0;

            std::unique_ptr<char, decltype (::free)*> demangled (abi::__cxa_demangle (info.dli_sname, nullptr, nullptr, &status), ::free);
            if (status == 0)
            {
                result
                    << juce::String (i).paddedRight (' ', 3)
                    << " " << juce::File (juce::String (info.dli_fname)).getFileName().paddedRight (' ', 35)
                    << " 0x" << juce::String::toHexString (reinterpret_cast<size_t> (stack[i])).paddedLeft ('0', sizeof (void*) * 2)
                    << " " << demangled.get()
                    << " + " << (reinterpret_cast<char*> (stack[i]) - reinterpret_cast<char*> (info.dli_saddr)) << juce::newLine;

                continue;
            }
        }

        result << frameStrings[i] << juce::newLine;
    }

    ::free (frameStrings);
   #endif

    return result;
}

// =================================================================================================

void applicationCrashHandler ([[maybe_unused]] void* stackFrame)
{
    juce::Logger::getCurrentLogger()->outputDebugString (getStackBacktrace());
}

} // namespace popsicle::Helpers
