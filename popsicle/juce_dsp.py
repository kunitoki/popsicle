import cppyy as __cppyy

from functools import lru_cache as __lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_audio_formats as __juce_audio_formats

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_dsp/juce_dsp.h")

__juce_bootstrap()
__juce_include()
