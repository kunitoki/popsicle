from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class DecibelSlider(juce.Slider):
    def __init__(self):
        juce.Slider.__init__(self)

    def getValueFromText(self, text):
        minusInfinitydB = -100.0

        decibelText = text.upToFirstOccurrenceOf("dB", False, False).trim()

        return minusInfinitydB if decibelText.equalsIgnoreCase("-INF") else decibelText.getDoubleValue()

    def getTextFromValue(self, value):
        return juce.Decibels.toString(value)


class MainContentComponent(juce.AudioAppComponent):
    decibelSlider = juce.Slider()
    decibelLabel = juce.Label()
    random = juce.Random()
    level = 0.0

    def __init__(self):
        juce.AudioAppComponent.__init__(self)

        self.decibelSlider.setRange(-100, -12)
        self.decibelSlider.setTextBoxStyle(juce.Slider.TextBoxRight, False, 100, 20)

        def valueChanged():
            self.level = juce.Decibels.decibelsToGain(float(self.decibelSlider.getValue()))

        self.decibelSlider.onValueChange = valueChanged
        self.decibelSlider.setValue(juce.Decibels.gainToDecibels(self.level))
        self.decibelLabel.setText("Noise Level in dB", juce.dontSendNotification)

        self.addAndMakeVisible(self.decibelSlider)
        self.addAndMakeVisible(self.decibelLabel)

        self.setSize(600, 100)
        self.setAudioChannels(0, 2)

    def visibilityChanged(self):
        if not self.isVisible():
            self.shutdownAudio()

    def prepareToPlay(self, blockSize, sampleRate):
        pass

    def getNextAudioBlock(self, bufferToFill):
        currentLevel = self.level
        levelScale = currentLevel * 2.0

        for channel in range(bufferToFill.buffer.getNumChannels()):
            buffer = bufferToFill.buffer.getWritePointer(channel, bufferToFill.startSample)

            for sample in range(bufferToFill.numSamples):
                buffer[sample] = self.random.nextFloat() * levelScale - currentLevel

    def releaseResources(self):
        pass

    def resized(self):
        self.decibelLabel .setBounds(10, 10, 120, 20);
        self.decibelSlider.setBounds(130, 10, self.getWidth() - 140, 20);


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Slider Decibels Example")
