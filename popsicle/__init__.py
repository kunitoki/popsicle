import sys
import gc
import cppyy

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_events as __juce_events


__all__ = [ "juce", "juce_multi", "juce_set_sample_data", "START_JUCE_APPLICATION", "START_JUCE_COMPONENT" ]


# Bootstrap code to initialise library and defines
__juce_bootstrap()


# Main juce defines to initialise the cpp part of juce
cppyy.cppdef("""
namespace popsicle {

// Bootstrap fixes
juce::JUCEApplicationBase* create_juce_instance() { return nullptr; }

void initialise()
{
    juce::JUCEApplicationBase::createInstance = create_juce_instance;
}

// Utilities
void set_sample_data(float* output, int sample, float value)
{
    output[sample] = value;
}

} // namespace popsicle
""")


# Popsicle initialisation
from cppyy.gbl import popsicle as __popsicle
__popsicle.initialise()


# Juce import, without this no symbols will be imported by python
from cppyy.gbl import juce


# Exported juce utilities
juce_multi = cppyy.multi

def juce_set_sample_data(output, sample, value):
    __popsicle.set_sample_data(output, sample, value)


# Main juce entry point
def START_JUCE_APPLICATION(application_class):
    juce.initialiseJuce_GUI()

    if sys.platform in ["win32", "cygwin"]:
        juce.Process.setCurrentModuleInstanceHandle()
    elif sys.platform in ["darwin"]:
        juce.initialiseNSApplication()

    application = None

    try:
        application = application_class()

        mm = juce.MessageManager.getInstance()
        mm.setCurrentThreadAsMessageThread()

        if application.initialiseApp():
            mm.runDispatchLoop()

    except Exception as e:
        print(e)

    finally:
        result = 255
        if application:
            result = application.shutdownApp()

            if hasattr(application, "__del__"):
                application.__del__()
            application = None

        gc.collect()

        juce.shutdownJuce_GUI()

        sys.exit(result)


# Additional juce entry point for simplified examples
def START_JUCE_COMPONENT(component_class, name: str = "Component Example", version: str = "1.0"):
    class MainWindow(juce.DocumentWindow):
        component = None

        def __init__(self):
            super().__init__(
                juce.JUCEApplication.getInstance().getApplicationName(),
                juce.Desktop.getInstance().getDefaultLookAndFeel()
                    .findColour(juce.ResizableWindow.backgroundColourId),
                juce.DocumentWindow.allButtons,
                True)

            self.component = component_class()

            self.setResizable(True, True)
            self.setContentNonOwned(self.component, True)
            self.setVisible(True)

        def __del__(self):
            self.clearContentComponent()

            if self.component and hasattr(self.component, "__del__"):
                self.component.__del__()
                self.component = None

        def closeButtonPressed(self):
            juce.JUCEApplication.getInstance().systemRequestedQuit()

    class Application(juce.JUCEApplication):
        window = None

        def getApplicationName(self):
            return name

        def getApplicationVersion(self):
            return version

        def initialise(self, commandLine):
            self.window = MainWindow()

        def shutdown(self):
            if self.window and hasattr(self.window, "__del__"):
                self.window.__del__()
                self.window = None

    START_JUCE_APPLICATION(Application)
