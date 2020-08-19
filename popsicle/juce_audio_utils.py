from functools import lru_cache as __lru_cache

import cppyy as __cppyy

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_audio_processors as __juce_audio_processors
from . import juce_audio_formats as __juce_audio_formats
from . import juce_audio_devices as __juce_audio_devices

__all__ = []

@__lru_cache(maxsize=1024)
def __juce_include():
    __cppyy.include("juce_audio_utils/juce_audio_utils.h")

__juce_bootstrap()
__juce_include()
