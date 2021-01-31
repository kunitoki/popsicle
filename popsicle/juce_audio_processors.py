import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_gui_extra as __juce_gui_extra
from . import juce_audio_basics as __juce_audio_basics

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_audio_processors/juce_audio_processors.h")

__juce_bootstrap()
__juce_include()
