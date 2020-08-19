import sys
sys.path.insert(0, "../")

import math

from popsicle import juce_gui_basics
from popsicle import juce, START_JUCE_COMPONENT


class MainContentComponent(juce.Component):
    header = juce.TextButton()
    sidebar = juce.TextButton()

    limeContent = juce.TextButton()
    grapefruitContent = juce.TextButton()
    lemonContent = juce.TextButton()
    orangeContent = juce.TextButton()
    footer = juce.TextButton()

    def __init__(self):
        super().__init__()

        self.header.setColour(juce.TextButton.buttonColourId, juce.Colours.cornflowerblue)
        self.header.setButtonText("Header")
        self.addAndMakeVisible(self.header)

        self.footer.setColour(juce.TextButton.buttonColourId, juce.Colours.cornflowerblue)
        self.footer.setButtonText("Footer")
        self.addAndMakeVisible(self.footer)

        self.sidebar.setColour(juce.TextButton.buttonColourId, juce.Colours.grey)
        self.sidebar.setButtonText("Sidebar")
        self.addAndMakeVisible(self.sidebar)

        self.limeContent.setColour(juce.TextButton.buttonColourId, juce.Colours.lime)
        self.addAndMakeVisible(self.limeContent)

        self.grapefruitContent.setColour (juce.TextButton.buttonColourId, juce.Colours.yellowgreen)
        self.addAndMakeVisible(self.grapefruitContent)

        self.lemonContent.setColour (juce.TextButton.buttonColourId, juce.Colours.yellow)
        self.addAndMakeVisible(self.lemonContent)

        self.orangeContent.setColour(juce.TextButton.buttonColourId, juce.Colours.orange)
        self.addAndMakeVisible(self.orangeContent)

        self.setSize(400, 400)

    def paint(self, g):
        g.fillAll(juce.Colours.darkgrey)

    def resized(self):
        area = self.getLocalBounds()

        headerFooterHeight = 36
        self.header.setBounds(area.removeFromTop    (headerFooterHeight))
        self.footer.setBounds(area.removeFromBottom (headerFooterHeight))

        sidebarWidth = 80
        self.sidebar.setBounds(area.removeFromLeft  (sidebarWidth))

        contentItemHeight = 24
        self.limeContent.setBounds       (area.removeFromTop (contentItemHeight))
        self.grapefruitContent.setBounds (area.removeFromTop (contentItemHeight))
        self.lemonContent.setBounds      (area.removeFromTop (contentItemHeight))
        self.orangeContent.setBounds     (area.removeFromTop (contentItemHeight))


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Advanced GUI layout techniques", width=400, height=400)
