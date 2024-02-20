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

void registerJuceAudioUtilsBindings (py::module_& m)
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

    // ============================================================================================ juce::AudioThumbnailBase

    py::class_<AudioThumbnailBase, ChangeBroadcaster, PyAudioThumbnailBase<>> classAudioThumbnailBase (m, "AudioThumbnailBase");

    classAudioThumbnailBase
        .def (py::init<>())
        .def ("clear", &AudioThumbnailBase::clear)
        .def ("setSource", &AudioThumbnailBase::setSource)
        .def ("setReader", &AudioThumbnailBase::setReader)
        .def ("loadFrom", &AudioThumbnailBase::loadFrom)
        .def ("saveTo", &AudioThumbnailBase::saveTo)
        .def ("getNumChannels", &AudioThumbnailBase::getNumChannels)
        .def ("getTotalLength", &AudioThumbnailBase::getTotalLength)
        .def ("drawChannel", &AudioThumbnailBase::drawChannel)
        .def ("drawChannels", &AudioThumbnailBase::drawChannels)
        .def ("isFullyLoaded", &AudioThumbnailBase::isFullyLoaded)
        .def ("getNumSamplesFinished", &AudioThumbnailBase::getNumSamplesFinished)
        .def ("getApproximatePeak", &AudioThumbnailBase::getApproximatePeak)
    //.def ("getApproximateMinMax", &AudioThumbnailBase::getApproximateMinMax)
        .def ("getHashCode", &AudioThumbnailBase::getHashCode)
        .def ("reset", &AudioThumbnailBase::reset, "numChannels"_a, "sampleRate"_a, "totalSamplesInSource"_a = 0)
        .def ("addBlock", &AudioThumbnailBase::addBlock, "sampleNumberInSource"_a, "newData"_a, "startOffsetInBuffer"_a, "numSamples"_a)
    ;

    // ============================================================================================ juce::AudioThumbnailCache

    py::class_<AudioThumbnailCache> classAudioThumbnailCache (m, "AudioThumbnailCache");

    classAudioThumbnailCache
        .def (py::init<int>(), "maxNumThumbsToStore"_a)
        .def ("clear", &AudioThumbnailCache::clear)
        .def ("loadThumb", &AudioThumbnailCache::loadThumb)
        .def ("storeThumb", &AudioThumbnailCache::storeThumb)
        .def ("removeThumb", &AudioThumbnailCache::removeThumb)
        .def ("readFromStream", &AudioThumbnailCache::readFromStream)
        .def ("writeToStream", &AudioThumbnailCache::writeToStream)
        .def ("getTimeSliceThread", &AudioThumbnailCache::getTimeSliceThread)
    ;

    // ============================================================================================ juce::AudioThumbnail

    py::class_<AudioThumbnail, AudioThumbnailBase, PyAudioThumbnailBase<AudioThumbnail>> classAudioThumbnail (m, "AudioThumbnail");

    classAudioThumbnail
        .def (py::init<int, AudioFormatManager&, AudioThumbnailCache&>(), "sourceSamplesPerThumbnailSample"_a, "formatManagerToUse"_a, "cacheToUse"_a)
        .def ("clear", &AudioThumbnail::clear)
        .def ("setSource", [](AudioThumbnail& self, py::object newSource) { self.setSource (newSource.release().cast<InputSource*>()); }, "newSource"_a)
        .def ("setReader", [](AudioThumbnail& self, py::object newReader, int64 hashCode) { self.setReader (newReader.release().cast<AudioFormatReader*>(), hashCode); }, "newReader"_a, "hashCode"_a)
        .def ("setSource", py::overload_cast<const AudioBuffer<float>*, double, int64> (&AudioThumbnail::setSource), "newSource"_a, "sampleRate"_a, "hashCode"_a)
        .def ("setSource", py::overload_cast<const AudioBuffer<int>*, double, int64> (&AudioThumbnail::setSource), "newSource"_a, "sampleRate"_a, "hashCode"_a)
    ;
}

} // namespace popsicle::Bindings
