from functools import lru_cache as __lru_cache

import cppyy as __cppyy

from .utils import juce_bootstrap as __juce_bootstrap

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_core/juce_core.h")

    def __pythonize(klass, name):
        if name == "String":
            try:
                klass.__str__ = klass.toRawUTF8
            except KeyError:
                pass

    __cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
