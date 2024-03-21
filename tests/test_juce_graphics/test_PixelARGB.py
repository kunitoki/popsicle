import sys

import popsicle as juce

#==================================================================================================

def test_pixel_argb_default_constructor():
    pixel = juce.PixelARGB()
    assert pixel.getAlpha() == 0
    assert pixel.getRed() == 0
    assert pixel.getGreen() == 0
    assert pixel.getBlue() == 0

#==================================================================================================

def test_pixel_argb_constructor_with_values():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    assert pixel.getAlpha() == 255
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 64
    assert pixel.getBlue() == 32

#==================================================================================================

def test_pixel_argb_get_native_argb():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    assert pixel.getNativeARGB() == (255 << 24 | 128 << 16 | 64 << 8 | 32)

#==================================================================================================

def test_pixel_argb_get_in_argb_mask_order():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    assert pixel.getInARGBMaskOrder() == (255 << 24 | 128 << 16 | 64 << 8 | 32)

#==================================================================================================

def test_pixel_argb_get_in_argb_memory_order():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    if sys.byteorder == 'big':
        expected = (255 << 24 | 128 << 16 | 64 << 8 | 32)
    else:
        expected = (32 << 24 | 64 << 16 | 128 << 8 | 255)
    assert pixel.getInARGBMemoryOrder() == expected

#==================================================================================================

def test_pixel_argb_get_even_bytes():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    assert pixel.getEvenBytes() == (128 << 16 | 32)

#==================================================================================================

def test_pixel_argb_get_odd_bytes():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    assert pixel.getOddBytes() == (255 << 16 | 64)

#==================================================================================================

def test_pixel_argb_set_argb():
    pixel = juce.PixelARGB()
    pixel.setARGB(255, 128, 64, 32)
    assert pixel.getAlpha() == 255
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 64
    assert pixel.getBlue() == 32

#==================================================================================================

def test_pixel_argb_blend():
    src_pixel = juce.PixelARGB(128, 32, 16, 8)
    dst_pixel = juce.PixelARGB(255, 128, 64, 32)
    dst_pixel.blend(src_pixel)
    assert dst_pixel.getAlpha() == 255
    assert dst_pixel.getRed() == 96
    assert dst_pixel.getGreen() == 48
    assert dst_pixel.getBlue() == 24

#==================================================================================================

def test_pixel_argb_set_alpha():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    pixel.setAlpha(128)
    assert pixel.getAlpha() == 128

#==================================================================================================

"""
def test_pixel_argb_multiply_alpha_int():
    pixel = juce.PixelARGB(100, 128, 64, 32)
    pixel.multiplyAlpha(1)
    assert pixel.getAlpha() == 200
"""

#==================================================================================================

def test_pixel_argb_multiply_alpha_float():
    pixel = juce.PixelARGB(100, 128, 64, 32)
    pixel.multiplyAlpha(0.5)
    assert pixel.getAlpha() == 50

#==================================================================================================

def test_pixel_argb_premultiply():
    pixel = juce.PixelARGB(128, 255, 255, 255)
    pixel.premultiply()
    expected_red = int((255 * 128 + 0x7f) >> 8)
    expected_green = int((255 * 128 + 0x7f) >> 8)
    expected_blue = int((255 * 128 + 0x7f) >> 8)
    assert pixel.getRed() == expected_red
    assert pixel.getGreen() == expected_green
    assert pixel.getBlue() == expected_blue

#==================================================================================================

def test_pixel_argb_unpremultiply():
    pixel = juce.PixelARGB(128, 128, 128, 128)
    pixel.unpremultiply()
    assert pixel.getRed() > 128
    assert pixel.getGreen() > 128
    assert pixel.getBlue() > 128

#==================================================================================================

def test_pixel_argb_desaturate():
    pixel = juce.PixelARGB(255, 128, 64, 32)
    pixel.desaturate()
    expected_value = int((128 + 64 + 32) / 3)
    assert pixel.getRed() == expected_value
    assert pixel.getGreen() == expected_value
    assert pixel.getBlue() == expected_value
