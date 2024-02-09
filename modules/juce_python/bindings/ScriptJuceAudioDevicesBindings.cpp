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

#include "ScriptJuceAudioDevicesBindings.h"
#include "../utilities/ClassDemangling.h"

#include "ScriptJuceEventsBindings.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

void registerJuceAudioDevicesBindings (py::module_& m)
{
    // ============================================================================================ juce::WASAPIDeviceMode

    py::enum_<WASAPIDeviceMode> (m, "WASAPIDeviceMode")
        .value ("shared", WASAPIDeviceMode::shared)
        .value ("exclusive", WASAPIDeviceMode::exclusive)
        .value ("sharedLowLatency", WASAPIDeviceMode::sharedLowLatency)
    ;

    // ============================================================================================ juce::AudioIODeviceType

    py::class_<AudioIODeviceType, PyAudioIODeviceType> classAudioIODeviceType (m, "AudioIODeviceType");

    classAudioIODeviceType
        .def (py::init<const String&>())
        .def ("getTypeName", &AudioIODeviceType::getTypeName)
        .def ("scanForDevices", &AudioIODeviceType::scanForDevices)
        .def ("getDeviceNames", &AudioIODeviceType::getDeviceNames)
        .def ("getDefaultDeviceIndex", &AudioIODeviceType::getDefaultDeviceIndex)
        .def ("getIndexOfDevice", &AudioIODeviceType::getIndexOfDevice)
        .def ("hasSeparateInputsAndOutputs", &AudioIODeviceType::hasSeparateInputsAndOutputs)
        .def ("createDevice", &AudioIODeviceType::createDevice)
        .def ("addListener", &AudioIODeviceType::addListener)
        .def ("removeListener", &AudioIODeviceType::removeListener)
        .def_static ("createAudioIODeviceType_CoreAudio", &AudioIODeviceType::createAudioIODeviceType_CoreAudio)
        .def_static ("createAudioIODeviceType_iOSAudio", &AudioIODeviceType::createAudioIODeviceType_iOSAudio)
        .def_static ("createAudioIODeviceType_WASAPI", static_cast<AudioIODeviceType* (*)(WASAPIDeviceMode)> (&AudioIODeviceType::createAudioIODeviceType_WASAPI))
        .def_static ("createAudioIODeviceType_DirectSound", &AudioIODeviceType::createAudioIODeviceType_DirectSound)
        .def_static ("createAudioIODeviceType_ASIO", &AudioIODeviceType::createAudioIODeviceType_ASIO)
        .def_static ("createAudioIODeviceType_ALSA", &AudioIODeviceType::createAudioIODeviceType_ALSA)
        .def_static ("createAudioIODeviceType_JACK", &AudioIODeviceType::createAudioIODeviceType_JACK)
        .def_static ("createAudioIODeviceType_Android", &AudioIODeviceType::createAudioIODeviceType_Android)
        .def_static ("createAudioIODeviceType_OpenSLES", &AudioIODeviceType::createAudioIODeviceType_OpenSLES)
        .def_static ("createAudioIODeviceType_Oboe", &AudioIODeviceType::createAudioIODeviceType_Oboe)
        .def_static ("createAudioIODeviceType_Bela", &AudioIODeviceType::createAudioIODeviceType_Bela)
    ;

    // ============================================================================================ juce::AudioIODeviceType

    py::class_<AudioIODeviceCallbackContext> classAudioIODeviceCallbackContext (m, "AudioIODeviceCallbackContext");

    classAudioIODeviceCallbackContext
        .def (py::init<>())
        .def_readonly ("hostTimeNs", &AudioIODeviceCallbackContext::hostTimeNs)
    ;

    // ============================================================================================ juce::AudioIODeviceCallback

    py::class_<AudioIODeviceCallback, PyAudioIODeviceCallback<>> classAudioIODeviceCallback (m, "AudioIODeviceCallback");

    classAudioIODeviceCallback
        .def (py::init<>())
        //.def ("audioDeviceIOCallbackWithContext", [](AudioIODeviceCallback& self, py::buffer inputs, int numInputs, py::buffer outputs, int numOutputs, int numSamples, const AudioIODeviceCallbackContext& ctx)
        //{
        //})
        .def ("audioDeviceAboutToStart", &AudioIODeviceCallback::audioDeviceAboutToStart, "device"_a)
        .def ("audioDeviceStopped", &AudioIODeviceCallback::audioDeviceStopped)
        .def ("audioDeviceError", &AudioIODeviceCallback::audioDeviceError, "errorMessage"_a)
    ;

    // ============================================================================================ juce::AudioIODeviceCallback

    py::class_<AudioIODevice, PyAudioIODevice> classAudioIODevice (m, "AudioIODevice");

    classAudioIODevice
        .def (py::init<const String&, const String&>())
        .def ("getName", &AudioIODevice::getName)
        .def ("getTypeName", &AudioIODevice::getTypeName)
        .def ("getOutputChannelNames", &AudioIODevice::getOutputChannelNames)
        .def ("getOutputChannelNames", &AudioIODevice::getOutputChannelNames)
        .def ("getInputChannelNames", &AudioIODevice::getInputChannelNames)
        .def ("getDefaultOutputChannels", &AudioIODevice::getDefaultOutputChannels)
        .def ("getDefaultInputChannels", &AudioIODevice::getDefaultInputChannels)
        .def ("getAvailableSampleRates", &AudioIODevice::getAvailableSampleRates)
        .def ("getAvailableBufferSizes", &AudioIODevice::getAvailableBufferSizes)
        .def ("getDefaultBufferSize", &AudioIODevice::getDefaultBufferSize)
        .def ("open", &AudioIODevice::open)
        .def ("close", &AudioIODevice::close)
        .def ("isOpen", &AudioIODevice::isOpen)
        .def ("start", &AudioIODevice::start)
        .def ("stop", &AudioIODevice::stop)
        .def ("isPlaying", &AudioIODevice::isPlaying)
        .def ("getLastError", &AudioIODevice::getLastError)
        .def ("getCurrentBufferSizeSamples", &AudioIODevice::getCurrentBufferSizeSamples)
        .def ("getCurrentSampleRate", &AudioIODevice::getCurrentSampleRate)
        .def ("getCurrentBitDepth", &AudioIODevice::getCurrentBitDepth)
        .def ("getActiveOutputChannels", &AudioIODevice::getActiveOutputChannels)
        .def ("getActiveInputChannels", &AudioIODevice::getActiveInputChannels)
        .def ("getOutputLatencyInSamples", &AudioIODevice::getOutputLatencyInSamples)
        .def ("getInputLatencyInSamples", &AudioIODevice::getInputLatencyInSamples)
    //.def ("getWorkgroup", &AudioIODevice::getWorkgroup)
        .def ("hasControlPanel", &AudioIODevice::hasControlPanel)
        .def ("showControlPanel", &AudioIODevice::showControlPanel)
        .def ("getXRunCount", &AudioIODevice::getXRunCount)
    ;

    // ============================================================================================ juce::AudioDeviceManager

    py::class_<AudioDeviceManager, ChangeBroadcaster> classAudioDeviceManager (m, "AudioDeviceManager");

    py::class_<AudioDeviceManager::AudioDeviceSetup> classAudioDeviceManagerAudioDeviceSetup (classAudioDeviceManager, "AudioDeviceSetup");

    classAudioDeviceManagerAudioDeviceSetup
        .def (py::init<>())
        .def_readwrite ("outputDeviceName", &AudioDeviceManager::AudioDeviceSetup::outputDeviceName)
        .def_readwrite ("inputDeviceName", &AudioDeviceManager::AudioDeviceSetup::inputDeviceName)
        .def_readwrite ("sampleRate", &AudioDeviceManager::AudioDeviceSetup::sampleRate)
        .def_readwrite ("bufferSize", &AudioDeviceManager::AudioDeviceSetup::bufferSize)
        .def_readwrite ("inputChannels", &AudioDeviceManager::AudioDeviceSetup::inputChannels)
        .def_readwrite ("useDefaultInputChannels", &AudioDeviceManager::AudioDeviceSetup::useDefaultInputChannels)
        .def_readwrite ("outputChannels", &AudioDeviceManager::AudioDeviceSetup::outputChannels)
        .def_readwrite ("useDefaultOutputChannels", &AudioDeviceManager::AudioDeviceSetup::useDefaultOutputChannels)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    py::class_<AudioDeviceManager::LevelMeter> classAudioDeviceManagerLevelMeter (classAudioDeviceManager, "LevelMeter");

    classAudioDeviceManagerLevelMeter
        .def (py::init<>())
        .def ("getCurrentLevel", &AudioDeviceManager::LevelMeter::getCurrentLevel)
    ;

    classAudioDeviceManager
        .def (py::init<>())
        .def ("initialise", &AudioDeviceManager::initialise,
            "numInputChannelsNeeded"_a, "numOutputChannelsNeeded"_a, "savedState"_a, "selectDefaultDeviceOnFailure"_a, "preferredDefaultDeviceName"_a = String(), "preferredSetupOptions"_a = nullptr, py::call_guard<py::gil_scoped_release>())
        .def ("initialiseWithDefaultDevices", &AudioDeviceManager::initialiseWithDefaultDevices, py::call_guard<py::gil_scoped_release>())
        .def ("createStateXml", &AudioDeviceManager::createStateXml)
        .def ("getAudioDeviceSetup", py::overload_cast<> (&AudioDeviceManager::getAudioDeviceSetup, py::const_))
        .def ("getAudioDeviceSetup", py::overload_cast<AudioDeviceManager::AudioDeviceSetup&> (&AudioDeviceManager::getAudioDeviceSetup, py::const_))
        .def ("setAudioDeviceSetup", &AudioDeviceManager::setAudioDeviceSetup)
        .def ("getCurrentAudioDevice", &AudioDeviceManager::getCurrentAudioDevice, py::return_value_policy::reference)
        .def ("getCurrentAudioDeviceType", &AudioDeviceManager::getCurrentAudioDeviceType)
        .def ("getCurrentDeviceTypeObject", &AudioDeviceManager::getCurrentDeviceTypeObject, py::return_value_policy::reference)
        .def ("setCurrentAudioDeviceType", &AudioDeviceManager::setCurrentAudioDeviceType)
        .def ("getDeviceAudioWorkgroup", &AudioDeviceManager::getDeviceAudioWorkgroup)
        .def ("closeAudioDevice", &AudioDeviceManager::closeAudioDevice, py::call_guard<py::gil_scoped_release>())
        .def ("restartLastAudioDevice", &AudioDeviceManager::restartLastAudioDevice, py::call_guard<py::gil_scoped_release>())
        .def ("addAudioCallback", &AudioDeviceManager::addAudioCallback, py::call_guard<py::gil_scoped_release>())
        .def ("removeAudioCallback", &AudioDeviceManager::removeAudioCallback, py::call_guard<py::gil_scoped_release>())
        .def ("getCpuUsage", &AudioDeviceManager::getCpuUsage)
        .def ("setMidiInputDeviceEnabled", &AudioDeviceManager::setMidiInputDeviceEnabled)
        .def ("isMidiInputDeviceEnabled", &AudioDeviceManager::isMidiInputDeviceEnabled)
    //.def ("addMidiInputDeviceCallback", &AudioDeviceManager::addMidiInputDeviceCallback)
    //.def ("removeMidiInputDeviceCallback", &AudioDeviceManager::removeMidiInputDeviceCallback)
        .def ("setDefaultMidiOutputDevice", &AudioDeviceManager::setDefaultMidiOutputDevice)
        .def ("getDefaultMidiOutputIdentifier", &AudioDeviceManager::getDefaultMidiOutputIdentifier)
    //.def ("getDefaultMidiOutput", &AudioDeviceManager::getDefaultMidiOutput)
    //.def ("getAvailableDeviceTypes", &AudioDeviceManager::getAvailableDeviceTypes)
    //.def ("createAudioDeviceTypes", &AudioDeviceManager::createAudioDeviceTypes)
    //.def ("addAudioDeviceType", &AudioDeviceManager::addAudioDeviceType)
    //.def ("removeAudioDeviceType", &AudioDeviceManager::removeAudioDeviceType)
        .def ("playTestSound", &AudioDeviceManager::playTestSound, py::call_guard<py::gil_scoped_release>())
        .def ("getInputLevelGetter", &AudioDeviceManager::getInputLevelGetter)
        .def ("getOutputLevelGetter", &AudioDeviceManager::getOutputLevelGetter)
        .def ("getAudioCallbackLock", &AudioDeviceManager::getAudioCallbackLock, py::return_value_policy::reference_internal)
        .def ("getMidiCallbackLock", &AudioDeviceManager::getMidiCallbackLock, py::return_value_policy::reference_internal)
        .def ("getXRunCount", &AudioDeviceManager::getXRunCount)
    ;

    // ============================================================================================ juce::AudioSourcePlayer

    py::class_<AudioSourcePlayer, AudioIODeviceCallback, PyAudioIODeviceCallback<AudioSourcePlayer>> classAudioSourcePlayer (m, "AudioSourcePlayer");

    classAudioSourcePlayer
        .def (py::init<>())
        .def ("setSource", &AudioSourcePlayer::setSource, py::call_guard<py::gil_scoped_release>())
        .def ("setSource", [](AudioSourcePlayer& self, py::none) { self.setSource (nullptr); }, py::call_guard<py::gil_scoped_release>())
        .def ("getCurrentSource", &AudioSourcePlayer::getCurrentSource, py::return_value_policy::reference)
        .def ("setGain", &AudioSourcePlayer::setGain)
        .def ("getGain", &AudioSourcePlayer::getGain)
    ;

    // ============================================================================================ juce::AudioTransportSource

    py::class_<AudioTransportSource, PositionableAudioSource, ChangeBroadcaster, PyPositionableAudioSource<AudioTransportSource>> classAudioTransportSource (m, "AudioTransportSource");

    classAudioTransportSource
        .def (py::init<>())
        .def ("setSource", &AudioTransportSource::setSource,
            "newSource"_a, "readAheadBufferSize"_a = 0, "readAheadThread"_a = static_cast<TimeSliceThread*>(nullptr), "sourceSampleRateToCorrectFor"_a = 0.0, "maxNumChannels"_a = 2, py::call_guard<py::gil_scoped_release>())
        .def ("setSource", [](AudioTransportSource& self, py::none, int readAheadSize, TimeSliceThread* readAheadThread, double sourceSampleRateToCorrectFor, int maxNumChannels)
        {
            self.setSource (nullptr, readAheadSize, readAheadThread, sourceSampleRateToCorrectFor, maxNumChannels);
        }, "newSource"_a, "readAheadBufferSize"_a = 0, "readAheadThread"_a = static_cast<TimeSliceThread*>(nullptr), "sourceSampleRateToCorrectFor"_a = 0.0, "maxNumChannels"_a = 2, py::call_guard<py::gil_scoped_release>())
        .def ("setSource", [](AudioTransportSource& self, PositionableAudioSource* newSource, int readAheadSize, py::none, double sourceSampleRateToCorrectFor, int maxNumChannels)
        {
            self.setSource (newSource, readAheadSize, nullptr, sourceSampleRateToCorrectFor, maxNumChannels);
        }, "newSource"_a, "readAheadBufferSize"_a = 0, "readAheadThread"_a = static_cast<TimeSliceThread*>(nullptr), "sourceSampleRateToCorrectFor"_a = 0.0, "maxNumChannels"_a = 2, py::call_guard<py::gil_scoped_release>())
        .def ("setSource", [](AudioTransportSource& self, py::none, int readAheadSize, py::none, double sourceSampleRateToCorrectFor, int maxNumChannels)
        {
            self.setSource (nullptr, readAheadSize, nullptr, sourceSampleRateToCorrectFor, maxNumChannels);
        }, "newSource"_a, "readAheadBufferSize"_a = 0, "readAheadThread"_a = static_cast<TimeSliceThread*>(nullptr), "sourceSampleRateToCorrectFor"_a = 0.0, "maxNumChannels"_a = 2, py::call_guard<py::gil_scoped_release>())
        .def ("setPosition", &AudioTransportSource::setPosition)
        .def ("getCurrentPosition", &AudioTransportSource::getCurrentPosition)
        .def ("getLengthInSeconds", &AudioTransportSource::getLengthInSeconds)
        .def ("hasStreamFinished", &AudioTransportSource::hasStreamFinished)
        .def ("start", &AudioTransportSource::start, py::call_guard<py::gil_scoped_release>())
        .def ("stop", &AudioTransportSource::stop, py::call_guard<py::gil_scoped_release>())
        .def ("isPlaying", &AudioTransportSource::isPlaying)
        .def ("setGain", &AudioTransportSource::setGain)
        .def ("getGain", &AudioTransportSource::getGain)
    ;

    // ============================================================================================ juce::SystemAudioVolume

    py::class_<SystemAudioVolume> classSystemAudioVolume (m, "SystemAudioVolume");

    classSystemAudioVolume
        .def_static ("getGain", &SystemAudioVolume::getGain)
        .def_static ("setGain", &SystemAudioVolume::setGain)
        .def_static ("isMuted", &SystemAudioVolume::isMuted)
        .def_static ("setMuted", &SystemAudioVolume::setMuted)
    ;
}

} // namespace popsicle::Bindings
