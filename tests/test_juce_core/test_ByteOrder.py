
import sys

import popsicle as juce


def test_swap_python_integer():
    a = 1337
    assert a != juce.ByteOrder.swap(a)
    assert a == juce.ByteOrder.swap(juce.ByteOrder.swap(a))


def test_littleEndianShort_python_integer():
    assert juce.ByteOrder.littleEndianShort(b'\x01\x00') == 1
    assert juce.ByteOrder.littleEndianShort(b'\xff\x00') == 255


def test_littleEndianInt_python_integer():
    assert juce.ByteOrder.littleEndianInt(b'\x01\x00\x00\00') == 1
    assert juce.ByteOrder.littleEndianInt(b'\xff\x00\x00\00') == 255


def test_littleEndianInt64_python_integer():
    assert juce.ByteOrder.littleEndianInt64(b'\x01\x00\x00\00\x00\x00\x00\x00') == 1
    assert juce.ByteOrder.littleEndianInt64(b'\xff\x00\x00\00\x00\x00\x00\x00') == 255


def test_bigEndianShort_python_integer():
    assert juce.ByteOrder.bigEndianShort(b'\x00\01') == 1
    assert juce.ByteOrder.bigEndianShort(b'\x00\xff') == 255


def test_bigEndianInt_python_integer():
    assert juce.ByteOrder.bigEndianInt(b'\x00\x00\x00\01') == 1
    assert juce.ByteOrder.bigEndianInt(b'\x00\x00\x00\xff') == 255


def test_bigEndianInt64_python_integer():
    assert juce.ByteOrder.bigEndianInt64(b'\x00\x00\x00\x00\x00\x00\x00\01') == 1
    assert juce.ByteOrder.bigEndianInt64(b'\x00\x00\x00\x00\x00\x00\x00\xff') == 255


def test_makeInt():
    assert juce.ByteOrder.makeInt(1, 0) == 1
    assert juce.ByteOrder.makeInt(255, 0) == 255
    assert juce.ByteOrder.makeInt(0, 1) == 256
    assert juce.ByteOrder.makeInt(255, 1) == 511
    assert juce.ByteOrder.makeInt(1, 0, 0, 0) == 1
    assert juce.ByteOrder.makeInt(255, 0, 0, 0) == 255
    assert juce.ByteOrder.makeInt(0, 0, 0, 1) == 16777216
    assert juce.ByteOrder.makeInt(255, 0, 0, 1) == 16777471
    assert juce.ByteOrder.makeInt(1, 0, 0, 0, 0, 0, 0, 0) == 1
    assert juce.ByteOrder.makeInt(255, 0, 0, 0, 0, 0, 0, 0) == 255
    assert juce.ByteOrder.makeInt(0, 0, 0, 0, 0, 0, 0, 1) == 72057594037927936
    assert juce.ByteOrder.makeInt(255, 0, 0, 0, 0, 0, 0, 1) == 72057594037928191


def test_isBigEndian():
    is_big_endian = sys.byteorder == "big"
    assert is_big_endian == juce.ByteOrder.isBigEndian()
