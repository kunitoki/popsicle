import pytest

import popsicle as juce

#==================================================================================================

def test_ints_constructor_positive():
    a = juce.int8(1)
    assert a.get() == 1

    a = juce.int16(1)
    assert a.get() == 1

    a = juce.int32(1)
    assert a.get() == 1

    a = juce.int64(1)
    assert a.get() == 1

#==================================================================================================

def test_ints_constructor_negative():
    a = juce.int8(-1)
    assert a.get() == -1

    a = juce.int16(-1)
    assert a.get() == -1

    a = juce.int32(-1)
    assert a.get() == -1

    a = juce.int64(-1)
    assert a.get() == -1

#==================================================================================================

def test_ints_constructor_out_of_range():
    with pytest.raises(TypeError):
        a = juce.int8(1_024)

    with pytest.raises(TypeError):
        a = juce.int16(65_537)

    with pytest.raises(TypeError):
        a = juce.int32(2_147_483_648)

    with pytest.raises(TypeError):
        a = juce.int64(9_223_372_036_854_775_808)

#==================================================================================================

def test_uints_constructor_positive():
    a = juce.uint8(1)
    assert a.get() == 1

    a = juce.uint16(1)
    assert a.get() == 1

    a = juce.uint32(1)
    assert a.get() == 1

    a = juce.uint64(1)
    assert a.get() == 1

#==================================================================================================

def test_uints_constructor_negative():
    with pytest.raises(TypeError):
        a = juce.uint8(-1)

    with pytest.raises(TypeError):
        a = juce.uint16(-1)

    with pytest.raises(TypeError):
        a = juce.uint32(-1)

    with pytest.raises(TypeError):
        a = juce.uint64(-1)

#==================================================================================================

def test_uints_constructor_out_of_range():
    with pytest.raises(TypeError):
        a = juce.uint8(1_024)

    with pytest.raises(TypeError):
        a = juce.uint16(65_537)

    with pytest.raises(TypeError):
        a = juce.uint32(4_294_967_296)

    with pytest.raises(TypeError):
        a = juce.uint64(18_446_744_073_709_551_616)
