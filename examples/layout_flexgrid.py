from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class RightSidePanel(juce.Component):
    backgroundColour = juce.Colours.lightblue
    buttons = []

    def __init__(self, colour=None):
        super().__init__()

        if colour:
            self.backgroundColour = colour

        for i in range(10):
            button = juce.TextButton(str(i))

            self.buttons.append(button)
            self.addAndMakeVisible(button)

    def paint(self, g):
        g.fillAll(self.backgroundColour)

    def resized(self):
        fb = juce.FlexBox()
        fb.flexWrap = juce.FlexBox.Wrap.wrap
        fb.justifyContent = juce.FlexBox.JustifyContent.center
        fb.alignContent = juce.FlexBox.AlignContent.center

        for b in self.buttons:
            fb.items.add(juce.FlexItem(b).withMinWidth(50.0).withMinHeight(50.0))

        fb.performLayout(self.getLocalBounds().toFloat())


class LeftSidePanel(juce.Component):
    backgroundColour = juce.Colours.lightgrey
    knobs = []

    def __init__(self, colour=None):
        super().__init__()

        if colour:
            self.backgroundColour = colour

        for _ in range(10):
            slider = juce.Slider()
            slider.setSliderStyle(juce.Slider.SliderStyle.Rotary)
            slider.setTextBoxStyle(juce.Slider.NoTextBox, True, 0, 0)

            self.knobs.append(slider)
            self.addAndMakeVisible(slider)

    def paint(self, g):
        g.fillAll(self.backgroundColour)

    def resized(self):
        knobBox = juce.FlexBox()
        knobBox.flexWrap = juce.FlexBox.Wrap.wrap
        knobBox.justifyContent = juce.FlexBox.JustifyContent.spaceBetween

        for k in self.knobs:
            knobBox.items.add(juce.FlexItem(k).withMinHeight(50.0).withMinWidth(50.0).withFlex(1))

        fb = juce.FlexBox()
        fb.flexDirection = juce.FlexBox.Direction.column
        fb.items.add(juce.FlexItem(knobBox).withFlex(2.5))
        fb.performLayout(self.getLocalBounds().toFloat())


class MainPanel(juce.Component):
    sliders = []

    def __init__(self):
        super().__init__()

        for _ in range(5):
            slider = juce.Slider()
            slider.setTextBoxStyle(juce.Slider.TextEntryBoxPosition.NoTextBox, True, 0, 0)

            self.sliders.append(slider)
            self.addAndMakeVisible(slider)

    def paint(self, g):
        g.fillAll(juce.Colours.hotpink)

    def resized(self):
        isPortrait = self.getLocalBounds().getHeight() > self.getLocalBounds().getWidth()

        fb = juce.FlexBox()
        fb.flexDirection = juce.FlexBox.Direction.column if isPortrait else juce.FlexBox.Direction.row

        for s in self.sliders:
            if isPortrait:
                s.setSliderStyle(juce.Slider.SliderStyle.LinearHorizontal)
            else:
                s.setSliderStyle(juce.Slider.SliderStyle.LinearVertical)

            fb.items.add(juce.FlexItem(s).withFlex(0, 1, self.getHeight() / 5.0 if isPortrait else self.getWidth() / 5.0))

        fb.performLayout(self.getLocalBounds().toFloat())


class MainContentComponent(juce.Component):
    def __init__(self):
        super().__init__()

        self.rightPanel = RightSidePanel(juce.Colours.lightgrey)
        self.leftPanel = LeftSidePanel(juce.Colours.lightblue)
        self.mainPanel = MainPanel()

        self.addAndMakeVisible(self.rightPanel)
        self.addAndMakeVisible(self.leftPanel)
        self.addAndMakeVisible(self.mainPanel)

        self.setSize(600, 400)

    def paint(self, g):
        g.fillAll(self.getLookAndFeel().findColour(juce.ResizableWindow.backgroundColourId))

    def resized(self):
        bounds = self.getLocalBounds()
        self.leftPanel.setBounds(bounds.removeFromLeft(self.proportionOfWidth(0.25)))
        self.rightPanel.setBounds(bounds.removeFromRight(self.proportionOfWidth(0.25)))
        self.mainPanel.setBounds(bounds)

        """
        grid = juce.Grid()

        grid.templateRows.add(juce.Grid.TrackInfo(juce.Grid.Fr(1)))
        grid.templateColumns.add(juce.Grid.TrackInfo(juce.Grid.Fr(1)))
        grid.templateColumns.add(juce.Grid.TrackInfo(juce.Grid.Fr(2)))
        grid.templateColumns.add(juce.Grid.TrackInfo(juce.Grid.Fr(1)))

        grid.items.add(juce.GridItem(self.leftPanel))
        grid.items.add(juce.GridItem(self.mainPanel))
        grid.items.add(juce.GridItem(self.rightPanel))

        grid.performLayout(self.getLocalBounds())
        """


if __name__ == "__main__":
    START_JUCE_COMPONENT(MainContentComponent, name="Animated Component")
