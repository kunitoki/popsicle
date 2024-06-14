#include "PopsiclePluginProcessor.h"
#include "PopsiclePluginEditor.h"

// =================================================================================================

JUCE_PYTHON_DEFINE_EMBEDDED_MODULES

// =================================================================================================

PYBIND11_EMBEDDED_MODULE(custom, m)
{
    namespace py = pybind11;

    //py::module_::import (popsicle::PythonModuleName);

    //py::class_<AudioPluginAudioProcessor, juce::Component> (m, "PopsicleDemo")
    //    .def_readwrite ("text", &PopsicleDemo::text);
}

// =================================================================================================

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , Thread ("Popsicle Python Thread")
{
    startThread();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    signalThreadShouldExit();

    audioReady.arrive_and_wait();
    backgroundReady.arrive_and_wait();

    waitForThreadToExit(10000);
}

// =================================================================================================

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// =================================================================================================

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

// =================================================================================================

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

// =================================================================================================

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

// =================================================================================================

void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate);

    audioBuffer.setSize (2, samplesPerBlock);
    backgroundBuffer.setSize (2, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    audioBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    audioBuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());

    if (isThreadRunning() && ! threadShouldExit())
    {
        audioReady.arrive_and_wait();

        std::swap(audioBuffer, backgroundBuffer);

        backgroundReady.arrive_and_wait();
    }

    buffer.clear();
}

// =================================================================================================

void AudioPluginAudioProcessor::run()
{
    auto tempPath = juce::File::getSpecialLocation (juce::File::tempDirectory);
    tempPath.deleteRecursively();

    auto engine = popsicle::ScriptEngine (popsicle::ScriptEngine::prepareScriptingHome (
        juce::JUCEApplication::getInstance()->getApplicationName(),
        tempPath,
        [](const char* resourceName) -> juce::MemoryBlock
        {
            int dataSize = 0;
            auto data = BinaryData::getNamedResource (resourceName, dataSize);
            return { data, static_cast<size_t> (dataSize) };
        }));

    pybind11::dict locals;
    locals["custom"] = pybind11::module_::import ("custom");
    locals["juce"] = pybind11::module_::import (popsicle::PythonModuleName);
    //locals["this"] = pybind11::cast (this);

    pybind11::dict globals = pybind11::globals();

/*
import threading

def example_thread(should_stop):
    import sys, time
    while not should_stop.is_set():
        sys.stdout.write("x")
        sys.stdout.flush()
        time.sleep(1)

should_stop = threading.Event()
t = threading.Thread(target=example_thread, args=(should_stop,))
t.daemon = True
t.start()
*/
    {
        auto result = engine.runScript (R"(
import numpy as np

def initialise():
    import sys
    print("Running Popsicle as Python" + ".".join(sys.version.split(".")[:2]))

def run(audioBuffer):
    return audioBuffer

def finalise():
    print("\nFinished Running Popsicle")

        )", locals, globals);

        if (result.failed())
            std::cout << result.getErrorMessage();
    }

    globals["initialise"]();

    while (! threadShouldExit())
    {
        audioReady.arrive_and_wait();
        backgroundReady.arrive_and_wait();

        auto maxValue = globals["run"](backgroundBuffer);
        std::cout << pybind11::str(maxValue) << std::endl;
    }

    globals["finalise"]();
}

// =================================================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
