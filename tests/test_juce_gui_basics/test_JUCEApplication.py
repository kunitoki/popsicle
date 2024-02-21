import popsicle as juce


#==================================================================================================

class CustomApplication(juce.JUCEApplication):
    wasInitialised = False
    wasShutdown = False
    commandLineParameters = None

    def __init__(self):
        juce.JUCEApplication.__init__(self)

    def getApplicationName(self):
        return "xyz"

    def getApplicationVersion(self):
        return "42.0"

    def initialise(self, commandLineParameters):
        self.wasInitialised = True
        self.commandLineParameters = commandLineParameters

    def shutdown(self):
        self.wasShutdown = True

#==================================================================================================

def test_applications_invariants():
    assert juce.JUCEApplication.isStandaloneApp()

#==================================================================================================

def test_application():
    a = CustomApplication()
    assert not a.wasInitialised
    assert not a.wasShutdown
    assert a.getApplicationName() == "xyz"
    assert a.getApplicationVersion() == "42.0"
    assert a.getApplicationReturnValue() == 0

    a.initialise("abc")
    assert a.wasInitialised
    assert a.commandLineParameters == "abc"

    a.setApplicationReturnValue(1)
    assert a.getApplicationReturnValue() == 1

    a.shutdown()
    assert a.wasShutdown
