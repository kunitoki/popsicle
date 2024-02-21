import os
import importlib

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class HotReloadContentComponent(juce.Component, juce.Timer):
    comp = None
    module = None

    moduleName = "hotreload_component"
    fileToWatch = juce.File(os.path.abspath(__file__)).getSiblingFile(f"{moduleName}.py")
    fileLastModificationTime = None

    def __init__(self):
        juce.Component.__init__(self)
        juce.Timer.__init__(self)

        width = 600
        height = 400

        self.setSize(int(width), int(height))
        self.setOpaque(True)
        self.startTimerHz(5)

    def timerCallback(self):
        fileLastModificationTime = self.fileToWatch.getLastModificationTime()

        if not self.fileLastModificationTime or self.fileLastModificationTime < fileLastModificationTime:
            self.instantiateComponent()
            self.resized()

        self.fileLastModificationTime = fileLastModificationTime

    def paint(self, g: juce.Graphics):
        g.fillAll(juce.Colours.black)

    def resized(self):
        bounds = self.getLocalBounds()

        if self.comp:
            self.comp.setBounds(bounds)

    def instantiateComponent(self):
        try:
            if not self.module:
                self.module = importlib.import_module(self.moduleName)
            else:
                self.module = importlib.reload(self.module)

            print("Module", self.fileToWatch.getFileName(), "reloaded correctly")

        except Exception as e:
            print(e)
            return

        if self.comp:
            self.removeChildComponent(self.comp)

        self.comp = self.module.TestComponent()

        self.addAndMakeVisible(self.comp)


if __name__ == "__main__":
    START_JUCE_COMPONENT(HotReloadContentComponent, name="Hot Reload Example", alwaysOnTop=True, catchExceptionsAndContinue=True)
