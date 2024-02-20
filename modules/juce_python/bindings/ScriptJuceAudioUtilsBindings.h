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

// =================================================================================================

template <class Base = juce::AudioThumbnailBase>
struct PyAudioThumbnailBase : Base
{
    using Base::Base;

    void clear() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, clear);
    }

    bool setSource (juce::InputSource* newSource) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, setSource, newSource);
    }

    void setReader (juce::AudioFormatReader* newReader, juce::int64 hashCode) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setReader, newReader, hashCode);
    }

    bool loadFrom (juce::InputStream& input) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, loadFrom, input);
    }

    void saveTo (juce::OutputStream& output) const override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, saveTo, output);
    }

    int getNumChannels() const noexcept override
    {
        PYBIND11_OVERRIDE_PURE (int, Base, getNumChannels);
    }

    double getTotalLength() const noexcept override
    {
        PYBIND11_OVERRIDE_PURE (double, Base, getTotalLength);
    }

    void drawChannel (juce::Graphics& g,
                      const juce::Rectangle<int>& area,
                      double startTimeSeconds,
                      double endTimeSeconds,
                      int channelNum,
                      float verticalZoomFactor) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawChannel"); override_)
        {
            override_ (std::addressof (g), area, startTimeSeconds, endTimeSeconds, channelNum, verticalZoomFactor);

            return;
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"AudioThumbnailBase::drawChannel\"");
    }

    void drawChannels (juce::Graphics& g,
                       const juce::Rectangle<int>& area,
                       double startTimeSeconds,
                       double endTimeSeconds,
                       float verticalZoomFactor) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawChannels"); override_)
        {
            override_ (std::addressof (g), area, startTimeSeconds, endTimeSeconds, verticalZoomFactor);

            return;
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"AudioThumbnailBase::drawChannels\"");
    }

    bool isFullyLoaded() const noexcept override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, isFullyLoaded);
    }

    juce::int64 getNumSamplesFinished() const noexcept override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, Base, getNumSamplesFinished);
    }

    float getApproximatePeak() const override
    {
        PYBIND11_OVERRIDE_PURE (float, Base, getApproximatePeak);
    }

    void getApproximateMinMax (double startTime, double endTime, int channelIndex,
                               float& minValue, float& maxValue) const noexcept override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<const Base*> (this), "getApproximateMinMax"); override_)
        {
            auto results = override_ (startTime, endTime, channelIndex).cast<pybind11::tuple>();

            if (results.size() != 2)
                pybind11::pybind11_fail("Invalid return type of function \"AudioThumbnailBase::getApproximateMinMax\" must be tuple[float, 2]");

            minValue = results[0].cast<float>();
            maxValue = results[1].cast<float>();

            return;
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"AudioThumbnailBase::getApproximateMinMax\"");
    }

    juce::int64 getHashCode() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::int64, Base, getHashCode);
    }

    void reset (int numChannels, double sampleRate, juce::int64 totalSamplesInSource) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, reset, numChannels, sampleRate, totalSamplesInSource);
    }

    void addBlock (juce::int64 sampleNumberInSource, const juce::AudioBuffer<float>& newData, int startOffsetInBuffer, int numSamples) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, addBlock, sampleNumberInSource, newData, startOffsetInBuffer, numSamples);
    }
};

} // namespace popsicle::Bindings
