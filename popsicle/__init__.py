import sys
import os
import gc
import cppyy

from .utils import juce_bind
from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_events as __juce_events


__all__ = [ "juce", "juce_multi", "juce_bind", "juce_equals", "START_JUCE_APPLICATION", "START_JUCE_COMPONENT" ]


__juce_bootstrap()


cppyy.cppdef("""
namespace popsicle {

// Bootstrap fixes
juce::JUCEApplicationBase* create_juce_instance() { return nullptr; }

void initialise()
{
    juce::JUCEApplicationBase::createInstance = create_juce_instance;
}

// Homebrew extensions
template <class T, class U>
bool equals(T* t, U* u) noexcept
{
    return t == u;
}

} // namespace popsicle
""")


from cppyy.gbl import popsicle as __popsicle
__popsicle.initialise()


from cppyy.gbl import juce

juce_multi = cppyy.multi
juce_equals = __popsicle.equals


def START_JUCE_APPLICATION(application_class):
    juce.initialiseJuce_GUI()

    if sys.platform in ["win32", "cygwin"]:
        juce.Process.setCurrentModuleInstanceHandle()
    elif sys.platform in ["darwin"]:
        juce.initialiseNSApplication()

    app = application_class()

    try:
        mm = juce.MessageManager.getInstance()
        mm.setCurrentThreadAsMessageThread()

        if app.initialiseApp():
            mm.runDispatchLoop()

    except Exception as e:
        print(e)

    finally:
        result = app.shutdownApp()
        del app

        gc.collect()

        juce.shutdownJuce_GUI()

        sys.exit(result)


def START_JUCE_COMPONENT(component_class, name="Component Example", version="1.0", width=800, height=600):
    class MainWindow(juce.DocumentWindow):
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
            if hasattr(self, "component") and self.component:
                del self.component

        def closeButtonPressed(self):
            self.setVisible(False)
            juce.JUCEApplication.getInstance().systemRequestedQuit()

    class Application(juce.JUCEApplication):
        def getApplicationName(self):
            return name

        def getApplicationVersion(self):
            return version

        def initialise(self, commandLine):
            self.window = MainWindow()

        def shutdown(self):
            if hasattr(self, "window") and self.window:
                del self.window

    START_JUCE_APPLICATION(Application)
