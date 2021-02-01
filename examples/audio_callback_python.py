#import sys
#sys.path.insert(0, "../")

import math
import cppyy

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, START_JUCE_COMPONENT


class AudioCallback(juce.AudioIODeviceCallback):
    def audioDeviceIOCallback(self, inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples):
        #try:
        for channel in range(numOutputChannels):
            channelData = outputChannelData[channel]
            for sample in range(numSamples):
                channelData[sample] = 0.0

        #except Exception as e:
        #    print(e)
        #    exit(1)

    def audioDeviceAboutToStart(self, device):
        print("started")

    def audioDeviceStopped(self):
        print("stopped")

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

    def paint(self, g):
        g.fillAll(juce.Colours.slategrey)

    def __del__(self):
        self.deviceManager.closeAudioDevice()


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent2, name="Python Audio Callback")
