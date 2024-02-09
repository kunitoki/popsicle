import pytest

import popsicle as juce

#==================================================================================================

def test_construct_default():
    c = juce.Colour()
    assert c.getARGB() == 0x00000000

def test_construct_copy():
    c1 = juce.Colour(0xFF112233)
    c2 = juce.Colour(c1)
    assert c1 == c2

def test_construct_rgb():
    c = juce.Colour(0x11, 0x22, 0x33)
    assert c.getARGB() == 0xFF112233

def test_construct_rgba():
    c = juce.Colour(0x11, 0x22, 0x33, 0x44)
    assert c.getARGB() == 0x44112233

def test_construct_hsb_int_alpha():
    c = juce.Colour(0.5, 0.6, 0.7, 0xFF)
    assert c.getARGB() == 0xFF47B2B2

def test_construct_hsb():
    c = juce.Colour(0.5, 0.6, 0.7, 1.0)
    assert c.getARGB() == 0xFF47B2B2

def test_construct_rgba_float_alpha():
    c = juce.Colour(0x11, 0x22, 0x33, 0.4)
    assert c.getARGB() == 0x66112233

def test_construct_from_argb():
    c = juce.Colour(0xFF112233)
    assert c.getARGB() == 0xFF112233

def test_construct_from_rgb():
    c = juce.Colour.fromRGB(0x11, 0x22, 0x33)
    assert c.getARGB() == 0xFF112233

def test_construct_from_rgba():
    c = juce.Colour.fromRGBA(0x11, 0x22, 0x33, 0x44)
    assert c.getARGB() == 0x44112233

def test_construct_from_float_rgba():
    c = juce.Colour.fromFloatRGBA(0.1, 0.2, 0.3, 0.4)
    assert c.getARGB() == 0x661A334C

def test_construct_from_hsv():
    c = juce.Colour.fromHSV(0.5, 0.6, 0.7, 0.8)
    assert c.getARGB() == 0xCC47B2B2

def test_construct_from_hsl():
    c = juce.Colour.fromHSL(0.5, 0.6, 0.7, 0.8)
    assert c.getARGB() == 0xCC85E0E0

def test_construct_with_pixel_argb():
    pixel_argb = juce.PixelARGB(255, 0, 255, 0)
    color = juce.Colour(pixel_argb)
    assert color.getARGB() == 0xFF00FF00

def test_construct_with_pixel_rgb():
    pixel_rgb = juce.PixelRGB(0, 255, 0)
    color = juce.Colour(pixel_rgb)
    assert color.getARGB() == 0xFF00FF00

def test_construct_with_pixel_alpha():
    pixel_alpha = juce.PixelAlpha(255)
    color = juce.Colour(pixel_alpha)
    assert color.getARGB() == 0xFFFFFFFF

#==================================================================================================

def test_getters():
    c = juce.Colour(0xFF112233)
    assert c.getRed() == 0x11
    assert c.getGreen() == 0x22
    assert c.getBlue() == 0x33
    assert c.getAlpha() == 0xFF
    assert c.getFloatRed() == pytest.approx(0.0666667)
    assert c.getFloatGreen() == pytest.approx(0.1333333)
    assert c.getFloatBlue() == pytest.approx(0.2)
    assert c.getFloatAlpha() == pytest.approx(1.0)

#==================================================================================================

def test_get_pixel_argb():
    c = juce.Colour(0xFFAABBCC)
    pixel_argb = c.getPixelARGB()
    assert pixel_argb.getNativeARGB() == 0xFFAABBCC

#==================================================================================================

def test_get_non_premultiplied_pixel_argb():
    c = juce.Colour(0x80AABBCC)
    non_premultiplied_pixel_argb = c.getNonPremultipliedPixelARGB()
    assert non_premultiplied_pixel_argb.getNativeARGB() == 0x80AABBCC

#==================================================================================================

def test_equality():
    color1 = juce.Colour(0xFF00FF00)
    color2 = juce.Colour(0xFF00FF00)
    assert color1 == color2

def test_inequality():
    color1 = juce.Colour(0xFF00FF00)
    color2 = juce.Colour(0xFFFF0000)
    assert color1 != color2

#==================================================================================================

def test_is_opaque():
    opaque_color = juce.Colour(0xFF00FF00)
    transparent_color = juce.Colour(0x00000000)
    assert opaque_color.isOpaque()
    assert not transparent_color.isOpaque()

#==================================================================================================

def test_with_alpha_uint8():
    color = juce.Colour(0xFF00FF00)
    new_color = color.withAlpha(128)
    assert new_color.getAlpha() == 0x80
    assert new_color.getARGB() == 0x8000FF00

def test_with_alpha_float():
    color = juce.Colour(0xFF00FF00)
    new_color = color.withAlpha(0.5)
    assert new_color.getAlpha() == 0x80
    assert new_color.getARGB() == 0x8000FF00

#==================================================================================================

def test_with_multiplied_alpha():
    c = juce.Colour(0xFFAABBCC)
    new_colour = c.withMultipliedAlpha(0.5)
    assert new_colour.getARGB() == 0x80AABBCC

#==================================================================================================

def test_get_hue():
    c = juce.Colour(0xFFAABBCC)
    assert c.getHue() == pytest.approx(0.5833333)

def test_get_saturation():
    color = juce.Colour(0xFF00FF00)
    assert color.getSaturation() == pytest.approx(1.0)

def test_get_saturation_hsl():
    color = juce.Colour(0xFF00FFFF)
    assert color.getSaturationHSL() == pytest.approx(1.0)

def test_get_brightness():
    color = juce.Colour(0xFF0000FF)
    assert color.getBrightness() == pytest.approx(1.0)

def test_get_lightness():
    color = juce.Colour(0xFF0000FF)
    assert color.getLightness() == pytest.approx(0.5)

def test_get_perceived_brightness():
    color = juce.Colour(0xFF0000FF)
    assert color.getPerceivedBrightness() == pytest.approx(0.260768)

#==================================================================================================

def test_get_hsb():
    color = juce.Colour(0xFF0000FF)
    hue, saturation, brightness = color.getHSB()
    assert hue == pytest.approx(0.6666666666)
    assert saturation == pytest.approx(1.0)
    assert brightness == pytest.approx(1.0)

def test_get_hsl():
    color = juce.Colour(0xFF0000FF)
    hue, saturation, lightness = color.getHSL()
    assert hue == pytest.approx(0.6666666666)
    assert saturation == pytest.approx(1.0)
    assert lightness == pytest.approx(0.5)

#==================================================================================================

def test_with_hue():
    original_color = juce.Colour(0xFFFF0000)
    new_color = original_color.withHue(0.5)
    assert new_color.getARGB() == 0xFF00FFFF

def test_with_saturation():
    original_color = juce.Colour(0xFF0000FF)
    new_color = original_color.withSaturation(0.5)
    assert new_color.getARGB() == 0xFF8080FF

def test_with_brightness():
    original_color = juce.Colour(0xFF808080)
    new_color = original_color.withBrightness(0.5)
    assert new_color.getARGB() == 0xFF808080

def test_with_saturation_hsl():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withSaturationHSL(0.5)
    assert new_colour.getARGB() == 0xFF4040BF

def test_with_lightness():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withLightness(0.5)
    assert new_colour.getARGB() == 0xFF0000FF

def test_with_rotated_hue():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withRotatedHue(0.1)
    assert new_colour.getARGB() == 0xFF9900FF

#==================================================================================================

def test_with_multiplied_saturation():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withMultipliedSaturation(0.5)
    assert new_colour.getARGB() == 0xFF8080FF

def test_with_multiplied_saturation_hsl():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withMultipliedSaturationHSL(0.5)
    assert new_colour.getARGB() == 0xFF4040BF

def test_with_multiplied_brightness():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withMultipliedBrightness(0.5)
    assert new_colour.getARGB() == 0xFF000080

def test_with_multiplied_lightness():
    colour = juce.Colour(0xFF0000FF)
    new_colour = colour.withMultipliedLightness(0.5)
    assert new_colour.getARGB() == 0xFF000080

#==================================================================================================

def test_brighter():
    colour = juce.Colour(0xFF0000FF)
    brighter_colour = colour.brighter(1.5)
    assert brighter_colour.getARGB() == 0xFF9999FF

def test_darker():
    colour = juce.Colour(0xFF0000FF)
    darker_colour = colour.darker(1.5)
    assert darker_colour.getARGB() == 0xFF000066

#==================================================================================================

def test_contrasting():
    colour = juce.Colour(0xFF0000FF)
    contrasting_colour = colour.contrasting(1.0)
    assert isinstance(contrasting_colour, juce.Colour)

def test_contrasting_with_min_luminosity_diff():
    colour = juce.Colour(0xFF0000FF)
    contrasting_colour = colour.contrasting(juce.Colour(0xFFFFFFFF), 0.5)
    assert contrasting_colour.getARGB() == 0xFFFFFFFF

def test_contrasting_static():
    colour1 = juce.Colour(0xFF0000FF)
    colour2 = juce.Colour(0xFFFFFFFF)
    contrasting_colour = juce.Colour.contrasting(colour1, 0.5)
    assert isinstance(contrasting_colour, juce.Colour)
    contrasting_colour = juce.Colour.contrasting(colour1, colour2, 0.5)
    assert isinstance(contrasting_colour, juce.Colour)

#==================================================================================================

def test_grey_level():
    grey_colour = juce.Colour.greyLevel(0.5)
    assert grey_colour.getARGB() == 0xFF808080

#==================================================================================================

def test_overlaid_with():
    c1 = juce.Colour(0xFFFF0000)  # Red
    c2 = juce.Colour(0xFF00FF00)  # Green
    c3 = juce.Colour(0x7F00FF00)  # Green
    overlaid = c1.overlaidWith(c2)
    assert overlaid.getARGB() == 0xFF00FF00
    overlaid = c1.overlaidWith(c3)
    assert overlaid.getARGB() == 0xFF7F7F00
    overlaid = c3.overlaidWith(c1)
    assert overlaid.getARGB() == 0xFFFF0000

#==================================================================================================

def test_interpolated_with():
    c1 = juce.Colour(0xFFFF0000)  # Red
    c2 = juce.Colour(0xFF0000FF)  # Blue
    interpolated = c1.interpolatedWith(c2, 0.0)
    assert interpolated == c1
    interpolated = c1.interpolatedWith(c2, 0.5)
    assert interpolated.getARGB() == 0xFF7F007F
    interpolated = c1.interpolatedWith(c2, 1.0)
    assert interpolated == c2

#==================================================================================================

def test_to_string():
    colour = juce.Colour(0xFF0000FF)
    assert isinstance(colour.toString(), str)

def test_to_display_string():
    colour = juce.Colour(0xFF0000FF)
    assert isinstance(colour.toDisplayString(True), str)
