/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#include "PopsicleDemo.h"

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(custom, m)
{
    namespace py = pybind11;

    py::module_::import ("popsicle");

    py::class_<PopsicleDemo, juce::Component> (m, "PopsicleDemo")
        .def_readwrite ("text", &PopsicleDemo::text);
}

//=================================================================================================

PopsicleDemo::PopsicleDemo()
    : text ("Unkown")
{
    pybind11::dict locals;
    locals["custom"] = pybind11::module_::import ("custom");
    locals["juce"] = pybind11::module_::import ("popsicle");
    locals["this"] = pybind11::cast (this);

    engine.runScript (R"(
# An example of scriptable self
this.text = "Popsicle"
this.setOpaque(True)
this.setSize(600, 300)
    )", pybind11::globals(), locals);
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
