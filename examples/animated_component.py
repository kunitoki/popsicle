import math

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class MainContentComponent(juce.AnimatedAppComponent):
    def __init__(self):
        super().__init__()

        self.setSize(800, 600)
        self.setFramesPerSecond(60)

    def update(self):
        pass

    def paint(self, g):
        g.fillAll(self.getLookAndFeel().findColour(juce.ResizableWindow.backgroundColourId))

        g.setColour(juce.Colours.turquoise) # self.getLookAndFeel().findColour(juce.Slider.thumbColourId)

        fishLength = 15
        spinePath = juce.Path()

        for i in range(fishLength):
            radius = 100 + 10 * math.sin(self.getFrameCounter() * 0.1 + i * 0.5)

            p = juce.Point[float](
                self.getWidth() / 2.0 + 1.5 * radius * math.sin(self.getFrameCounter() * 0.02 + i * 0.12),
                self.getHeight() / 2.0 + 1.0 * radius * math.cos(self.getFrameCounter() * 0.04 + i * 0.12))

            g.fillEllipse(p.x - i, p.y - i, 2.0 + 2.0 * i, 2.0 + 2.0 * i)

            if i == 0:
                spinePath.startNewSubPath(p)
            else:
                spinePath.lineTo(p)

        g.strokePath(spinePath, juce.PathStrokeType(4.0))


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Animated Component")
