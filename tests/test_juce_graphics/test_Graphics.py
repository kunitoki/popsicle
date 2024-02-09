import sys
import pytest

import popsicle as juce
from .utilities import get_logo_image, is_image_equal_reference

#if sys.platform == "win32":
#    pytest.skip(allow_module_level=True)

#==================================================================================================

width = 400
height = 400

#==================================================================================================

@pytest.fixture(scope="session")
def update_rendering(pytestconfig):
    return pytestconfig.getoption("update_rendering")

#==================================================================================================

def test_fill_all(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)

    g.setColour(juce.Colours.slategrey)
    g.fillAll()

    assert is_image_equal_reference(update_rendering, image)

@pytest.mark.skipif(sys.platform != "darwin", reason="Png format loads differently in each platform")
def test_fill_all_image(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)

    g.setTiledImageFill(get_logo_image(), 0, 0, 1.0)
    g.fillAll()

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

def test_fill_rect(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(50)

    g.setColour(juce.Colours.magenta)
    g.fillRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight())

    assert is_image_equal_reference(update_rendering, image)

def test_fill_rect_area(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)

    g.setColour(juce.Colours.magenta)
    g.fillRect(image.getBounds().reduced(50))

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

def test_fill_rect_gradient_vertical(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(50)

    c = juce.ColourGradient.vertical(juce.Colours.magenta, 0.0, juce.Colours.yellow, rect.getHeight())
    g.setFillType(juce.FillType(c))
    g.fillRect(rect)

    assert is_image_equal_reference(update_rendering, image)

def test_fill_rect_gradient_vertical_area(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(50)

    c = juce.ColourGradient.vertical(juce.Colours.magenta, juce.Colours.yellow, rect)
    g.setGradientFill(c)
    g.fillRect(rect)

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

def test_fill_rect_gradient_horizontal(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(50)

    c = juce.ColourGradient.horizontal(juce.Colours.red, 0.0, juce.Colours.green, rect.getWidth())
    g.setFillType(juce.FillType(c))
    g.fillRect(rect)

    assert is_image_equal_reference(update_rendering, image)

def test_fill_rect_gradient_horizontal_area(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(50)

    c = juce.ColourGradient.horizontal(juce.Colours.red, juce.Colours.green, rect)
    g.setGradientFill(c)
    g.fillRect(rect)

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

@pytest.mark.skipif(sys.platform != "darwin", reason="Png format loads differently in each platform")
def test_fill_rect_image(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    rect = image.getBounds().reduced(100)

    g.setTiledImageFill(get_logo_image(), 50, 50, 1.0)
    g.fillRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight())

    assert is_image_equal_reference(update_rendering, image)

@pytest.mark.skipif(sys.platform != "darwin", reason="Png format loads differently in each platform")
def test_fill_rect_image_area(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)

    g.setTiledImageFill(get_logo_image(), 50, 50, 1.0)
    g.fillRect(image.getBounds().reduced(100))

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

def test_draw_rect(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    g.fillAll(juce.Colours.black)

    c = juce.Colours.darkmagenta.darker(1.0)
    b = image.getBounds()
    for x in range(15):
        c = c.brighter(0.1)
        b = b.reduced(15 + x)
        g.setColour(c)
        g.drawRect(b, 15 - x)

        g.setColour(juce.Colours.yellow)
        g.drawRect(b, 1)

    assert is_image_equal_reference(update_rendering, image)

#==================================================================================================

def test_draw_rounded_rect(update_rendering):
    image = juce.Image(juce.Image.ARGB, width, height, True)
    llsr = juce.LowLevelGraphicsSoftwareRenderer(image)
    g = juce.Graphics(llsr)
    g.fillAll(juce.Colours.black)

    c = juce.Colours.darkmagenta.darker(1.0)
    b = image.getBounds().toFloat()
    k = 0
    for x in range(15):
        c = c.brighter(0.1)
        b = b.reduced(15 + x)
        k = k + 4
        g.setColour(c)
        g.drawRoundedRectangle(b, k, 15 - x)

        g.setColour(juce.Colours.yellow)
        g.drawRect(b, 1)

    assert is_image_equal_reference(update_rendering, image, threshold=1e-09)
