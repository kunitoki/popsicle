import math
import numpy as np

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class WavetableOscillator(object):
    wavetable = None
    tableSize = 0
    currentIndex = 0.0
    tableDelta = 0.0

    def __init__(self, wavetableToUse, samplePerBlock):
        assert wavetableToUse.getNumChannels() == 1

        self.wavetable = wavetableToUse
        self.tableSize = self.wavetable.getNumSamples() - 1

    def setFrequency(self, frequency, sampleRate):
        tableSizeOverSampleRate = float(self.tableSize) / sampleRate
        self.tableDelta = frequency * tableSizeOverSampleRate

    def getNextBlock(self, numSamples):
        indexI = np.linspace(self.currentIndex, self.currentIndex + self.tableDelta * (numSamples - 1), numSamples)
        indexI = np.remainder(indexI, float(self.tableSize))
        index0 = np.floor(indexI).astype(int)
        index1 = index0 + 1
        frac = indexI - index0.astype(float)

        table = np.array(self.wavetable.getReadPointer(0), copy=False)
        value0 = table[index0]
        value1 = table[index1]

        block = value0 + frac * (value1 - value0)

        self.currentIndex = (self.currentIndex + self.tableDelta * numSamples) % self.tableSize

        return block


class MainContentComponent(juce.AudioAppComponent, juce.Timer):
    cpuUsageLabel = juce.Label()
    cpuUsageText = juce.Label()

    tableSize = 1 << 7
    level = 0.0

    sineTable = juce.AudioSampleBuffer()
    oscillators = []

    def __init__(self):
        juce.AudioAppComponent.__init__(self)
        juce.Timer.__init__(self)

        self.cpuUsageLabel.setText("CPU Usage", juce.dontSendNotification)
        self.cpuUsageText.setJustificationType(juce.Justification.right)
        self.addAndMakeVisible(self.cpuUsageLabel)
        self.addAndMakeVisible(self.cpuUsageText)

        self.createWavetable()

        self.setSize(400, 200)
        self.setAudioChannels(0, 2)
        self.startTimer(50)

    def visibilityChanged(self):
        if not self.isVisible():
            self.shutdownAudio()

    def resized(self):
        self.cpuUsageLabel.setBounds(10, 10, self.getWidth() - 20, 20)
        self.cpuUsageText.setBounds(10, 10, self.getWidth() - 20, 20)

    def timerCallback(self):
        cpu = self.deviceManager.getCpuUsage() * 100
        self.cpuUsageText.setText(f"{cpu:.1f} %", juce.dontSendNotification)

    def createWavetable(self):
        self.sineTable.setSize(1, int(self.tableSize) + 1)
        self.sineTable.clear()

        harmonics = np.array([1, 3, 5, 6, 7, 9, 13, 15])
        harmonicWeights = np.array([0.5, 0.1, 0.05, 0.125, 0.09, 0.005, 0.002, 0.001])
        assert len(harmonics) == len(harmonicWeights)

        def computeHarmonic(index):
            angleDelta = 2 * np.pi / float(self.tableSize - 1) * harmonics[index]
            return np.sin(np.linspace(0.0, angleDelta * float(self.tableSize - 1), int(self.tableSize))) * harmonicWeights[index]

        harm = computeHarmonic(0)
        for harmonic in range(1, len(harmonics)):
            harm += computeHarmonic(harmonic)

        samples = np.array(self.sineTable.getWritePointer(0), copy=False)
        samples[0:len(harm)] = harm
        samples[self.tableSize] = samples[0]

    def prepareToPlay (self, samplePerBlock, sampleRate):
        numberOfOscillators = 10

        for i in range(numberOfOscillators):
            oscillator = WavetableOscillator(self.sineTable, samplePerBlock)

            #midiNote = float(i * 4) + 48.0
            midiNote = juce.Random.getSystemRandom().nextDouble() * 36.0 + 48.0
            frequency = 440.0 * math.pow(2.0, (midiNote - 69.0) / 12.0)

            oscillator.setFrequency(frequency, sampleRate)
            self.oscillators.append(oscillator)

        self.level = 0.25 / float(numberOfOscillators)

    def releaseResources(self):
        pass

    def getNextAudioBlock(self, bufferToFill):
        bufferToFill.clearActiveBufferRegion()

        leftBuffer  = np.array(bufferToFill.buffer.getWritePointer(0, bufferToFill.startSample), copy=False)
        rightBuffer = np.array(bufferToFill.buffer.getWritePointer(1, bufferToFill.startSample), copy=False)

        for oscillator in self.oscillators:
            block = oscillator.getNextBlock(bufferToFill.numSamples)

            leftBuffer += block * self.level
            rightBuffer += block * self.level


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Wavetable Oscillator")
