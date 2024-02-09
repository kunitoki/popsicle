import pytest

from .utilities import get_logo_image
import popsicle as juce

#==================================================================================================

def test_default_constructor():
    f = juce.FillType()
    assert f.colour == juce.Colour(0xff000000)
    assert f.gradient is None
    assert f.image == juce.Image()
    assert f.transform == juce.AffineTransform()
    assert f.isColour()
    assert not f.isGradient()
    assert not f.isTiledImage()

#==================================================================================================

def test_colour_constructor():
    f = juce.FillType(juce.Colours.red)
    assert f.colour == juce.Colours.red
    assert f.gradient is None
    assert f.image == juce.Image()
    assert f.transform == juce.AffineTransform()
    assert f.isColour()
    assert not f.isGradient()
    assert not f.isTiledImage()

#==================================================================================================

def test_gradient_constructor():
    f = juce.FillType(juce.ColourGradient.vertical(juce.Colours.red, 0.0, juce.Colours.green, 1.0))
    assert f.colour == juce.Colour(0xff000000)
    assert f.gradient == juce.ColourGradient.vertical(juce.Colours.red, 0.0, juce.Colours.green, 1.0)
    assert f.image == juce.Image()
    assert f.transform == juce.AffineTransform()
    assert not f.isColour()
    assert f.isGradient()
    assert not f.isTiledImage()

#==================================================================================================

def test_image_constructor():
    f = juce.FillType(get_logo_image(), juce.AffineTransform.translation(10.0, 10))
    assert f.colour == juce.Colour(0xff000000)
    assert f.gradient is None
    assert f.image == get_logo_image()
    assert f.transform == juce.AffineTransform.translation(10.0, 10)
    assert not f.isColour()
    assert not f.isGradient()
    assert f.isTiledImage()

#==================================================================================================

def test_copy_constructor():
    f1 = juce.FillType(juce.Colours.red)
    f2 = juce.FillType(f1)
    assert f1.colour == f2.colour
    assert f1 == f2
    assert not (f1 != f2)

#==================================================================================================

def test_set_colour():
    f = juce.FillType(juce.ColourGradient.vertical(juce.Colours.red, 0.0, juce.Colours.green, 1.0))
    f.setColour(juce.Colours.red)
    assert f.isColour()
    assert not f.isGradient()

#==================================================================================================

def test_set_gradient():
    f = juce.FillType(juce.Colours.red)
    f.setGradient(juce.ColourGradient.vertical(juce.Colours.red, 0.0, juce.Colours.green, 1.0))
    assert not f.isColour()
    assert f.isGradient()

#==================================================================================================

def test_set_image():
    f = juce.FillType(juce.Colours.red)
    f.setTiledImage(get_logo_image(), juce.AffineTransform.translation(10.0, 10))
    assert not f.isColour()
    assert f.isTiledImage()

#==================================================================================================

def test_set_opacity():
    f = juce.FillType(juce.Colours.red)
    assert not f.isInvisible()
    assert f.getOpacity() == pytest.approx(1.0)

    f.setOpacity(0.0)
    assert f.isColour()
    assert f.getOpacity() == pytest.approx(0.0)
    assert f.isInvisible()

#==================================================================================================

def test_transformed():
    f = juce.FillType(get_logo_image(), juce.AffineTransform())
    result = f.transformed(juce.AffineTransform.translation(10.0, 10))
    assert result.transform == juce.AffineTransform.translation(10.0, 10)
