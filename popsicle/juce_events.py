import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_core as __juce_core

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_events/juce_events.h")

    def __pythonize(klass, name):
        if name == "MessageManager":
            klass.runDispatchLoop.__release_gil__ = True

    cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
