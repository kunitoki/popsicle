import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_core/juce_core.h")

    def __pythonize(klass, name):
        if name == "String":
            klass.__str__ = klass.toRawUTF8

        if name == "XmlElement":
            klass.getNextElement.__creates__ = False
            klass.getFirstChildElement.__creates__ = False
            klass.getNextElementWithTagName.__creates__ = False
            klass.getChildElement.__creates__ = False
            klass.getChildByName.__creates__ = False
            klass.getChildByAttribute.__creates__ = False

    __cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
