import sys
sys.path.insert(0, "../")

from popsicle import juce_gui_basics
from popsicle import juce, juce_multi, START_JUCE_APPLICATION


class MainContentComponent(juce_multi(juce.Component, juce.Timer)):
    def __init__(self):
        super().__init__((), ())

        self.setSize(600, 400)
        self.startTimerHz(60)

    def __del__(self):
        self.stopTimer()

    def paint(self, g):
        g.fillAll(juce.Colours.black)

        random = juce.Random.getSystemRandom()
        rect = juce.Rectangle[int](0, 0, 20, 20)

        for _ in range(100):
            g.setColour(juce.Colour(
                random.nextInt(256),
                random.nextInt(256),
                random.nextInt(256)))

            rect.setCentre(random.nextInt(self.getWidth()), random.nextInt(self.getHeight()))
            g.drawRect(rect)

    def timerCallback(self):
        if self.isVisible():
            self.repaint()


class MainWindow(juce.DocumentWindow):
    component = None

    def __init__(self):
        super().__init__(
            juce.JUCEApplication.getInstance().getApplicationName(),
            juce.Colours.black,
            juce.DocumentWindow.allButtons,
            True)

        self.component = MainContentComponent()

        self.setResizable(True, True)
        self.setContentNonOwned(self.component, True)
        self.centreWithSize(800, 600)
        self.setVisible(True)

    def __del__(self):
        if self.component:
            self.component.__del__()
            self.component = None

    def closeButtonPressed(self):
        juce.JUCEApplication.getInstance().systemRequestedQuit()


class Application(juce.JUCEApplication):
    window = None

    def getApplicationName(self):
        return "JUCE-o-matic"

    def getApplicationVersion(self):
        return "1.0"

    def initialise(self, commandLine):
        self.window = MainWindow()

    def shutdown(self):
        if self.window:
            self.window.__del__()
            self.window = None


if __name__ == "__main__":
    START_JUCE_APPLICATION(Application)
