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

#pragma once

#include <juce_core/juce_core.h>

#include <exception>

namespace popsicle {

//=================================================================================================

/**
 * @brief Custom exception class for handling script-related errors.
 *
 * The `ScriptException` class is derived from `std::exception` and is designed to be used for throwing exceptions when errors
 * occur during script execution. It provides a way to encapsulate and convey error messages in a human-readable format.
 *
 * @warning It is important to note that this class should be used judiciously and only for exceptional situations in script execution.
 *
 * @param msg A string containing the error message to be associated with the exception.
 */
class ScriptException : public std::exception
{
public:
    /**
     * @brief Constructs a `ScriptException` with the specified error message.
     *
     * This constructor initializes the `ScriptException` object with the provided error message.
     *
     * @param msg The error message to be associated with the exception.
     */
    ScriptException (juce::String msg)
        : message (std::move (msg))
    {
    }

    /**
     * @brief Returns a C-string representing the error message associated with the exception.
     *
     * The `what` method is overridden from the `std::exception` base class to provide access to the error message stored within
     * this `ScriptException`.
     *
     * @return A pointer to a C-string containing the error message.
     */
    const char* what() const noexcept override
    {
        return message.toUTF8();
    }

private:
    juce::String message;
};

} // namespace popsicle
