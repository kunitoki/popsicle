import os
import sys
import pytest

from .. import common

import popsicle as juce

if not hasattr(juce, "Component") or (sys.platform == "linux" and not os.environ.get("DISPLAY", False)):
    pytest.skip(allow_module_level=True)
