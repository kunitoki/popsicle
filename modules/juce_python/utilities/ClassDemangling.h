/**
 * juce_python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include <juce_core/juce_core.h>

#include <functional>

namespace popsicle::Helpers {

//=================================================================================================

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

} // namespace popsicle::Helpers
