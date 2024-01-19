import popsicle as juce


def test_var_constructor():
    a = juce.Value()
    assert isinstance(a.getValue(), type(None))
    assert a.getValue() is None

    a = juce.Value(True)
    assert isinstance(a.getValue(), bool)
    assert a.getValue() == True

    a = juce.Value(1)
    assert isinstance(a.getValue(), int)
    assert a.getValue() == 1

    a = juce.Value(1.0)
    assert isinstance(a.getValue(), float)
    assert a.getValue() == 1.0

    a = juce.Value("abcdefg")
    assert isinstance(a.getValue(), str)
    assert a.getValue() == "abcdefg"

    a = juce.Value(b"abcdefg")
    assert isinstance(a.getValue(), bytes)
    assert a.getValue() == b"abcdefg"

    a = juce.Value((1, 2, 3))
    assert isinstance(a.getValue(), list)
    assert a.getValue() == [1, 2, 3]

    a = juce.Value([1, "2", 3, False, None])
    assert isinstance(a.getValue(), list)
    assert a.getValue() == [1, "2", 3, False, None]

    a = juce.Value({"a": 1, "b": 2, "c": None})
    assert isinstance(a.getValue(), dict)
    assert a.getValue() == {"a": 1, "b": 2, "c": None}

    a = juce.Value({"a": {"x": 10, "y": 20, "z": [1, 2]}, "b": 2, "c": 3})
    assert isinstance(a.getValue(), dict)
    assert a.getValue() == {"a": {"x": 10, "y": 20, "z": [1, 2]}, "b": 2, "c": 3}

    a = juce.Value(juce.MemoryBlock(b'\x01\x01\x01\x01'))
    assert isinstance(a.getValue(), bytes)
    assert a.getValue() == b'\x01\x01\x01\x01'
