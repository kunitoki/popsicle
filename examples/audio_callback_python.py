from random import random
import sys
sys.path.insert(0, "../")

import math
import cppyy
import random

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, juce_set_sample_data, START_JUCE_COMPONENT


class AudioCallback(juce.AudioIODeviceCallback):
    def audioDeviceIOCallback(self, inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples):
        for channel in range(numOutputChannels):
            channelData = outputChannelData[channel]
            for sample in range(numSamples):
                juce_set_sample_data(channelData, sample, random.random() * 2.0 - 1.0)

    def audioDeviceAboutToStart(self, device):
        pass

    def audioDeviceStopped(self):
        pass

    def audioDeviceError(self, errorMessage):
        print(errorMessage.toRawUTF8())


class MainContentComponent2(juce.Component):
    deviceManager = juce.AudioDeviceManager()
    audioCallback = AudioCallback()

    def __init__(self):
        super().__init__()

        result = self.deviceManager.initialiseWithDefaultDevices(0, 2)
        if result.isNotEmpty():
            print(result.toRawUTF8())

        self.deviceManager.addAudioCallback(self.audioCallback)

        self.setSize(800, 600)

    def __del__(self):
        if not self.deviceManager:
            return

        self.deviceManager.removeAudioCallback(self.audioCallback)

        self.deviceManager.closeAudioDevice()
        self.deviceManager = None

    def paint(self, g):
        g.fillAll(juce.Colours.slategrey)


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent2, name="Python Audio Callback")
