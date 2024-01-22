import pytest
from .. import common

import popsicle as juce

if not hasattr(juce, "MessageManager"):
    pytest.skip(allow_module_level=True)
