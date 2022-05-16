import cppyy
from functools import lru_cache

from .utils import juce_bootstrap as __juce_bootstrap
from . import juce_audio_basics as __juce_audio_basics
from . import juce_events as __juce_events

__all__ = []

@lru_cache(maxsize=1024)
def __juce_include():
    cppyy.include("juce_audio_devices/juce_audio_devices.h")

    def __pythonize(klass, name):
        if name == "AudioSourcePlayer":
            klass.audioDeviceIOCallback.__release_gil__ = True

        elif name == "AudioDeviceManager":
            klass.removeAudioCallback.__release_gil__ = True

    cppyy.py.add_pythonization(__pythonize, "juce")

__juce_bootstrap()
__juce_include()
