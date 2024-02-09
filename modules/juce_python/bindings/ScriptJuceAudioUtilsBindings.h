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

#pragma once

#if !JUCE_MODULE_AVAILABLE_juce_audio_utils
 #error This binding file requires adding the juce_audio_utils module in the project
#else
 #include <juce_audio_utils/juce_audio_utils.h>
#endif

#include "ScriptJuceGuiBasicsBindings.h"
#include "ScriptJuceAudioBasicsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../utilities/PythonInterop.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceAudioUtilsBindings (pybind11::module_& m);

// =================================================================================================

template <class Base = juce::AudioAppComponent>
struct PyAudioAppComponent : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, prepareToPlay, samplesPerBlockExpected, sampleRate);
    }

    void releaseResources() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, releaseResources);
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, getNextAudioBlock, bufferToFill);
    }
};

} // namespace popsicle::Bindings
