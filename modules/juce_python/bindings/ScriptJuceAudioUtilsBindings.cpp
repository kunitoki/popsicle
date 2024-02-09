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

#include "ScriptJuceAudioUtilsBindings.h"

#include "ScriptJuceAudioDevicesBindings.h"

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

void registerJuceAudioUtilsBindings ([[maybe_unused]] py::module_& m)
{
    // ============================================================================================ juce::AudioAppComponent

    py::class_<AudioAppComponent, Component, PyAudioAppComponent<>> classAudioAppComponent (m, "AudioAppComponent");

    classAudioAppComponent
        .def (py::init<>())
        .def (py::init<AudioDeviceManager&>())
        .def ("setAudioChannels", &AudioAppComponent::setAudioChannels,
            "numInputChannels"_a, "numOutputChannels"_a, "storedSettings"_a = nullptr)
        .def ("prepareToPlay", &AudioAppComponent::prepareToPlay)
        .def ("releaseResources", &AudioAppComponent::releaseResources)
        .def ("getNextAudioBlock", &AudioAppComponent::getNextAudioBlock)
        .def ("shutdownAudio", &AudioAppComponent::shutdownAudio)
        .def_property_readonly("deviceManager", [](const AudioAppComponent& self)
        {
            return std::addressof (self.deviceManager);
        }, py::return_value_policy::reference)
    ;
}

} // namespace popsicle::Bindings
