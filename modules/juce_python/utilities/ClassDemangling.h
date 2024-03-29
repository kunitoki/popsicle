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

#pragma once

#include <juce_core/juce_core.h>

#include <functional>

namespace popsicle::Helpers {

// =================================================================================================

/**
 * @brief Demangle a C++ class name.
 *
 * This function takes a StringRef containing a mangled class name and returns the demangled class name as a String.
 *
 * @param className The mangled class name to demangle.
 *
 * @return A String containing the demangled class name.
 */
juce::String demangleClassName (juce::StringRef className);

// =================================================================================================

/**
 * @brief Demangle a C++ class name and pythonize it.
 *
 * This function takes a StringRef containing a mangled class name and returns the demangled and pythonized class name as a String.
 *
 * @param className The mangled class name to demangle and pythonize.
 *
 * @return A String containing the demangled and pythonized class name.
 */
juce::String pythonizeClassName (juce::StringRef className, int maxTemplateArgs = -1);

// =================================================================================================

/**
 * @brief Demangle a C++ class name and pythonize it by compunding to another class name.
 *
 * @param prefixName The prefix to apply to the class name.
 * @param className The mangled class name to demangle and pythonize.
 *
 * @return A String containing the demangled and pythonized class name with a prefix.
 */
juce::String pythonizeCompoundClassName (juce::StringRef prefixName, juce::StringRef className, int maxTemplateArgs = -1);

// =================================================================================================

/**
 * @brief Demangle a C++ class name and pythonize it by making it part of a module.
 *
 * @param moduleName The name of the module to prepend.
 * @param className The mangled class name to demangle and pythonize.
 *
 * @return A String containing the demangled and pythonized class name belonging to a module.
 */
juce::String pythonizeModuleClassName (juce::StringRef moduleName, juce::StringRef className, int maxTemplateArgs = -1);

} // namespace popsicle::Helpers
