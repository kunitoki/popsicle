import pytest

from . import common
import popsicle as juce


def yield_test():
    yield


@pytest.fixture
def juce_app():
    class Application(juce.JUCEApplication):
        def __init__(self):
            super().__init__()

        def getApplicationName(self):
            return "TestApp"

        def getApplicationVersion(self):
            return "1.0"

        def initialise(self, commandLineParameters: str):
            juce.MessageManager.callAsync(yield_test)

        def shutdown(self):
            pass

    with juce.TestApplication(Application) as app:
        app.processEvents()
        yield app
