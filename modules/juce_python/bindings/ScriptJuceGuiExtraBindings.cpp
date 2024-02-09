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

#include "ScriptJuceGuiExtraBindings.h"
#include "../utilities/ClassDemangling.h"

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

void registerJuceGuiExtraBindings (py::module_& m)
{
    // ============================================================================================ juce::AnimatedAppComponent

    py::class_<AnimatedAppComponent, Component, PyAnimatedAppComponent<>> classAnimatedAppComponent (m, "AnimatedAppComponent");

    classAnimatedAppComponent
        .def (py::init<>())
        .def ("setFramesPerSecond", &AnimatedAppComponent::setFramesPerSecond)
        .def ("setSynchroniseToVBlank", &AnimatedAppComponent::setSynchroniseToVBlank)
        .def ("update", &AnimatedAppComponent::update)
        .def ("getFrameCounter", &AnimatedAppComponent::getFrameCounter)
        .def ("getMillisecondsSinceLastUpdate", &AnimatedAppComponent::getMillisecondsSinceLastUpdate)
    ;
}

} // namespace popsicle::Bindings
