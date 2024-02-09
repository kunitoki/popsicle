import math
import random

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class AudioCallback(juce.AudioIODeviceCallback):
    gain = 1.0
    time = 0.0
    device = None

    def audioDeviceAboutToStart(self, device: juce.AudioIODevice):
        print("starting", device, "at", device.getCurrentSampleRate())
        self.device = device

    def audioDeviceIOCallbackWithContext(self, inputs, numInputChannels, outputs, numOutputChannels, numSamples, context):
        for sample in range(numSamples):
            noise = (random.random() * 2.0 - 1.0) * 0.025
            sound = math.sin(math.radians(self.time))
            self.time = (self.time + 2.0) % 360.0

            for output in outputs:
                output[sample] = (noise + sound) * self.gain

    def audioDeviceError(self, errorMessage: str):
        print("error", errorMessage)

    def audioDeviceStopped(self):
        print("stopping")


class MainContentComponent(juce.Component):
    manager = juce.AudioDeviceManager()
    audio_callback = AudioCallback()

    def __init__(self):
        juce.Component.__init__(self)

        width = 600
        height = 400

        self.manager.addAudioCallback(self.audio_callback)
        result = self.manager.initialiseWithDefaultDevices(0, 2)
        if result:
            print(result)

        self.button = juce.TextButton("Silence!")
        self.addAndMakeVisible(self.button)
        self.button.onStateChange = lambda: self.onButtonStateChange()

        self.setSize(int(width), int(height))
        self.setOpaque(True)

    def visibilityChanged(self):
        if not self.isVisible() and self.manager:
            self.manager.removeAudioCallback(self.audio_callback)
            self.manager.closeAudioDevice()

    def onButtonStateChange(self):
        if self.button.getState() == juce.Button.ButtonState.buttonDown:
            self.audio_callback.gain = 0.25
        else:
            self.audio_callback.gain = 1.0

    def paint(self, g: juce.Graphics):
        g.fillAll(juce.Colours.black)

    def resized(self):
        bounds = self.getLocalBounds()
        self.button.setBounds(bounds.reduced(100))


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Audio Device Example")
