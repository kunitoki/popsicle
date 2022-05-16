import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_core as __juce_core

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_audio_basics/juce_audio_basics.h")

__juce_bootstrap()
__juce_include()
