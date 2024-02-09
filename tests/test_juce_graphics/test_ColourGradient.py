import pytest

import popsicle as juce

#==================================================================================================

def test_construct_default():
    c = juce.ColourGradient()
    assert c.getNumColours() == 0
    assert c.isOpaque()
    assert c.isRadial == False
    assert c.point1 == juce.Point[float](0, 0) or c.point1 == juce.Point[float](987654, 0)
    assert c.point2 == juce.Point[float](0, 0)

#==================================================================================================

def test_copy_construct():
    c1 = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    c2 = juce.ColourGradient(c1)
    assert c1 == c2
    assert not (c1 != c2)

#==================================================================================================

def test_construct_coordinates_linear():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    assert c.isRadial == False
    assert c.point1 == juce.Point[float](0, 0)
    assert c.point2 == juce.Point[float](1, 1)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_construct_points_linear():
    c = juce.ColourGradient(juce.Colours.black, juce.Point[float](0.0, 0.0), juce.Colours.white, juce.Point[float](1.0, 1.0), False)
    assert c.isRadial == False
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

#==================================================================================================

def test_construct_coordinates_radial():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, True)
    assert c.isRadial == True
    assert c.point1 == juce.Point[float](0, 0)
    assert c.point2 == juce.Point[float](1, 1)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_construct_points_radial():
    c = juce.ColourGradient(juce.Colours.black, juce.Point[float](0.0, 0.0), juce.Colours.white, juce.Point[float](1.0, 1.0), True)
    assert c.isRadial == True
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

#==================================================================================================

def test_colour_at_position():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    assert c.getColourAtPosition(0.0) == juce.Colours.black
    assert c.getColourAtPosition(0.5) == juce.Colour(127, 127, 127)
    assert c.getColourAtPosition(1.0) == juce.Colours.white

#==================================================================================================

def test_get_colour_position():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    assert c.getColourPosition(0) == pytest.approx(0.0)
    assert c.getColourPosition(1) == pytest.approx(1.0)

#==================================================================================================

def test_set_colour():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    c.setColour(0, juce.Colours.red)
    c.setColour(1, juce.Colours.green)
    assert c.getColour(0) == juce.Colours.red
    assert c.getColour(1) == juce.Colours.green

#==================================================================================================

def test_add_remove_colour():
    c = juce.ColourGradient()
    assert c.getNumColours() == 0

    c.addColour(0.0, juce.Colours.yellow)
    assert c.getNumColours() == 1
    assert c.getColour(0) == juce.Colours.yellow

    c.addColour(0.5, juce.Colours.green)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.yellow
    assert c.getColour(1) == juce.Colours.green

    c.addColour(1.0, juce.Colours.red)
    assert c.getNumColours() == 3
    assert c.getColour(0) == juce.Colours.yellow
    assert c.getColour(1) == juce.Colours.green
    assert c.getColour(2) == juce.Colours.red

    c.removeColour(1)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.yellow
    assert c.getColour(1) == juce.Colours.red

    c.clearColours()
    assert c.getNumColours() == 0

#==================================================================================================

@pytest.mark.skip(reason="JUCE bug, assert raises when removing index 0")
def test_remove_colour():
    c = juce.ColourGradient()
    assert c.getNumColours() == 0

    c.addColour(0.0, juce.Colours.yellow)
    assert c.getNumColours() == 1
    assert c.getColour(0) == juce.Colours.yellow

    c.removeColour(0)
    assert c.getNumColours() == 0

#==================================================================================================

def test_vertical_values():
    c = juce.ColourGradient.vertical(juce.Colours.black, 0.0, juce.Colours.white, 1.0)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_vertical_rectangle_int():
    c = juce.ColourGradient.vertical(juce.Colours.black, juce.Colours.white, juce.Rectangle[int](0, 0, 100, 100))
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_vertical_rectangle_float():
    c = juce.ColourGradient.vertical(juce.Colours.black, juce.Colours.white, juce.Rectangle[float](0, 0, 100, 100))
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

#==================================================================================================

def test_horizontal_values():
    c = juce.ColourGradient.horizontal(juce.Colours.black, 0.0, juce.Colours.white, 1.0)
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_horizontal_rectangle_int():
    c = juce.ColourGradient.horizontal(juce.Colours.black, juce.Colours.white, juce.Rectangle[int](0, 0, 100, 100))
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

def test_horizontal_rectangle_float():
    c = juce.ColourGradient.horizontal(juce.Colours.black, juce.Colours.white, juce.Rectangle[float](0, 0, 100, 100))
    assert c.getNumColours() == 2
    assert c.getColour(0) == juce.Colours.black
    assert c.getColour(1) == juce.Colours.white

#==================================================================================================

def test_is_invisible_is_opaque():
    c = juce.ColourGradient(juce.Colours.black, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    assert not c.isInvisible()
    assert c.isOpaque()

    c = juce.ColourGradient(juce.Colours.transparentBlack, 0.0, 0.0, juce.Colours.white, 1.0, 1.0, False)
    assert not c.isInvisible()
    assert not c.isOpaque()

    c = juce.ColourGradient(juce.Colours.transparentBlack, 0.0, 0.0, juce.Colours.transparentWhite, 1.0, 1.0, False)
    assert c.isInvisible()
    assert not c.isOpaque()
