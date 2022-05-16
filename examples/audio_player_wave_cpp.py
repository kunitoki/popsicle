import sys
sys.path.insert(0, "../")

import cppyy
from enum import Enum

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, START_JUCE_COMPONENT

cppyy.cppdef("""

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
        if (!hasReader.get())
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
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::Atomic<bool> hasReader{ false };
};
""")


class TransportState(Enum):
    Stopped = 0
    Starting = 1
    Playing = 2
    Stopping = 3


class SimpleThumbnailComponent(juce_multi(juce.Component, juce.ChangeListener)):
    def __init__(self, sourceSamplesPerThumbnailSample, formatManager, cache):
        super().__init__((), ())

        self.thumbnail = juce.AudioThumbnail(sourceSamplesPerThumbnailSample, formatManager, cache)
        self.thumbnail.addChangeListener(self)

    def setFile(self, file):
        source = juce.FileInputSource(file)
        self.thumbnail.setSource(source)

    def paint(self, g):
        if self.thumbnail.getNumChannels() == 0:
            self.paintIfNoFileLoaded(g)
        else:
            self.paintIfFileLoaded(g)

    def paintIfNoFileLoaded(self, g):
        g.fillAll(juce.Colours.white)
        g.setColour(juce.Colours.darkgrey)
        g.drawFittedText("No File Loaded", self.getLocalBounds(), juce.Justification.centred, 1)

    def paintIfFileLoaded(self, g):
        g.fillAll(juce.Colours.white)

        g.setColour(juce.Colours.red)
        self.thumbnail.drawChannels(g, self.getLocalBounds(), 0.0, self.thumbnail.getTotalLength(), 1.0)

    def changeListenerCallback(self, source):
        if source == self.thumbnail:
            self.thumbnailChanged()

    def thumbnailChanged(self):
        self.repaint()


class SimplePositionOverlay(juce_multi(juce.Component, juce.Timer)):
    def __init__(self, transportSourceToUse):
        super().__init__((), ())

        self.transportSource = transportSourceToUse
        self.startTimer(40)

    def __del__(self):
        self.stopTimer()

    def paint(self, g):
        duration = float(self.transportSource.getLengthInSeconds())

        if duration > 0.0:
            audioPosition = float(self.transportSource.getCurrentPosition())
            drawPosition = (audioPosition / duration) * self.getWidth()

            g.setColour(juce.Colours.green)
            g.drawLine(drawPosition, 0.0, drawPosition, float(self.getHeight()), 2.0)

    def mouseDown(self, event):
        duration = self.transportSource.getLengthInSeconds()

        if duration > 0.0:
            clickPosition = event.position.x
            audioPosition = (clickPosition / self.getWidth()) * duration

            self.transportSource.setPosition(audioPosition)

    def timerCallback(self):
        self.repaint()


class MainContentComponent(juce_multi(cppyy.gbl.AudioAppComponent, juce.ChangeListener)):
    openButton = juce.TextButton()
    playButton = juce.TextButton()
    stopButton = juce.TextButton()

    deviceManager = juce.AudioDeviceManager()
    formatManager = juce.AudioFormatManager()

    state = TransportState.Stopped

    def __init__(self):
        super().__init__((), ())

        self.addAndMakeVisible(self.openButton)
        self.openButton.setButtonText("Open...")
        self.openButton.onClick = self.openButtonClicked

        self.addAndMakeVisible (self.playButton)
        self.playButton.setButtonText ("Play")
        self.playButton.onClick = self.playButtonClicked
        self.playButton.setColour(juce.TextButton.buttonColourId, juce.Colours.green)
        self.playButton.setEnabled(False)

        self.addAndMakeVisible(self.stopButton)
        self.stopButton.setButtonText("Stop")
        self.stopButton.onClick = self.stopButtonClicked
        self.stopButton.setColour(juce.TextButton.buttonColourId, juce.Colours.red)
        self.stopButton.setEnabled(False)

        self.formatManager.registerBasicFormats()
        self.transportSource.addChangeListener(self)

        self.deviceManager.initialise(0, 2, cppyy.nullptr, True)
        self.deviceManager.addAudioCallback(self.audioSourcePlayer)
        self.audioSourcePlayer.setSource(self)

        self.thumbnailComp = SimpleThumbnailComponent(512, self.formatManager, self.thumbnailCache)
        self.addAndMakeVisible(self.thumbnailComp)

        self.positionOverlay = SimplePositionOverlay(self.transportSource)
        self.addAndMakeVisible(self.positionOverlay)

        self.setSize(600, 400)

    def __del__(self):
        if not self.deviceManager:
            return

        self.hasReader.set(False)
        self.deviceManager.removeAudioCallback(self.audioSourcePlayer)

        self.transportSource.setSource(cppyy.nullptr)
        self.audioSourcePlayer.setSource(cppyy.nullptr)

        self.deviceManager.closeAudioDevice()
        self.deviceManager = None

    def resized(self):
        self.openButton.setBounds(10, 10, self.getWidth() - 20, 20)
        self.playButton.setBounds(10, 40, self.getWidth() - 20, 20)
        self.stopButton.setBounds(10, 70, self.getWidth() - 20, 20)

        thumbnailBounds = juce.Rectangle[int](10, 100, self.getWidth() - 20, self.getHeight() - 120)
        self.thumbnailComp.setBounds(thumbnailBounds)
        self.positionOverlay.setBounds(thumbnailBounds)

    def changeListenerCallback(self, source):
        if source == self.transportSource:
            if self.transportSource.isPlaying():
                self.changeState(TransportState.Playing)
            else:
                self.changeState(TransportState.Stopped)

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
            file = chooser.getResult()
            reader = self.formatManager.createReaderFor(file)

            if reader:
                self.readerSource = juce.AudioFormatReaderSource(reader, True)
                self.transportSource.setSource(self.readerSource, 0, cppyy.nullptr, reader.sampleRate, 2)
                self.thumbnailComp.setFile(file)
                self.hasReader.set(True)

                self.playButton.setEnabled(True)
            else:
                self.hasReader.set(False)

    def playButtonClicked(self):
        self.changeState(TransportState.Starting)

    def stopButtonClicked(self):
        self.changeState(TransportState.Stopping)


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="C++ Audio Player Waveform")
