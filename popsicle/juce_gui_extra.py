import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_gui_basics as __juce_gui_basics

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_gui_extra/juce_gui_extra.h")

__juce_bootstrap()
__juce_include()
