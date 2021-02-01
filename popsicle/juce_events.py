import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_core as __juce_core

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_events/juce_events.h")

    def __pythonize(klass, name):
        if name == "MessageManager":
            try:
                klass.runDispatchLoop.__release_gil__ = True
            except KeyError:
                pass

    __cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
