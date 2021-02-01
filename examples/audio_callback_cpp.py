import sys
sys.path.insert(0, "../")

import cppyy

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, START_JUCE_COMPONENT


cppyy.cppdef("""
class AudioCallback : public juce::AudioIODeviceCallback
{
    void audioDeviceIOCallback(
        const float** inputChannelData,
        int numInputChannels,
        float** outputChannelData,
        int numOutputChannels,
        int numSamples) override
    {
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            float* data = outputChannelData[channel];

            for (int sample = 0; sample < numSamples; ++sample)
                *data++ = rand() / (float)RAND_MAX;
        }
    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
    }

    void audioDeviceStopped() override
    {
    }
};
""")


class MainContentComponent2(juce.Component):
    deviceManager = juce.AudioDeviceManager()
    audioCallback = cppyy.gbl.AudioCallback()

    def __init__(self):
        super().__init__()

        result = self.deviceManager.initialiseWithDefaultDevices(0, 2)
        if result.isNotEmpty():
            print(result.toRawUTF8())

        self.deviceManager.addAudioCallback(self.audioCallback)

        self.setSize(800, 600)

    def paint(self, g):
        g.fillAll(juce.Colours.slategrey)

    def __del__(self):
        self.deviceManager.closeAudioDevice()


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent2, name="C++ Audio Callback")
