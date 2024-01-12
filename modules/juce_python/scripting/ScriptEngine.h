/**
 * juce_python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
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
     * @brief Construct a new ScriptEngine object
     *
     * Initializes a ScriptEngine object.
     */
    ScriptEngine();

    /**
     * @brief Construct a new ScriptEngine object
     *
     * Initializes a ScriptEngine object with the specified custom modules.
     *
     * @param modules An array of module names to be imported in the Python interpreter.
     *
     * @warning Ensure that the provided modules are available and compatible with the Python interpreter.
     */
    ScriptEngine (juce::StringArray modules);

    /**
     * @brief Destroy the ScriptEngine object
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
     *
     * @return A Result object indicating the success or failure of the script execution.
     */
    juce::Result runScript (const juce::String& code, pybind11::dict globals = {}, pybind11::dict locals = {});

    /**
     * @brief Run a Python script file.
     *
     * Executes the given Python file within the Python interpreter.
     *
     * @param script The Python file to be executed.
     *
     * @return A Result object indicating the success or failure of the script execution.
     */
    juce::Result runScript (const juce::File& script, pybind11::dict globals = {}, pybind11::dict locals = {});

private:
    juce::Result runScriptInternal (const juce::String& code, pybind11::dict globals = {}, pybind11::dict locals = {});

    pybind11::scoped_interpreter& pythonEngine;
    juce::StringArray customModules;
    juce::String currentScriptCode;
    juce::File currentScriptFile;

    JUCE_DECLARE_WEAK_REFERENCEABLE (ScriptEngine)
};

} // namespace popsicle
