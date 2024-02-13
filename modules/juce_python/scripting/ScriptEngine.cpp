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

#include "ScriptEngine.h"
#include "ScriptBindings.h"
#include "ScriptException.h"
#include "ScriptUtilities.h"

#include <regex>

namespace popsicle {

namespace py = pybind11;

namespace {

// =================================================================================================

[[maybe_unused]] juce::String replaceBrokenLineNumbers (const juce::String& input, const juce::String& code)
{
    static const std::regex pattern ("<string>\\((\\d+)\\)");

    const auto codeLines = juce::StringArray::fromLines (code);

    juce::String output;
    std::string result = input.toStdString();
    std::ptrdiff_t startPos = 0;

    std::smatch match;
    while (std::regex_search (result, match, pattern))
    {
        if (match.size() > 1)
        {
            const int matchLine = std::stoi (match[1]) - 1;

            output
                << input.substring (static_cast<int> (startPos), static_cast<int> (match.position() - startPos))
                << "<string>(" << matchLine << "): \'" << codeLines[matchLine - 1] << "\'";

            startPos = match.position() + match.length();
        }

        result = match.suffix();
    }

    output << result;
    return output;
}

} // namespace

// =================================================================================================

std::unique_ptr<PyConfig> ScriptEngine::prepareScriptingHome (
    const juce::String& programName,
    const juce::File& destinationFolder,
    std::function<const void*(const char*, int&)> standardLibraryCallback,
    bool forceInstall)
{
    juce::String pythonVersion, pythonVersionNoDot;
    pythonVersion << "python" << PY_MAJOR_VERSION << "." << PY_MINOR_VERSION;
    pythonVersionNoDot << "python" << PY_MAJOR_VERSION << PY_MINOR_VERSION;

    if (! destinationFolder.isDirectory())
        destinationFolder.createDirectory();

    auto libFolder = destinationFolder.getChildFile ("lib");
    if (! libFolder.isDirectory())
        libFolder.createDirectory();

    auto pythonFolder = libFolder.getChildFile (pythonVersion);
    if (! pythonFolder.isDirectory())
        pythonFolder.createDirectory();

    if (forceInstall && pythonFolder.getNumberOfChildFiles (juce::File::findFilesAndDirectories) > 0)
    {
        pythonFolder.deleteRecursively();
        pythonFolder.createDirectory();
    }

    if (! pythonFolder.getChildFile ("lib-dynload").isDirectory())
    {
        int dataSizeInBytes = 0;
        const void* data = standardLibraryCallback (pythonVersionNoDot.toRawUTF8(), dataSizeInBytes);

        auto mis = juce::MemoryInputStream (data, static_cast<size_t> (dataSizeInBytes), false);

        auto zip = juce::ZipFile (mis);
        zip.uncompressTo (pythonFolder);
    }

    auto config = std::make_unique<PyConfig>();

    PyConfig_InitPythonConfig (config.get());
    config->parse_argv = 0;
    config->isolated = 1;
    config->install_signal_handlers = 0;
    config->program_name = Py_DecodeLocale (programName.toRawUTF8(), nullptr);
    config->home = Py_DecodeLocale (destinationFolder.getFullPathName().toRawUTF8(), nullptr);

    return config;
}

// =================================================================================================

ScriptEngine::ScriptEngine ()
    : ScriptEngine (juce::StringArray{})
{
}

ScriptEngine::ScriptEngine (std::unique_ptr<PyConfig> config)
    : ScriptEngine (juce::StringArray{}, std::move (config))
{
}

ScriptEngine::ScriptEngine (juce::StringArray modules, std::unique_ptr<PyConfig> config)
    : customModules (std::move (modules))
{
    if (config)
        pybind11::initialize_interpreter (config.get(), 0, nullptr, false);
    else
        pybind11::initialize_interpreter();

    py::set_shared_data ("_ENGINE", this);
}

ScriptEngine::~ScriptEngine()
{
    py::set_shared_data ("_ENGINE", nullptr);

    pybind11::finalize_interpreter();
}

// =================================================================================================

juce::Result ScriptEngine::runScript (const juce::String& code, py::dict locals, py::dict globals)
{
    currentScriptCode = code;
    currentScriptFile = juce::File();

    return runScriptInternal (currentScriptCode, std::move (globals), std::move (locals));
}

// =================================================================================================

juce::Result ScriptEngine::runScript (const juce::File& script, py::dict locals, py::dict globals)
{
    {
        auto is = script.createInputStream();
        if (is == nullptr)
            return juce::Result::fail ("Unable to open the requested script file");

        currentScriptCode = is->readEntireStreamAsString();
        currentScriptFile = script;
    }

    return runScriptInternal (currentScriptCode, std::move (globals), std::move (locals));
}

// =================================================================================================

juce::Result ScriptEngine::runScriptInternal (const juce::String& code, py::dict locals, py::dict globals)
{
#if JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
    try
#endif

    {
        py::gil_scoped_acquire acquire;

        [[maybe_unused]] const auto redirectStreamsUntilExit = ScriptStreamRedirection();

        for (const auto& m : customModules)
            globals [m.toRawUTF8()] = py::module_::import (m.toRawUTF8());

        py::str pythonCode { code.toRawUTF8(), code.getNumBytesAsUTF8() };

        py::exec (std::move (pythonCode), std::move (globals), std::move (locals));

        return juce::Result::ok();
    }

#if JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
    catch (const py::error_already_set& e)
    {
        return juce::Result::fail (replaceBrokenLineNumbers (e.what(), code));
    }
    catch (...)
    {
        return juce::Result::fail ("Unhandled exception while processing script");
    }
#endif
}

} // namespace popsicle
