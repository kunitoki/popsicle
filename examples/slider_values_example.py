import sys
sys.path.insert(0, "../")

from popsicle import juce_gui_basics
from popsicle import juce, START_JUCE_COMPONENT


class MainContentComponent(juce.Component):
    frequencySlider = juce.Slider()
    frequencyLabel = juce.Label()
    durationSlider = juce.Slider()
    durationLabel = juce.Label()

    def __init__(self):
        super().__init__()

        self.addAndMakeVisible(self.frequencySlider)
        self.frequencySlider.setRange (50, 5000.0)
        self.frequencySlider.setTextValueSuffix (" Hz")
        self.frequencySlider.onValueChange = lambda: self.durationSlider.setValue(1.0 / self.frequencySlider.getValue(), juce.dontSendNotification)

        self.addAndMakeVisible(self.frequencyLabel)
        self.frequencyLabel.setText("Frequency", juce.dontSendNotification)
        self.frequencyLabel.attachToComponent(self.frequencySlider, True)

        self.addAndMakeVisible(self.durationSlider)
        self.durationSlider.setRange (1.0 / self.frequencySlider.getMaximum(), 1.0 / self.frequencySlider.getMinimum())
        self.durationSlider.setTextValueSuffix(" s")
        self.durationSlider.onValueChange = lambda: self.frequencySlider.setValue (1.0 / self.durationSlider.getValue(), juce.dontSendNotification)

        self.addAndMakeVisible(self.durationLabel)
        self.durationLabel.setText("Duration", juce.dontSendNotification)
        self.durationLabel.attachToComponent(self.durationSlider, True)

        self.frequencySlider.setValue(500.0)

        self.frequencySlider.setTextBoxStyle(juce.Slider.TextBoxLeft, False, 160, self.frequencySlider.getTextBoxHeight())
        self.durationSlider.setTextBoxStyle(juce.Slider.TextBoxLeft, False, 160, self.durationSlider.getTextBoxHeight())

        self.frequencySlider.setSkewFactorFromMidPoint(500)
        self.durationSlider.setSkewFactorFromMidPoint(0.002)

        self.setSize(600, 90)

    def resized(self):
        sliderLeft = 120
        self.frequencySlider.setBounds(sliderLeft, 20, self.getWidth() - sliderLeft - 10, 20)
        self.durationSlider.setBounds(sliderLeft, 50, self.getWidth() - sliderLeft - 10, 20)


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Slider Values Example", width=600, height=90)
