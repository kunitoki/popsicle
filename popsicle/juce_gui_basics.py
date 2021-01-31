import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_graphics as __juce_graphics
from . import juce_data_structures as __juce_data_structures

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_gui_basics/juce_gui_basics.h")

__juce_bootstrap()
__juce_include()
