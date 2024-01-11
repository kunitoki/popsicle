import sys
sys.path.insert(0, "../")

import math
import cppyy
import cppyy.ll

from popsicle import juce_gui_basics, juce_audio_utils
from popsicle import juce, juce_multi, START_JUCE_COMPONENT


class SineWaveSound(juce.SynthesiserSound):
    def __init__(self):
        super().__init__()

    def appliesToNote(self, channel):
        return True

    def appliesToChannel(self, channel):
        return True


class SineWaveVoice(juce.SynthesiserVoice):
    def __init__(self):
        super().__init__()

        self.currentAngle = 0.0
        self.angleDelta = 0.0
        self.level = 0.0
        self.tailOff = 0.0

    def canPlaySound(self, sound):
        return cppyy.ll.dynamic_cast[SineWaveSound](sound) != cppyy.nullptr

    def startNote(self, midiNoteNumber, velocity, sound, currentPitchWheelPosition):
        self.currentAngle = 0.0
        self.level = velocity * 0.15
        self.tailOff = 0.0

        cyclesPerSecond = juce.MidiMessage.getMidiNoteInHertz(midiNoteNumber)
        cyclesPerSample = cyclesPerSecond / self.getSampleRate()

        self.angleDelta = cyclesPerSample * 2.0 * math.pi

    def stopNote(self, velocity, allowTailOff):
        if allowTailOff:
            if self.tailOff == 0.0:
                self.tailOff = 1.0
        else:
            self.clearCurrentNote()
            self.angleDelta = 0.0

    def pitchWheelMoved(self, value):
        pass

    def controllerMoved(self, controller, value):
        pass

    def renderNextBlock(self, outputBuffer, startSample, numSamples):
        if self.angleDelta == 0.0:
            return

        if self.tailOff > 0.0:
            for _ in range(numSamples):
                currentSample = float(math.sin(self.currentAngle) * self.level * self.tailOff)

                for i in reversed(range(outputBuffer.getNumChannels())):
                    outputBuffer.addSample(i, startSample, currentSample)

                self.currentAngle += self.angleDelta
                startSample += 1

                self.tailOff *= 0.99
                if self.tailOff <= 0.005:
                    self.clearCurrentNote()
                    self.angleDelta = 0.0
                    break

        else:
            for _ in range(numSamples):
                currentSample = float(math.sin(self.currentAngle) * self.level)

                for i in reversed(range(outputBuffer.getNumChannels())):
                    outputBuffer.addSample(i, startSample, currentSample)

                self.currentAngle += self.angleDelta
                startSample += 1


class SynthAudioSource(juce.AudioSource):
    synth = juce.Synthesiser()
    midiCollector = juce.MidiMessageCollector()
    voices = []

    def __init__(self, keyState):
        super().__init__()

        self.keyboardState = keyState

        for _ in range(8):
            voice = SineWaveVoice()
            self.voices.append(voice)

            self.synth.addVoice(voice)

        self.synth.addSound(SineWaveSound())

    def setUsingSineWaveSound(self):
        self.synth.clearSounds()

    def prepareToPlay(self, samplesPerBlockExpected, sampleRate):
        self.synth.setCurrentPlaybackSampleRate(sampleRate)
        self.midiCollector.reset(sampleRate)

    def releaseResources(self):
        pass

    def getNextAudioBlock(self, bufferToFill):
        bufferToFill.clearActiveBufferRegion()

        incomingMidi = juce.MidiBuffer()
        self.midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples)

        self.keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, True)

        #self.synth.renderNextBlock(bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples)

    def getMidiCollector(self):
        return self.midiCollector


class MainContentComponent(juce_multi(juce.AudioAppComponent, juce.Timer)):
    keyboardState = juce.MidiKeyboardState()
    midiInputList = juce.ComboBox()
    midiInputListLabel = juce.Label()
    lastInputIndex = 0

    def __init__(self):
        super().__init__((), ())

        self.synthAudioSource = SynthAudioSource(self.keyboardState)
        self.keyboardComponent = juce.MidiKeyboardComponent(self.keyboardState, juce.MidiKeyboardComponent.horizontalKeyboard)

        self.addAndMakeVisible(self.midiInputListLabel)
        self.midiInputListLabel.setText("MIDI Input:", juce.dontSendNotification)
        self.midiInputListLabel.attachToComponent(self.midiInputList, True)

        midiInputs = juce.MidiInput.getAvailableDevices()
        self.addAndMakeVisible(self.midiInputList)
        self.midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled")

        midiInputNames = juce.StringArray()
        for midiInput in midiInputs:
            midiInputNames.add(midiInput.name)

        self.midiInputList.addItemList(midiInputNames, 1)

        def onChange():
            self.setMidiInput(self.midiInputList.getSelectedItemIndex())
        self.midiInputList.onChange = onChange

        for midiInput in midiInputs:
            if self.deviceManager.isMidiInputDeviceEnabled(midiInput.identifier):
                self.setMidiInput(midiInputs.indexOf(midiInput))
                break

        if self.midiInputList.getSelectedId() == 0:
            self.setMidiInput(0)

        self.addAndMakeVisible(self.keyboardComponent)
        self.setAudioChannels(0, 2)

        self.setSize(600, 190)
        self.startTimer(400)

    def __del__(self):
        self.keyboardComponent = None

        self.stopTimer()
        self.shutdownAudio()

    def resized(self):
        self.midiInputList.setBounds(200, 10, self.getWidth() - 210, 20)
        self.keyboardComponent.setBounds(10, 40, self.getWidth() - 20, self.getHeight() - 50)

    def prepareToPlay(self, samplesPerBlockExpected, sampleRate):
        self.synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate)

    def getNextAudioBlock(self, bufferToFill):
        self.synthAudioSource.getNextAudioBlock(bufferToFill)

    def releaseResources(self):
        self.synthAudioSource.releaseResources()

    def timerCallback(self):
        self.keyboardComponent.grabKeyboardFocus()
        self.stopTimer()

    def setMidiInput(self, index):
        availableDevices = juce.MidiInput.getAvailableDevices()

        self.deviceManager.removeMidiInputDeviceCallback(availableDevices[self.lastInputIndex].identifier,
                                                         self.synthAudioSource.getMidiCollector())

        newInput = availableDevices[index]

        if not self.deviceManager.isMidiInputDeviceEnabled(newInput.identifier):
            self.deviceManager.setMidiInputDeviceEnabled(newInput.identifier, True)

        self.deviceManager.addMidiInputDeviceCallback(newInput.identifier, self.synthAudioSource.getMidiCollector())
        self.midiInputList.setSelectedId(index + 1, juce.dontSendNotification)

        self.lastInputIndex = index


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Midi Synth")
