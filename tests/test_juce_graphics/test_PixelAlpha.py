import sys

import popsicle as juce

#==================================================================================================

def test_pixel_alpha_default_constructor():
    pixel = juce.PixelAlpha()
    assert pixel.getAlpha() == 0

#==================================================================================================

def test_pixel_alpha_set_alpha():
    pixel = juce.PixelAlpha()
    pixel.setAlpha(128)
    assert pixel.getAlpha() == 128

#==================================================================================================

def test_pixel_alpha_get_native_argb():
    pixel = juce.PixelAlpha(128)
    assert pixel.getNativeARGB() == (128 << 24) | (128 << 16) | (128 << 8) | 128

#==================================================================================================

def test_pixel_alpha_get_in_argb_mask_order():
    pixel = juce.PixelAlpha(128)
    assert pixel.getInARGBMaskOrder() == (128 << 24) | (128 << 16) | (128 << 8) | 128

#==================================================================================================

def test_pixel_alpha_get_in_argb_memory_order():
    pixel = juce.PixelAlpha(128)
    expected = (128 << 24) | (128 << 16) | (128 << 8) | 128
    assert pixel.getInARGBMemoryOrder() == expected

#==================================================================================================

def test_pixel_alpha_get_even_bytes():
    pixel = juce.PixelAlpha(128)
    assert pixel.getEvenBytes() == (128 << 16) | 128

#==================================================================================================

def test_pixel_alpha_get_odd_bytes():
    pixel = juce.PixelAlpha(128)
    assert pixel.getOddBytes() == (128 << 16) | 128

#==================================================================================================

def test_pixel_alpha_set_argb():
    pixel = juce.PixelAlpha()
    pixel.setARGB(128, 0, 0, 0)
    assert pixel.getAlpha() == 128

#==================================================================================================

def test_pixel_alpha_blend_with_extra_alpha():
    src_pixel = juce.PixelAlpha(128)
    dst_pixel = juce.PixelAlpha(64)
    dst_pixel.blend(src_pixel, 128)
    assert dst_pixel.getAlpha() == 112

#==================================================================================================

def test_pixel_alpha_multiply_alpha_int():
    pixel = juce.PixelAlpha(100)
    pixel.multiplyAlpha(2)
    assert pixel.getAlpha() == 1

#==================================================================================================

def test_pixel_alpha_multiply_alpha_float():
    pixel = juce.PixelAlpha(100)
    pixel.multiplyAlpha(0.5)
    assert pixel.getAlpha() == 50

#==================================================================================================

def test_pixel_alpha_premultiply():
    pixel = juce.PixelAlpha(128)
    pixel.premultiply()
    assert pixel.getAlpha() == 128

#==================================================================================================

def test_pixel_alpha_unpremultiply():
    pixel = juce.PixelAlpha(128)
    pixel.unpremultiply()
    assert pixel.getAlpha() == 128

#==================================================================================================

def test_pixel_alpha_desaturate():
    pixel = juce.PixelAlpha(128)
    pixel.desaturate()
    assert pixel.getAlpha() == 128
