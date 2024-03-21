import sys

import popsicle as juce

#==================================================================================================

def test_pixel_rgb_default_constructor():
    pixel = juce.PixelRGB()
    assert pixel.getAlpha() == 255
    assert pixel.getRed() == 0
    assert pixel.getGreen() == 0
    assert pixel.getBlue() == 0

#==================================================================================================

def test_pixel_rgb_set_argb():
    pixel = juce.PixelRGB()
    pixel.setARGB(255, 128, 64, 32)
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 64
    assert pixel.getBlue() == 32

#==================================================================================================

def test_pixel_rgb_get_native_argb():
    pixel = juce.PixelRGB(128, 64, 32)
    assert pixel.getNativeARGB() == (0xFF << 24 | 128 << 16 | 64 << 8 | 32)

#==================================================================================================

def test_pixel_rgb_get_in_argb_mask_order():
    pixel = juce.PixelRGB(128, 64, 32)
    assert pixel.getInARGBMaskOrder() == (0xFF << 24 | 128 << 16 | 64 << 8 | 32)

#==================================================================================================

def test_pixel_rgb_get_in_argb_memory_order():
    pixel = juce.PixelRGB(128, 64, 32)
    if sys.byteorder == 'big':
        expected = (0xFF << 24 | 128 << 16 | 64 << 8 | 32)
    else:
        expected = (32 << 24 | 64 << 16 | 128 << 8 | 0xFF)
    assert pixel.getInARGBMemoryOrder() == expected

#==================================================================================================

def test_pixel_rgb_get_even_bytes():
    pixel = juce.PixelRGB(128, 64, 32)
    assert pixel.getEvenBytes() == (128 << 16 | 32)

#==================================================================================================

def test_pixel_rgb_get_odd_bytes():
    pixel = juce.PixelRGB(128, 64, 32)
    assert pixel.getOddBytes() == (0xFF << 16 | 64)

#==================================================================================================

def test_pixel_rgb_blend():
    src_pixel = juce.PixelRGB(32, 16, 8)
    dst_pixel = juce.PixelRGB(16, 64, 32)
    dst_pixel.blend(src_pixel)
    assert dst_pixel.getRed() == 32
    assert dst_pixel.getGreen() == 16
    assert dst_pixel.getBlue() == 8

#==================================================================================================

def test_pixel_rgb_multiply_alpha_int():
    pixel = juce.PixelRGB(128, 64, 32)
    pixel.multiplyAlpha(2)
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 64
    assert pixel.getBlue() == 32

#==================================================================================================

def test_pixel_rgb_multiply_alpha_float():
    pixel = juce.PixelRGB(128, 64, 32)
    pixel.multiplyAlpha(0.5)
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 64
    assert pixel.getBlue() == 32

#==================================================================================================

def test_pixel_rgb_premultiply():
    pixel = juce.PixelRGB(255, 255, 255)
    pixel.premultiply()
    assert pixel.getRed() == 255
    assert pixel.getGreen() == 255
    assert pixel.getBlue() == 255

#==================================================================================================

def test_pixel_rgb_unpremultiply():
    pixel = juce.PixelRGB(128, 128, 128)
    pixel.unpremultiply()
    assert pixel.getRed() == 128
    assert pixel.getGreen() == 128
    assert pixel.getBlue() == 128

#==================================================================================================

def test_pixel_rgb_desaturate():
    pixel = juce.PixelRGB(128, 64, 32)
    pixel.desaturate()
    expected_value = int((128 + 64 + 32) / 3)
    assert pixel.getRed() == expected_value
    assert pixel.getGreen() == expected_value
    assert pixel.getBlue() == expected_value
