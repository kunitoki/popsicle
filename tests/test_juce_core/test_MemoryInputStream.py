import os
import pytest

import popsicle as juce

#==================================================================================================

def test_constructor_with_empty_source_data():
    source_data = b""

    stream = juce.MemoryInputStream(source_data, False)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

    stream = juce.MemoryInputStream(source_data, True)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

#==================================================================================================

def test_constructor_with_source_data():
    source_data = b"Hello, world!"

    stream = juce.MemoryInputStream(source_data, False)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

    stream = juce.MemoryInputStream(source_data, True)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

#==================================================================================================

def test_constructor_with_empty_memory_block():
    source_data = b""
    memory_block = juce.MemoryBlock(source_data)

    stream = juce.MemoryInputStream(memory_block, False)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

    stream = juce.MemoryInputStream(memory_block, True)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

#==================================================================================================

def test_constructor_with_memory_block():
    source_data = b"Hello, world!"
    memory_block = juce.MemoryBlock(source_data)

    stream = juce.MemoryInputStream(memory_block, False)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)

    stream = juce.MemoryInputStream(memory_block, True)
    assert stream.getData() == source_data
    assert stream.getDataSize() == len(source_data)
