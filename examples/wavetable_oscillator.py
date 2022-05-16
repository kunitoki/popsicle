import sys
sys.path.insert(0, "../")

import math

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, START_JUCE_COMPONENT


class WavetableOscillator(object):
    wavetable = None
    tableSize = 0
    currentIndex = 0.0
    tableDelta = 0.0

    def __init__(self, wavetableToUse):
        self.wavetable = wavetableToUse
        self.tableSize = self.wavetable.getNumSamples() - 1

        assert self.wavetable.getNumChannels() == 1

    def setFrequency(self, frequency, sampleRate):
        tableSizeOverSampleRate = float(self.tableSize) / sampleRate
        self.tableDelta = frequency * tableSizeOverSampleRate

    def getNextSample(self):
        index0 = int(self.currentIndex)
        index1 = index0 + 1

        frac = self.currentIndex - float(index0)

        table = self.wavetable.getReadPointer(0)
        value0 = table[index0]
        value1 = table[index1]

        currentSample = value0 + frac * (value1 - value0)

        self.currentIndex += self.tableDelta
        if self.currentIndex > self.tableSize:
            self.currentIndex -= self.tableSize

        return currentSample


class MainContentComponent(juce_multi(juce.AudioAppComponent, juce.Timer)):
    cpuUsageLabel = juce.Label()
    cpuUsageText = juce.Label()

    tableSize = 1 << 7
    level = 0.0

    sineTable = juce.AudioSampleBuffer()
    oscillators = []

    def __init__(self):
        super().__init__((), ())

        self.cpuUsageLabel.setText("CPU Usage", juce.dontSendNotification)
        self.cpuUsageText.setJustificationType(juce.Justification.right)
        self.addAndMakeVisible(self.cpuUsageLabel)
        self.addAndMakeVisible(self.cpuUsageText)

        self.createWavetable()

        self.setSize(400, 200)
        self.setAudioChannels(0, 2)
        self.startTimer(50)

    def __del__(self):
        self.shutdownAudio()

    def resized(self):
        self.cpuUsageLabel.setBounds(10, 10, self.getWidth() - 20, 20)
        self.cpuUsageText.setBounds(10, 10, self.getWidth() - 20, 20)

    def timerCallback(self):
        cpu = self.deviceManager.getCpuUsage() * 100
        self.cpuUsageText.setText(juce.String(cpu, 6) + " %", juce.dontSendNotification)

    def createWavetable(self):
        self.sineTable.setSize(1, int(self.tableSize) + 1)
        self.sineTable.clear()

        samples = self.sineTable.getWritePointer(0)

        harmonics = [ 1, 3, 5, 6, 7, 9, 13, 15 ]
        harmonicWeights = [ 0.5, 0.1, 0.05, 0.125, 0.09, 0.005, 0.002, 0.001 ]

        assert len(harmonics) == len(harmonicWeights)

        for harmonic in range(len(harmonics)):
            angleDelta = juce.MathConstants[float].twoPi / float(self.tableSize - 1) * harmonics[harmonic]
            currentAngle = 0.0

            for i in range(self.tableSize):
                sample = math.sin(currentAngle)
                samples[i] += sample * harmonicWeights[harmonic]
                currentAngle += angleDelta

        samples[self.tableSize] = samples[0]

    def prepareToPlay (self, samplePerBlock, sampleRate):
        numberOfOscillators = 10

        for _ in range(numberOfOscillators):
            oscillator = WavetableOscillator(self.sineTable)

            midiNote = juce.Random.getSystemRandom().nextDouble() * 36.0 + 48.0
            frequency = 440.0 * math.pow(2.0, (midiNote - 69.0) / 12.0)

            oscillator.setFrequency(frequency, sampleRate)
            self.oscillators.append(oscillator)

        self.level = 0.25 / float(numberOfOscillators)

    def releaseResources(self):
        pass

    def getNextAudioBlock(self, bufferToFill):
        leftBuffer  = bufferToFill.buffer.getWritePointer(0, bufferToFill.startSample)
        rightBuffer = bufferToFill.buffer.getWritePointer(1, bufferToFill.startSample)

        bufferToFill.clearActiveBufferRegion()

        for oscillator in self.oscillators:
            for sample in range(bufferToFill.numSamples):
                levelSample = oscillator.getNextSample() * self.level

                leftBuffer[sample] += levelSample
                rightBuffer[sample] += levelSample


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Wavetable Oscillator")
