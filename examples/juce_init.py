import os
import sys
import glob
import time
import traceback
from pathlib import Path
from functools import wraps


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


def START_JUCE_COMPONENT(ComponentClass, name, **kwargs):
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
            self.setAlwaysOnTop(kwargs.get("alwaysOnTop", False))
            self.setVisible(True)

        def __del__(self):
            self.clearContentComponent()

            if self.component:
                self.component.setVisible(False)
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

        # def unhandledException(self, ex: Exception, file: str, line: int):
        #     if hasattr(ex, "__traceback__"):
        #         print("Traceback (most recent call last):")
        #         traceback.print_tb(ex.__traceback__)
        #     print(ex)
        #     if isinstance(ex, KeyboardInterrupt):
        #         juce.JUCEApplication.getInstance().systemRequestedQuit()

    juce.START_JUCE_APPLICATION(
        DefaultApplication,
        catchExceptionsAndContinue=kwargs.get("catchExceptionsAndContinue", False))


def timeit(func):
    @wraps(func)
    def timeit_wrapper(*args, **kwargs):
        start_time = time.perf_counter()
        result = func(*args, **kwargs)
        total_time = time.perf_counter() - start_time
        print(f'Function {func.__name__} Took {total_time:.4f} seconds') # {args} {kwargs}
        return result

    return timeit_wrapper
