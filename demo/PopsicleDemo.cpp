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

#include "PopsicleDemo.h"

// =================================================================================================

PYBIND11_EMBEDDED_MODULE(custom, m)
{
    namespace py = pybind11;

    py::module_::import (popsicle::PythonModuleName);

    py::class_<PopsicleDemo, juce::Component> (m, "PopsicleDemo")
        .def_readwrite ("text", &PopsicleDemo::text);
}

// =================================================================================================

namespace {
void crashHandler ([[maybe_unused]] void* stack)
{
    DBG (juce::SystemStats::getStackBacktrace());
}
} // namespace

// =================================================================================================

PopsicleDemo::PopsicleDemo()
    : text ("Unkown")
    , engine (popsicle::ScriptEngine::prepareScriptingHome (
        juce::JUCEApplication::getInstance()->getApplicationName(),
        juce::File::getSpecialLocation (juce::File::tempDirectory),
        &BinaryData::getNamedResource))
{
    juce::SystemStats::setApplicationCrashHandler (crashHandler);

    pybind11::dict locals;
    locals["custom"] = pybind11::module_::import ("custom");
    locals["juce"] = pybind11::module_::import (popsicle::PythonModuleName);
    locals["this"] = pybind11::cast (this);

    auto result = engine.runScript (R"(
# An example of scriptable self
print("Scripting JUCE!")

this.text = "Popsicle"
this.setOpaque(True)
this.setSize(600, 300)
    )", locals);

    if (result.failed())
        std::cout << result.getErrorMessage();
}

PopsicleDemo::~PopsicleDemo()
{
}

void PopsicleDemo::paint (juce::Graphics& g)
{
    g.fillAll (findColour (juce::DocumentWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);

    juce::String finalText;
    finalText << "Hello, " << text << "!";

    g.drawText (finalText, getLocalBounds(), juce::Justification::centred, true);
}

void PopsicleDemo::resized()
{
}
