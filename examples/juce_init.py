import os
import sys
import glob
from pathlib import Path

try:
	import popsicle as juce

except ImportError:
    folder = (Path(__file__).parent.parent / "build")
    for ext in ["*.so", "*.pyd"]:
        path_to_search = folder / "**" / ext
        for f in glob.iglob(str(path_to_search), recursive=True):
            if os.path.isfile(f):
                sys.path.append(str(Path(f).parent))
                break

    import popsicle as juce


def START_JUCE_COMPONENT(ComponentClass, name):
    class DefaultWindow(juce.DocumentWindow):
        component = None

        def __init__(self):
            super().__init__(
                juce.JUCEApplication.getInstance().getApplicationName(),
                juce.Desktop.getInstance().getDefaultLookAndFeel()
                    .findColour(juce.ResizableWindow.backgroundColourId),
                juce.DocumentWindow.allButtons,
                True)

            self.component = ComponentClass()

            self.setResizable(True, True)
            self.setContentNonOwned(self.component, True)
            self.centreWithSize(self.component.getWidth(), self.component.getHeight() + self.getTitleBarHeight())
            self.setVisible(True)

        def __del__(self):
            self.clearContentComponent()

            if self.component:
                del self.component

        def closeButtonPressed(self):
            juce.JUCEApplication.getInstance().systemRequestedQuit()

    class DefaultApplication(juce.JUCEApplication):
        window = None

        def __init__(self):
            super().__init__()

        def getApplicationName(self):
            return name

        def getApplicationVersion(self):
            return "1.0"

        def initialise(self, commandLineParameters):
            self.window = DefaultWindow()

            juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

        def shutdown(self):
            if self.window:
                del self.window

        def systemRequestedQuit(self):
            self.quit()

    juce.START_JUCE_APPLICATION(DefaultApplication)
