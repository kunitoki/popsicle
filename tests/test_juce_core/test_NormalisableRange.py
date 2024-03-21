import pytest

import popsicle as juce

#==================================================================================================

def test_normalisable_range_default_constructor():
    nr = juce.NormalisableRange[float]()
    assert nr.start == pytest.approx(0)
    assert nr.end == pytest.approx(1)
    assert nr.interval == pytest.approx(0)
    assert nr.skew == pytest.approx(1)
    assert not nr.symmetricSkew

#==================================================================================================

def test_normalisable_range_with_range_interval_skew():
    nr = juce.NormalisableRange[float](0, 10, 1, 0.5)
    assert nr.start == pytest.approx(0)
    assert nr.end == pytest.approx(10)
    assert nr.interval == pytest.approx(1)
    assert nr.skew == pytest.approx(0.5)
    assert not nr.symmetricSkew

#==================================================================================================

def test_normalisable_range_with_range():
    nr = juce.NormalisableRange[float](0, 10)
    assert nr.start == pytest.approx(0)
    assert nr.end == pytest.approx(10)
    assert nr.interval == pytest.approx(0)
    assert nr.skew == pytest.approx(1)
    assert not nr.symmetricSkew

#==================================================================================================

def test_normalisable_range_with_range_and_interval():
    nr = juce.NormalisableRange[float](0, 10, 2)
    assert nr.start == pytest.approx(0)
    assert nr.end == pytest.approx(10)
    assert nr.interval == pytest.approx(2)
    assert nr.skew == pytest.approx(1)
    assert not nr.symmetricSkew

#==================================================================================================

def test_convert_to_0to1_with_linear_skew():
    nr = juce.NormalisableRange[float](0, 10)
    assert nr.convertTo0to1(0) == pytest.approx(0.0)
    assert nr.convertTo0to1(5) == pytest.approx(0.5)

#==================================================================================================

def test_convert_to_0to1_with_skew():
    nr = juce.NormalisableRange[float](0, 10, 1, 0.5)
    result = nr.convertTo0to1(5)
    assert 0.5 < result < 1.0

    nr = juce.NormalisableRange[float](0, 10, 1, 1.5)
    result = nr.convertTo0to1(5)
    assert 0.0 < result < 0.5

#==================================================================================================

def test_convert_from_0to1_with_linear_skew():
    nr = juce.NormalisableRange[float](0, 10)
    assert nr.convertFrom0to1(0.0) == pytest.approx(0)
    assert nr.convertFrom0to1(0.5) == pytest.approx(5)
    assert nr.convertFrom0to1(1.0) == pytest.approx(10)

#==================================================================================================

def test_convert_from_0to1_with_skew():
    nr = juce.NormalisableRange[float](0, 10, 1, 1.5)
    result = nr.convertFrom0to1(0.5)
    assert result > 5

    nr = juce.NormalisableRange[float](0, 10, 1, 0.5)
    result = nr.convertFrom0to1(0.5)
    assert result < 5

#==================================================================================================

def test_snap_to_legal_value_with_interval():
    nr = juce.NormalisableRange[float](0, 10, 1)
    assert nr.snapToLegalValue(5.4) == pytest.approx(5)
    assert nr.snapToLegalValue(5.6) == pytest.approx(6)

#==================================================================================================

def test_get_range():
    nr = juce.NormalisableRange[float](0, 10)
    range = nr.getRange()
    assert range.getStart() == 0
    assert range.getEnd() == 10

#==================================================================================================

def test_set_skew_for_centre():
    nr = juce.NormalisableRange[float](0, 10)
    nr.setSkewForCentre(2.5)
    assert nr.skew != 1
    assert not nr.symmetricSkew
