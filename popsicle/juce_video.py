import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_gui_extra as __juce_gui_extra

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_video/juce_video.h")

__juce_bootstrap()
__juce_include()
