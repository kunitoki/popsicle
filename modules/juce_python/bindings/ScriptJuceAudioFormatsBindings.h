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

#if !JUCE_MODULE_AVAILABLE_juce_audio_formats
 #error This binding file requires adding the juce_audio_formats module in the project
#else
 #include <juce_audio_formats/juce_audio_formats.h>
#endif

#include "ScriptJuceAudioBasicsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include "../utilities/PythonInterop.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceAudioFormatsBindings (pybind11::module_& m);

// =================================================================================================

template <class Base = juce::AudioFormatReader>
struct PyAudioFormatReader : Base
{
    using Base::Base;

    PyAudioFormatReader (juce::InputStream* sourceStream, const juce::String& formatName)
        : Base (sourceStream, formatName)
    {
    }

    void readMaxLevels (juce::int64 startSample, juce::int64 numSamples,
                        juce::Range<float>* results, int numChannelsToRead) override
    {
        PYBIND11_OVERRIDE (void, Base, readMaxLevels, startSample, numSamples, results, numChannelsToRead);
    }

    void readMaxLevels (juce::int64 startSample, juce::int64 numSamples,
                        float& lowestLeft,  float& highestLeft,
                        float& lowestRight, float& highestRight) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "readMaxLevels"); override_)
        {
            auto results = override_ (startSample, numSamples).cast<pybind11::tuple>();

            if (results.size() != 4)
                pybind11::pybind11_fail("Invalid return type of function \"AudioFormatReader::readMaxLevels\" must be tuple[float, 4]");

            lowestLeft = results[0].cast<float>();
            highestLeft = results[1].cast<float>();
            lowestRight = results[2].cast<float>();
            highestRight = results[3].cast<float>();

            return;
        }

        Base::readMaxLevels (startSample, numSamples, lowestLeft, highestLeft, lowestRight, highestRight);
    }

    juce::AudioChannelSet getChannelLayout() override
    {
        PYBIND11_OVERRIDE (juce::AudioChannelSet, Base, getChannelLayout);
    }

    bool readSamples (int* const* destChannels,
                      int numDestChannels,
                      int startOffsetInDestBuffer,
                      juce::int64 startSampleInFile,
                      int numSamples) override
    {
        juce::ignoreUnused (destChannels, numDestChannels, startOffsetInDestBuffer, startSampleInFile, numSamples);

        //PYBIND11_OVERRIDE_PURE (bool, Base, readSamples, destChannels, numDestChannels, startOffsetInDestBuffer, startSampleInFile, numSamples);

        return false;
    }
};

// =================================================================================================

template <class Base = juce::MemoryMappedAudioFormatReader>
struct PyMemoryMappedAudioFormatReader : PyAudioFormatReader<Base>
{
    using PyAudioFormatReader<Base>::PyAudioFormatReader;

    PyMemoryMappedAudioFormatReader (const juce::File& file, const juce::AudioFormatReader& details,
                                     juce::int64 dataChunkStart, juce::int64 dataChunkLength, int bytesPerFrame)
        : PyAudioFormatReader<Base> (file, details, dataChunkStart, dataChunkLength, bytesPerFrame)
    {
    }

    bool mapSectionOfFile (juce::Range<juce::int64> samplesToMap) override
    {
        PYBIND11_OVERRIDE (bool, Base, mapSectionOfFile, samplesToMap);
    }

    void getSample (juce::int64 sampleIndex, float* result) const noexcept override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<const PyAudioFormatReader<Base>*> (this), "compareElements"); override_)
        {
            auto sample = override_ (sampleIndex);

            *result = sample.cast<float>();
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"MemoryMappedAudioFormatReader::getSample\"");
    }
};

// =================================================================================================

template <class Base = juce::AudioFormatWriter>
struct PyAudioFormatWriter : Base
{
    using Base::Base;

    PyAudioFormatWriter (juce::OutputStream* destStream,
                         const juce::String& formatName,
                         double sampleRate,
                         unsigned int numberOfChannels,
                         unsigned int bitsPerSample)
        : Base (destStream, formatName, sampleRate, numberOfChannels, bitsPerSample)
    {
    }

    PyAudioFormatWriter (juce::OutputStream* destStream,
                         const juce::String& formatName,
                         double sampleRate,
                         const juce::AudioChannelSet& audioChannelLayout,
                         unsigned int bitsPerSample)
        : Base (destStream, formatName, sampleRate, audioChannelLayout, bitsPerSample)
    {
    }

    bool write (const int** samplesToWrite, int numSamples) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<const Base*> (this), "write"); override_)
        {
            pybind11::list channelSamples;

            auto samplesByChannel = samplesToWrite;
            while (*samplesByChannel != nullptr)
            {
                channelSamples.append (PyArrayView<const int> (*samplesByChannel, static_cast<size_t> (numSamples)));
                ++samplesByChannel;
            }

            return override_ (channelSamples, numSamples).cast<bool>();
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"AudioFormatWriter::write\"");
    }
};

struct PyAudioFormatWriterIncomingDataReceiver : juce::AudioFormatWriter::ThreadedWriter::IncomingDataReceiver
{
    using juce::AudioFormatWriter::ThreadedWriter::IncomingDataReceiver::IncomingDataReceiver;

    void reset (int numChannels, double sampleRate, juce::int64 totalSamplesInSource) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioFormatWriter::ThreadedWriter::IncomingDataReceiver, reset, numChannels, sampleRate, totalSamplesInSource);
    }

    void addBlock (juce::int64 sampleNumberInSource, const juce::AudioBuffer<float>& newData, int startOffsetInBuffer, int numSamples) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AudioFormatWriter::ThreadedWriter::IncomingDataReceiver, addBlock, sampleNumberInSource, newData, startOffsetInBuffer, numSamples);
    }
};

// =================================================================================================

template <class Base = juce::AudioFormat>
struct PyAudioFormat : Base
{
    using Base::Base;

    PyAudioFormat (juce::String formatName, juce::StringArray fileExtensions)
        : Base (std::move (formatName), std::move (fileExtensions))
    {
    }

    PyAudioFormat (juce::StringRef formatName, juce::StringRef fileExtensions)
        : Base (std::move (formatName), std::move (fileExtensions))
    {
    }

    juce::StringArray getFileExtensions() const override
    {
        PYBIND11_OVERRIDE (juce::StringArray, Base, getFileExtensions);
    }

    bool canHandleFile (const juce::File& fileToTest) override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, canHandleFile, fileToTest);
    }

    juce::Array<int> getPossibleSampleRates() override
    {
        PYBIND11_OVERRIDE_PURE (juce::Array<int>, Base, getPossibleSampleRates);
    }

    juce::Array<int> getPossibleBitDepths() override
    {
        PYBIND11_OVERRIDE_PURE (juce::Array<int>, Base, getPossibleBitDepths);
    }

    bool canDoStereo() override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, canDoStereo);
    }

    bool canDoMono() override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, canDoMono);
    }

    bool isCompressed() override
    {
        PYBIND11_OVERRIDE (bool, Base, isCompressed);
    }

    bool isChannelLayoutSupported (const juce::AudioChannelSet& channelSet) override
    {
        PYBIND11_OVERRIDE (bool, Base, isChannelLayoutSupported, channelSet);
    }

    juce::StringArray getQualityOptions() override
    {
        PYBIND11_OVERRIDE (juce::StringArray, Base, getQualityOptions);
    }

    juce::AudioFormatReader* createReaderFor (juce::InputStream* sourceStream, bool deleteStreamIfOpeningFails) override
    {
        PYBIND11_OVERRIDE_PURE (juce::AudioFormatReader*, Base, createReaderFor, sourceStream, deleteStreamIfOpeningFails);
    }

    juce::MemoryMappedAudioFormatReader* createMemoryMappedReader (const juce::File& file) override
    {
        PYBIND11_OVERRIDE (juce::MemoryMappedAudioFormatReader*, Base, createMemoryMappedReader, file);
    }

    juce::MemoryMappedAudioFormatReader* createMemoryMappedReader (juce::FileInputStream* fin) override
    {
        PYBIND11_OVERRIDE (juce::MemoryMappedAudioFormatReader*, Base, createMemoryMappedReader, fin);
    }

    juce::AudioFormatWriter* createWriterFor (juce::OutputStream* streamToWriteTo,
                                              double sampleRateToUse,
                                              unsigned int numberOfChannels,
                                              int bitsPerSample,
                                              const juce::StringPairArray& metadataValues,
                                              int qualityOptionIndex) override
    {
        PYBIND11_OVERRIDE_PURE (juce::AudioFormatWriter*, Base, createWriterFor, streamToWriteTo, sampleRateToUse, numberOfChannels, bitsPerSample, metadataValues, qualityOptionIndex);
    }

    juce::AudioFormatWriter* createWriterFor (juce::OutputStream* streamToWriteTo,
                                              double sampleRateToUse,
                                              const juce::AudioChannelSet& channelLayout,
                                              int bitsPerSample,
                                              const juce::StringPairArray& metadataValues,
                                              int qualityOptionIndex) override
    {
        PYBIND11_OVERRIDE_PURE (juce::AudioFormatWriter*, Base, createWriterFor, streamToWriteTo, sampleRateToUse, channelLayout, bitsPerSample, metadataValues, qualityOptionIndex);
    }
};

} // namespace popsicle::Bindings
