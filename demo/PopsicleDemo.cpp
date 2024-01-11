/**
 * popsicle - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "PopsicleDemo.h"

//=================================================================================================

PYBIND11_EMBEDDED_MODULE(popsicle, m)
{
    namespace py = pybind11;

    py::module_::import ("juce");

    py::class_<PopsicleDemo, juce::Component> (m, "PopsicleDemo");
}

//=================================================================================================

PopsicleDemo::PopsicleDemo()
{
    pybind11::dict locals;
    locals["juce"] = pybind11::module_::import ("popsicle");
    locals["this"] = pybind11::cast (this);

    engine.runScript (R"(
# An example of scriptable self
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
    g.drawText ("Hello, Popsicle!", getLocalBounds(), juce::Justification::centred, true);
}

void PopsicleDemo::resized()
{
}
