import os

import popsicle as juce

#==================================================================================================

def test_constructor():
    a = juce.File(__file__)
    assert a.getFullPathName() == __file__
    assert a.getFileName() == os.path.split(__file__)[1]
