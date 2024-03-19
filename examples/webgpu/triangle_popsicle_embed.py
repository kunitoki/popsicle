"""
An example demonstrating a popsicle app with a wgpu viz inside.
"""

# run_example = false

import popsicle as juce
from pop import WgpuWidget

from triangle import main


class Example(juce.Component):
    def __init__(self):
        super().__init__()

        self.canvas1 = WgpuWidget()
        self.addAndMakeVisible(self.canvas1)

        self.canvas2 = WgpuWidget()
        self.addAndMakeVisible(self.canvas2)

        self.slider = juce.Slider()
        self.addAndMakeVisible(self.slider)

        self.setSize(640, 480)
        self.setVisible(True)

    def resized(self):
        bounds = self.getLocalBounds()

        sliderBounds = bounds.removeFromBottom(40).reduced(5)
        self.slider.setBounds(sliderBounds)

        bounds1 = bounds.removeFromTop(int(bounds.getHeight() / 2)).reduced(5)
        bounds2 = bounds.reduced(5)

        self.canvas1.setBounds(bounds1)
        self.canvas2.setBounds(bounds2)

    def getCanvas1(self):
        return self.canvas1

    def getCanvas2(self):
        return self.canvas2


class WgpuWindow(juce.DocumentWindow):
    component = None

    def __init__(self):
        super().__init__(
            "wgpu triangle embedded in a popsicle app",
            juce.Desktop.getInstance().getDefaultLookAndFeel()
                .findColour(juce.ResizableWindow.backgroundColourId),
            juce.DocumentWindow.allButtons,
            True)

        self.component = Example()

        self.setUsingNativeTitleBar(True)
        self.setResizable(True, True)
        self.setContentNonOwned(self.component, True)
        self.centreWithSize(self.component.getWidth(), self.component.getHeight())
        self.setVisible(True)

    def __del__(self):
        self.clearContentComponent()
        if self.component:
            del self.component

    def closeButtonPressed(self):
        juce.JUCEApplication.getInstance().systemRequestedQuit()

    def getCanvas1(self):
        return self.component.getCanvas1()

    def getCanvas2(self):
        return self.component.getCanvas2()


class WgpuApplication(juce.JUCEApplication):
    window = None

    def getApplicationName(self):
        return "triangle-embed-juce"

    def getApplicationVersion(self):
        return "1.0"

    def initialise(self, commandLineParameters: str):
        self.window = WgpuWindow()

        self.window.getCanvas1().reparentToWindow(self.window)
        main(self.window.getCanvas1())

        self.window.getCanvas2().reparentToWindow(self.window)
        main(self.window.getCanvas2())

        self.window.getContentComponent().resized()

        juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

    def shutdown(self):
        if self.window:
            del self.window

    def systemRequestedQuit(self):
        self.quit()


juce.START_JUCE_APPLICATION(WgpuApplication)
