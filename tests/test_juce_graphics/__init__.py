import pytest

from .. import common

import popsicle as juce

if not hasattr(juce, "Graphics"):
    pytest.skip(allow_module_level=True)
