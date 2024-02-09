import sys
import pytest

import popsicle as juce

from .utilities import get_runtime_data_folder, remove_directory_recursively

#==================================================================================================

def pytest_addoption(parser):
    parser.addoption("--update-rendering", action="store_true", default=False)

def pytest_generate_tests(metafunc):
    option_value = metafunc.config.option.update_rendering
    if "update_rendering" in metafunc.fixturenames and option_value:
        metafunc.parametrize("update_rendering", [option_value])

def pytest_unconfigure(config):
    if sys.gettrace() is not None:
        return

    remove_directory_recursively(get_runtime_data_folder().getFullPathName(), [".gitignore"])

#==================================================================================================

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
        next(app)
        yield app
        next(app)
