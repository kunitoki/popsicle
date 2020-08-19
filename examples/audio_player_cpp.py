import sys
sys.path.insert(0, "../")

import math
import cppyy
from cppyy.gbl import std
from enum import Enum

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, juce_equals, juce_bind, START_JUCE_COMPONENT

cppyy.cppdef("""
#include <atomic>

class AudioAppComponent : public juce::Component, public juce::AudioSource
{
public:
    AudioAppComponent() = default;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (!hasReader)
            bufferToFill.clearActiveBufferRegion();
        else
            transportSource.getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override
    {
        transportSource.releaseResources();
    }

    juce::AudioTransportSource transportSource;
    juce::AudioSourcePlayer audioSourcePlayer;
    std::atomic<int> hasReader{ false };
};
""")


class TransportState(Enum):
    Stopped = 0
    Starting = 1
    Playing = 2
    Stopping = 3


class MainContentComponent(juce_multi(cppyy.gbl.AudioAppComponent, juce.ChangeListener, juce.Timer)):
    openButton = juce.TextButton()
    playButton = juce.TextButton()
    stopButton = juce.TextButton()
    loopingToggle = juce.ToggleButton()
    currentPositionLabel = juce.Label()

    deviceManager = juce.AudioDeviceManager()
    formatManager = juce.AudioFormatManager()
    state = TransportState.Stopped

    def __init__(self):
        super().__init__((), (), ())

        self.addAndMakeVisible(self.openButton)
        self.openButton.setButtonText("Open...")
        self.openButton.onClick = juce_bind(self, self.openButtonClicked)

        self.addAndMakeVisible (self.playButton)
        self.playButton.setButtonText ("Play")
        self.playButton.onClick = juce_bind(self, self.playButtonClicked)
        self.playButton.setColour(juce.TextButton.buttonColourId, juce.Colours.green)
        self.playButton.setEnabled(False)

        self.addAndMakeVisible(self.stopButton)
        self.stopButton.setButtonText("Stop")
        self.stopButton.onClick = juce_bind(self, self.stopButtonClicked)
        self.stopButton.setColour(juce.TextButton.buttonColourId, juce.Colours.red)
        self.stopButton.setEnabled(False)

        self.addAndMakeVisible(self.loopingToggle)
        self.loopingToggle.setButtonText("Loop")
        self.loopingToggle.onClick = juce_bind(self, self.loopButtonChanged)

        self.addAndMakeVisible(self.currentPositionLabel)
        self.currentPositionLabel.setText("Stopped", juce.dontSendNotification)

        self.formatManager.registerBasicFormats()
        self.transportSource.addChangeListener(self)

        self.deviceManager.initialise(0, 2, cppyy.nullptr, True)
        self.deviceManager.addAudioCallback(self.audioSourcePlayer)
        self.audioSourcePlayer.setSource(self)

        self.setSize(400, 400)
        self.startTimer(20)

    def __del__(self):
        self.hasReader.store(False)
        self.deviceManager.removeAudioCallback(self.audioSourcePlayer)

        self.transportSource.setSource(cppyy.nullptr)
        self.audioSourcePlayer.setSource(cppyy.nullptr)

        self.deviceManager.closeAudioDevice()

    def resized(self):
        self.openButton          .setBounds (10, 10,  self.getWidth() - 20, 20)
        self.playButton          .setBounds (10, 40,  self.getWidth() - 20, 20)
        self.stopButton          .setBounds (10, 70,  self.getWidth() - 20, 20)
        self.loopingToggle       .setBounds (10, 100, self.getWidth() - 20, 20)
        self.currentPositionLabel.setBounds (10, 130, self.getWidth() - 20, 20)

    def changeListenerCallback(self, source):
        if juce_equals(source, self.transportSource):
            if self.transportSource.isPlaying():
                self.changeState(TransportState.Playing)
            else:
                self.changeState(TransportState.Stopped)

    def timerCallback(self):
        if self.transportSource.isPlaying():
            position = juce.RelativeTime(self.transportSource.getCurrentPosition())

            minutes = int(position.inMinutes()) % 60
            seconds = int(position.inSeconds()) % 60
            millis  = int(position.inMilliseconds()) % 1000

            positionString = "{:02d}:{:02d}:{:03d}".format(minutes, seconds, millis)

            self.currentPositionLabel.setText(positionString, juce.dontSendNotification)
        else:
            self.currentPositionLabel.setText("Stopped", juce.dontSendNotification)

    def updateLoopState(self, shouldLoop):
        if self.readerSource:
            self.readerSource.setLooping(shouldLoop)

    def changeState (self, newState):
        if self.state == newState:
            return

        self.state = newState

        if self.state == TransportState.Stopped:
            self.stopButton.setEnabled(False)
            self.playButton.setEnabled(True)
            self.transportSource.setPosition(0.0)

        elif self.state == TransportState.Starting:
            self.playButton.setEnabled(False)
            self.transportSource.start()

        elif self.state == TransportState.Playing:
            self.stopButton.setEnabled(True)

        elif self.state == TransportState.Stopping:
            self.transportSource.stop()

    def openButtonClicked(self):
        chooser = juce.FileChooser("Select a Wave file to play...", juce.File(), "*.wav")

        if chooser.browseForFileToOpen():
            reader = self.formatManager.createReaderFor(chooser.getResult())

            if reader:
                self.readerSource = juce.AudioFormatReaderSource(reader, True)
                self.transportSource.setSource(self.readerSource, 0, cppyy.nullptr, reader.sampleRate, 2)
                self.hasReader.store(True)

                self.playButton.setEnabled(True)
            else:
                self.hasReader.store(False)

    def playButtonClicked(self):
        self.updateLoopState(self.loopingToggle.getToggleState())
        self.changeState(TransportState.Starting)

    def stopButtonClicked(self):
        self.changeState(TransportState.Stopping)

    def loopButtonChanged(self):
        self.updateLoopState(self.loopingToggle.getToggleState())


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="C++ Audio Player", width=400, height=400)
