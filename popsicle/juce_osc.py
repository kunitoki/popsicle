import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_events as __juce_events

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_osc/juce_osc.h")

__juce_bootstrap()
__juce_include()
