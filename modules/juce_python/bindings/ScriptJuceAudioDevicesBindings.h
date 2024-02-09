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

#if !JUCE_MODULE_AVAILABLE_juce_audio_devices
 #error This binding file requires adding the juce_audio_devices module in the project
#else
 #include <juce_audio_devices/juce_audio_devices.h>
#endif

#include "ScriptJuceAudioBasicsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../utilities/PythonInterop.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceAudioDevicesBindings (pybind11::module_& m);

// =================================================================================================

struct PyAudioIODeviceType : juce::AudioIODeviceType
{
    using juce::AudioIODeviceType::AudioIODeviceType;

    explicit PyAudioIODeviceType (const juce::String& typeName)
        : juce::AudioIODeviceType (typeName)
    {
    }

    void scanForDevices() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioIODeviceType, scanForDevices);
    }

    juce::StringArray getDeviceNames (bool wantInputNames = false) const override
    {
        PYBIND11_OVERRIDE_PURE (juce::StringArray, juce::AudioIODeviceType, getDeviceNames, wantInputNames);
    }

    int getDefaultDeviceIndex (bool forInput) const override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODeviceType, getDefaultDeviceIndex, forInput);
    }

    int getIndexOfDevice (juce::AudioIODevice* device, bool asInput) const override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODeviceType, getIndexOfDevice, device, asInput);
    }

    bool hasSeparateInputsAndOutputs() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODeviceType, hasSeparateInputsAndOutputs);
    }

    juce::AudioIODevice* createDevice (const juce::String& outputDeviceName, const juce::String& inputDeviceName) override
    {
        PYBIND11_OVERRIDE_PURE (juce::AudioIODevice*, juce::AudioIODeviceType, createDevice, outputDeviceName, inputDeviceName);
    }
};

struct PyAudioIODeviceTypeListener : juce::AudioIODeviceType::Listener
{
    using juce::AudioIODeviceType::Listener::Listener;

    void audioDeviceListChanged() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioIODeviceType::Listener, audioDeviceListChanged);
    }
};

// =================================================================================================

template <class Base = juce::AudioIODeviceCallback>
struct PyAudioIODeviceCallback : Base
{
    using Base::Base;

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const juce::AudioIODeviceCallbackContext& context) override
    {
        pybind11::gil_scoped_acquire gil;

        if (! override_)
        {
            override_ = pybind11::get_override (static_cast<Base*> (this), "audioDeviceIOCallbackWithContext");
            if (! override_)
                return;
        }

        const auto numInputs = static_cast<size_t> (numInputChannels);

        pybind11::list inputs (numInputs);
        for (size_t i = 0; i < numInputs; ++i)
            inputs[i] = PyArrayView<const float> (inputChannelData[i], static_cast<size_t> (numSamples));

        const auto numOutputs = static_cast<size_t> (numOutputChannels);

        pybind11::list outputs (numOutputs);
        for (size_t i = 0; i < numOutputs; ++i)
            outputs[i] = PyArrayView<float> (outputChannelData[i], static_cast<size_t> (numSamples));

        override_ (inputs, numInputChannels, outputs, numOutputChannels, numSamples, context);
    }

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, audioDeviceAboutToStart, device);
    }

    void audioDeviceStopped() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, audioDeviceStopped);
    }

    void audioDeviceError (const juce::String& errorMessage) override
    {
        PYBIND11_OVERRIDE (void, Base, audioDeviceError, errorMessage);
    }

private:
    pybind11::function override_;
};

// =================================================================================================

struct PyAudioIODevice : juce::AudioIODevice
{
    using juce::AudioIODevice::AudioIODevice;

    PyAudioIODevice (const juce::String& deviceName, const juce::String& typeName)
        : juce::AudioIODevice (deviceName, typeName)
    {
    }

    juce::StringArray getOutputChannelNames() override
    {
        PYBIND11_OVERRIDE_PURE (juce::StringArray, juce::AudioIODevice, getOutputChannelNames);
    }

    juce::StringArray getInputChannelNames() override
    {
        PYBIND11_OVERRIDE_PURE (juce::StringArray, juce::AudioIODevice, getInputChannelNames);
    }

    std::optional<juce::BigInteger> getDefaultOutputChannels() const override
    {
        PYBIND11_OVERRIDE (std::optional<juce::BigInteger>, juce::AudioIODevice, getDefaultOutputChannels);
    }

    std::optional<juce::BigInteger> getDefaultInputChannels() const override
    {
        PYBIND11_OVERRIDE (std::optional<juce::BigInteger>, juce::AudioIODevice, getDefaultInputChannels);
    }

    juce::Array<double> getAvailableSampleRates() override
    {
        PYBIND11_OVERRIDE_PURE (juce::Array<double>, juce::AudioIODevice, getAvailableSampleRates);
    }

    juce::Array<int> getAvailableBufferSizes() override
    {
        PYBIND11_OVERRIDE_PURE (juce::Array<int>, juce::AudioIODevice, getAvailableBufferSizes);
    }

    int getDefaultBufferSize() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODevice, getDefaultBufferSize);
    }

    juce::String open (const juce::BigInteger& inputChannels, const juce::BigInteger& outputChannels, double sampleRate, int bufferSizeSamples) override
    {
        PYBIND11_OVERRIDE_PURE (juce::String, juce::AudioIODevice, open, inputChannels, outputChannels, sampleRate, bufferSizeSamples);
    }

    void close() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioIODevice, close);
    }

    bool isOpen() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODevice, isOpen);
    }

    void start (juce::AudioIODeviceCallback* callback) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioIODevice, start, callback);
    }

    void stop() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioIODevice, stop);
    }

    bool isPlaying() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODevice, isPlaying);
    }

    juce::String getLastError() override
    {
        PYBIND11_OVERRIDE_PURE (juce::String, juce::AudioIODevice, getLastError);
    }

    int getCurrentBufferSizeSamples() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODevice, getCurrentBufferSizeSamples);
    }

    double getCurrentSampleRate() override
    {
        PYBIND11_OVERRIDE_PURE (double, juce::AudioIODevice, getCurrentSampleRate);
    }

    int getCurrentBitDepth() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODevice, getCurrentBitDepth);
    }

    juce::BigInteger getActiveOutputChannels() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::BigInteger, juce::AudioIODevice, getActiveOutputChannels);
    }

    juce::BigInteger getActiveInputChannels() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::BigInteger, juce::AudioIODevice, getActiveInputChannels);
    }

    int getOutputLatencyInSamples() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODevice, getOutputLatencyInSamples);
    }

    int getInputLatencyInSamples() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::AudioIODevice, getInputLatencyInSamples);
    }

    juce::AudioWorkgroup getWorkgroup() const override
    {
        PYBIND11_OVERRIDE (juce::AudioWorkgroup, juce::AudioIODevice, getWorkgroup);
    }

    bool hasControlPanel() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODevice, hasControlPanel);
    }

    bool showControlPanel() override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODevice, showControlPanel);
    }

    bool setAudioPreprocessingEnabled (bool shouldBeEnabled) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::AudioIODevice, setAudioPreprocessingEnabled, shouldBeEnabled);
    }

    int getXRunCount() const noexcept override
    {
        PYBIND11_OVERRIDE (int, juce::AudioIODevice, getXRunCount);
    }
};

} // namespace popsicle::Bindings
