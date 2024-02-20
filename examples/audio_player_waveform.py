from enum import Enum

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class TransportState(Enum):
    Stopped = 0
    Starting = 1
    Playing = 2
    Stopping = 3


class AudioSource(juce.AudioSource):
    transportSource = juce.AudioTransportSource()
    hasReader = juce.Atomic[bool](False)

    def __init__(self):
        juce.AudioSource.__init__(self)

    def prepareToPlay(self, samplesPerBlockExpected, sampleRate):
        self.transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate)

    def getNextAudioBlock(self, bufferToFill):
        if not self.hasReader.get():
            bufferToFill.clearActiveBufferRegion()
        else:
            self.transportSource.getNextAudioBlock(bufferToFill)

    def releaseResources(self):
        self.transportSource.releaseResources()


class SimpleThumbnailComponent(juce.Component, juce.ChangeListener):
    def __init__(self, sourceSamplesPerThumbnailSample, formatManager, cache):
        juce.Component.__init__(self)
        juce.ChangeListener.__init__(self)

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


class SimplePositionOverlay(juce.Component, juce.Timer):
    def __init__(self, transportSourceToUse):
        juce.Component.__init__(self)
        juce.Timer.__init__(self)

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


class MainContentComponent(juce.Component, juce.ChangeListener, juce.Timer):
    openButton = juce.TextButton()
    playButton = juce.TextButton()
    stopButton = juce.TextButton()
    loopingToggle = juce.ToggleButton()
    currentPositionLabel = juce.Label()

    audioSource = AudioSource()
    audioSourcePlayer = juce.AudioSourcePlayer()
    readerSource = None
    isLooping = False

    deviceManager = juce.AudioDeviceManager()
    formatManager = juce.AudioFormatManager()
    state = TransportState.Stopped

    def __init__(self):
        juce.Component.__init__(self)
        juce.ChangeListener.__init__(self)
        juce.Timer.__init__(self)

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

        self.addAndMakeVisible(self.loopingToggle)
        self.loopingToggle.setButtonText("Loop")
        self.loopingToggle.onClick = self.loopButtonChanged

        self.addAndMakeVisible(self.currentPositionLabel)
        self.currentPositionLabel.setText("Stopped", juce.dontSendNotification)

        self.formatManager.registerBasicFormats()
        self.audioSource.transportSource.addChangeListener(self)

        self.deviceManager.initialise(0, 2, None, True)
        self.deviceManager.addAudioCallback(self.audioSourcePlayer)
        self.audioSourcePlayer.setSource(self.audioSource)

        self.thumbnailCache = juce.AudioThumbnailCache(10)

        self.thumbnailComp = SimpleThumbnailComponent(512, self.formatManager, self.thumbnailCache)
        self.addAndMakeVisible(self.thumbnailComp)

        self.positionOverlay = SimplePositionOverlay(self.audioSource.transportSource)
        self.addAndMakeVisible(self.positionOverlay)

        self.setSize(400, 400)
        self.startTimer(20)

    def visibilityChanged(self):
        if self.isVisible() or not self.deviceManager:
            return

        self.audioSource.hasReader.set(False)
        self.deviceManager.removeAudioCallback(self.audioSourcePlayer)

        self.audioSource.transportSource.setSource(None)
        self.audioSourcePlayer.setSource(None)

        self.deviceManager.closeAudioDevice()

    def resized(self):
        self.openButton.setBounds(10, 10, self.getWidth() - 20, 20)
        self.playButton.setBounds(10, 40, self.getWidth() - 20, 20)
        self.stopButton.setBounds(10, 70, self.getWidth() - 20, 20)
        self.loopingToggle.setBounds(10, 100, self.getWidth() - 20, 20)
        self.currentPositionLabel.setBounds(10, 130, self.getWidth() - 20, 20)

        thumbnailBounds = juce.Rectangle[int](10, 100, self.getWidth() - 20, self.getHeight() - 120)
        self.thumbnailComp.setBounds(thumbnailBounds)
        self.positionOverlay.setBounds(thumbnailBounds)

    def changeListenerCallback(self, source):
        print("changeListenerCallback", source, self.audioSource.transportSource)
        if source == self.audioSource.transportSource:
            if self.audioSource.transportSource.isPlaying():
                self.changeState(TransportState.Playing)
            else:
                self.changeState(TransportState.Stopped)

    def timerCallback(self):
        if self.audioSource.transportSource.isPlaying():
            position = juce.RelativeTime(self.audioSource.transportSource.getCurrentPosition())

            minutes = int(position.inMinutes()) % 60
            seconds = int(position.inSeconds()) % 60
            millis  = int(position.inMilliseconds()) % 1000

            positionString = "{:02d}:{:02d}:{:03d}".format(minutes, seconds, millis)

            self.currentPositionLabel.setText(positionString, juce.dontSendNotification)
        else:
            self.currentPositionLabel.setText("Stopped", juce.dontSendNotification)

    def updateLoopState(self, shouldLoop):
        self.isLooping = shouldLoop

        if self.readerSource:
            self.readerSource.setLooping(self.isLooping)

    def changeState (self, newState):
        if self.state == newState:
            return

        self.state = newState
        print(self.state)

        if self.state == TransportState.Stopped:
            self.stopButton.setEnabled(False)
            self.playButton.setEnabled(True)
            self.audioSource.transportSource.setPosition(0.0)

        elif self.state == TransportState.Starting:
            self.playButton.setEnabled(False)
            self.audioSource.transportSource.start()

        elif self.state == TransportState.Playing:
            self.stopButton.setEnabled(True)

        elif self.state == TransportState.Stopping:
            self.audioSource.transportSource.stop()

    def openButtonClicked(self):
        chooser = juce.FileChooser("Select a Wave file to play...", juce.File(), "*.wav")

        if chooser.browseForFileToOpen():
            file = chooser.getResult()
            self.reader = self.formatManager.createReaderFor(file)

            if self.reader:
                self.readerSource = juce.AudioFormatReaderSource(self.reader, False)
                if self.isLooping:
                    self.readerSource.setLooping(self.isLooping)

                self.audioSource.transportSource.setSource(self.readerSource, 0, None, self.reader.sampleRate, 2)
                self.audioSource.hasReader.set(True)
                self.thumbnailComp.setFile(file)

                self.playButton.setEnabled(True)
            else:
                self.audioSource.hasReader.set(False)

    def playButtonClicked(self):
        self.updateLoopState(self.loopingToggle.getToggleState())
        self.changeState(TransportState.Starting)

    def stopButtonClicked(self):
        self.changeState(TransportState.Stopping)

    def loopButtonChanged(self):
        self.updateLoopState(self.loopingToggle.getToggleState())


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Audio Player")
