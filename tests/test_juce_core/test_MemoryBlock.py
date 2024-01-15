from .. import common
import popsicle as juce

def test_constructor():
    mb = juce.MemoryBlock()
    mb.append(bytearray(b"123456"))
    assert mb.toString() == "123456"
