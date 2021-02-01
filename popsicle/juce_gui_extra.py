import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_gui_basics as __juce_gui_basics

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_gui_extra/juce_gui_extra.h")

__juce_bootstrap()
__juce_include()
