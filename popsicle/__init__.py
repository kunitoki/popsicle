import sys as __sys
import gc as __gc

import cppyy

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_events as __juce_events


__all__ = [ "juce", "juce_multi", "juce_delete", "juce_set_sample_data", "START_JUCE_APPLICATION", "START_JUCE_COMPONENT" ]


__juce_bootstrap()


cppyy.cppdef("""
namespace popsicle {

// Bootstrap fixes
juce::JUCEApplicationBase* create_juce_instance() { return nullptr; }

void initialise()
{
    juce::JUCEApplicationBase::createInstance = create_juce_instance;
}

void set_sample_data(float* output, int sample, float value)
{
    output[sample] = value;
}

} // namespace popsicle
""")


from cppyy.gbl import popsicle as __popsicle
__popsicle.initialise()


from cppyy.gbl import juce

juce_multi = cppyy.multi


def juce_delete(object):
    if object and hasattr(object, "aboutToBeDeleted"):
        object.aboutToBeDeleted()
        del object


def juce_set_sample_data(output, sample, value):
    __popsicle.set_sample_data(output, sample, value)


def START_JUCE_APPLICATION(application_class):
    juce.initialiseJuce_GUI()

    if __sys.platform in ["win32", "cygwin"]:
        juce.Process.setCurrentModuleInstanceHandle()
    elif __sys.platform in ["darwin"]:
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
            del application

        __gc.collect()

        juce.shutdownJuce_GUI()

        __sys.exit(result)


def START_JUCE_COMPONENT(component_class, name="Component Example", version="1.0", width=800, height=600):
    class MainWindow(juce.DocumentWindow):
        component = None

        def __init__(self):
            super().__init__(
                juce.JUCEApplication.getInstance().getApplicationName(),
                juce.Colours.black,
                juce.DocumentWindow.allButtons,
                True)

            self.component = component_class()

            self.setResizable(True, True)
            self.setContentNonOwned(self.component, True)
            self.centreWithSize(width, height)
            self.setVisible(True)

        def __del__(self):
            self.clearContentComponent()

            juce_delete(self.component)

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
            if self.window:
                del self.window

    START_JUCE_APPLICATION(Application)
