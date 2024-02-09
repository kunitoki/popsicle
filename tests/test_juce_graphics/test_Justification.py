import popsicle as juce

#==================================================================================================

def test_constructor():
    b = juce.Justification(juce.Justification.centred)
    assert b.getFlags() == juce.Justification.centred

    b = juce.Justification(juce.Justification.verticallyCentred | juce.Justification.horizontallyJustified)
    assert b.getFlags() == juce.Justification.verticallyCentred | juce.Justification.horizontallyJustified
    assert b.testFlags(juce.Justification.verticallyCentred)
    assert b.testFlags(juce.Justification.horizontallyJustified)
    assert b.testFlags(juce.Justification.centred)
    assert not b.testFlags(juce.Justification.left)
    assert not b.testFlags(juce.Justification.right)

#==================================================================================================

def test_comparison():
    b = juce.Justification(juce.Justification.centred)
    c = juce.Justification(juce.Justification.left)
    assert b == juce.Justification.centred
    assert b != juce.Justification.left
    assert c != juce.Justification.centred
    assert c == juce.Justification.left
    assert b == b
    assert b != c

#==================================================================================================

def test_get_only_flags():
    b = juce.Justification(juce.Justification.verticallyCentred | juce.Justification.horizontallyJustified)
    assert b.getOnlyVerticalFlags() == juce.Justification.verticallyCentred
    assert b.getOnlyHorizontalFlags() == juce.Justification.horizontallyJustified

#==================================================================================================

def test_apply_to_rectangle_int():
    b = juce.Justification(juce.Justification.left)

    outer = juce.Rectangle[int](10, 10, 100, 100)
    inner = juce.Rectangle[int](20, 20, 20, 20)

    result = b.appliedToRectangle(inner, outer)
    assert result == juce.Rectangle[int](10, 10, 20, 20)

    result = b.appliedToRectangle(outer, inner)
    assert result == juce.Rectangle[int](20, 20, 100, 100)

#==================================================================================================

def test_apply_to_rectangle_float():
    b = juce.Justification(juce.Justification.bottomRight)

    outer = juce.Rectangle[float](10, 10, 100, 100)
    inner = juce.Rectangle[float](20, 20, 20, 20)

    result = b.appliedToRectangle(inner, outer)
    assert result == juce.Rectangle[float](90, 90, 20, 20)

    result = b.appliedToRectangle(outer, inner)
    assert result == juce.Rectangle[float](-60, -60, 100, 100)
