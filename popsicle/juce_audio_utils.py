import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_audio_processors as __juce_audio_processors
from . import juce_audio_formats as __juce_audio_formats
from . import juce_audio_devices as __juce_audio_devices

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_audio_utils/juce_audio_utils.h")

    def __pythonize(klass, name):
        if name == "AudioThumbnail":
            originalSetSource = klass.setSource
            def setSource(obj, source):
                source.__python_owns__ = False
                originalSetSource(obj, source)
            klass.setSource = setSource

        elif name == "AudioAppComponent":
            klass.shutdownAudio.__release_gil__ = True

    cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
