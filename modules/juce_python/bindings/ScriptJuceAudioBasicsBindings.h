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

#if !JUCE_MODULE_AVAILABLE_juce_audio_basics
 #error This binding file requires adding the juce_audio_basics module in the project
#else
 #include <juce_audio_basics/juce_audio_basics.h>
#endif

#include "../utilities/PyBind11Includes.h"

// #include "ScriptJuceGuiBasicsBindings.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceAudioBasicsBindings (pybind11::module_& m);

// =================================================================================================

template <class T>
struct PyArrayView
{
    PyArrayView() = default;

    PyArrayView (T* values, size_t numValues) noexcept
        : values (values)
        , numValues (numValues)
    {
    }

    T* data() noexcept
    {
        return values;
    }

    template <class U = T>
    auto data() const noexcept -> std::enable_if_t<! std::is_const_v<U>, const T*>
    {
        return values;
    }

    size_t size() const noexcept
    {
        return numValues;
    }

private:
    T* values = nullptr;
    size_t numValues = 0;
};

// =================================================================================================

struct PyAudioPlayHead : juce::AudioPlayHead
{
    using juce::AudioPlayHead::AudioPlayHead;

    juce::Optional<juce::AudioPlayHead::PositionInfo> getPosition() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Optional<juce::AudioPlayHead::PositionInfo>, juce::AudioPlayHead, getPosition);
    }

    bool canControlTransport() override
    {
        PYBIND11_OVERRIDE (bool, juce::AudioPlayHead, canControlTransport);
    }

    void transportPlay (bool shouldStartPlaying) override
    {
        PYBIND11_OVERRIDE (void, juce::AudioPlayHead, transportPlay, shouldStartPlaying);
    }

    void transportRecord (bool shouldStartRecording) override
    {
        PYBIND11_OVERRIDE (void, juce::AudioPlayHead, transportRecord, shouldStartRecording);
    }

    void transportRewind() override
    {
        PYBIND11_OVERRIDE (void, juce::AudioPlayHead, transportRewind);
    }
};

// =================================================================================================

template <class Base = juce::AudioSource>
struct PyAudioSource : Base
{
    using Base::Base;

    void prepareToPlay (int newSamplesPerBlockExpected, double newSampleRate) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, prepareToPlay, newSamplesPerBlockExpected, newSampleRate);
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

template <class Base = juce::PositionableAudioSource>
struct PyPositionableAudioSource : PyAudioSource<Base>
{
    using PyAudioSource<Base>::PyAudioSource;

    void setNextReadPosition (juce::int64 newPosition) override
    {
        PYBIND11_OVERRIDE_PURE (void, PyAudioSource<Base>, setNextReadPosition, newPosition);
    }

    juce::int64 getNextReadPosition() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, PyAudioSource<Base>, getNextReadPosition);
    }

    juce::int64 getTotalLength() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, PyAudioSource<Base>, getTotalLength);
    }

    bool isLooping() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, PyAudioSource<Base>, isLooping);
    }
};

} // namespace popsicle::Bindings
