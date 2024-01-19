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

#include "../utilities/PyBind11Includes.h"

#include <functional>

namespace popsicle {

//=================================================================================================

/**
 * @brief The ScriptEngine class provides a C++ interface for running Python scripts
 *
 * The ScriptEngine class encapsulates the functionality to run Python scripts within a C++ application. It utilizes the pybind11 library to
 * interact with the Python interpreter and provides a way to run custom Python code within the C++ environment.
 */
class ScriptEngine
{
public:
    /**
     * @brief Construct a new ScriptEngine object.
     *
     * Initializes a ScriptEngine object.
     */
    ScriptEngine();

    /**
     * @brief Construct a new ScriptEngine object.
     *
     * Initializes a ScriptEngine object with the specified custom modules.
     *
     * @param modules An array of module names to be imported in the Python interpreter.
     *
     * @warning Ensure that the provided modules are available and compatible with the Python interpreter.
     */
    ScriptEngine (juce::StringArray modules);

    /**
     * @brief Destroy the ScriptEngine object.
     *
     * Cleans up resources associated with the ScriptEngine object.
     */
    ~ScriptEngine();

    /**
     * @brief Run a Python script.
     *
     * Executes the given Python code within the Python interpreter.
     *
     * @param code The Python code to be executed.
     * @param locals A python dictionary containing local variables.
     * @param globals A python dictionary containing global variables.
     *
     * @return A Result object indicating the success or failure of the script execution.
     */
    juce::Result runScript (const juce::String& code, pybind11::dict locals = {}, pybind11::dict globals = pybind11::globals());

    /**
     * @brief Run a Python script file.
     *
     * Executes the given Python file within the Python interpreter.
     *
     * @param script The Python file to be executed.
     * @param locals A python dictionary containing local variables.
     * @param globals A python dictionary containing global variables.
     *
     * @return A Result object indicating the success or failure of the script execution.
     */
    juce::Result runScript (const juce::File& script, pybind11::dict locals = {}, pybind11::dict globals = pybind11::globals());

private:
    juce::Result runScriptInternal (const juce::String& code, pybind11::dict locals, pybind11::dict globals);

    pybind11::scoped_interpreter pythonInterpreter;
    juce::StringArray customModules;
    juce::String currentScriptCode;
    juce::File currentScriptFile;

    JUCE_DECLARE_WEAK_REFERENCEABLE (ScriptEngine)
};

} // namespace popsicle
