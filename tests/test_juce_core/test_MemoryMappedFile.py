import os
import sys
import pytest

import popsicle as juce
from popsicle import int64

#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))
data_folder = this_file.getSiblingFile("data")

#==================================================================================================

def test_memory_mapped_file_read_only_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.MemoryMappedFile.readOnly)
    assert mmf.getData() is not None
    assert mmf.getSize() > 0
    assert mmf.getRange().getStart() >= 0
    assert mmf.getRange().getEnd() > mmf.getRange().getStart()

#==================================================================================================

def test_memory_mapped_file_read_write_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.MemoryMappedFile.readWrite)
    assert mmf.getData() is not None
    assert mmf.getSize() > 0
    assert mmf.getRange().getStart() >= 0
    assert mmf.getRange().getEnd() > mmf.getRange().getStart()

#==================================================================================================

@pytest.mark.skipif(sys.platform == "win32")
def test_memory_mapped_file_exclusive_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.MemoryMappedFile.readOnly, exclusive=True)
    assert mmf.getData() is not None
    assert mmf.getSize() > 0

#==================================================================================================

def test_memory_mapped_file_non_existent_file():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("nonexistent.txt"), juce.MemoryMappedFile.readOnly)
    assert mmf.getData() is None
    assert mmf.getSize() == 0

#==================================================================================================

def test_memory_mapped_file_section_read_only_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.Range[int64](0, 10), juce.MemoryMappedFile.readOnly)
    assert mmf.getData() is not None
    assert mmf.getSize() == 10
    assert mmf.getRange().getLength() == 10

#==================================================================================================

def test_memory_mapped_file_section_read_write_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.Range[int64](0, 10), juce.MemoryMappedFile.readWrite)
    assert mmf.getData() is not None
    assert mmf.getSize() == 10
    assert mmf.getRange().getLength() == 10

#==================================================================================================

@pytest.mark.skipif(sys.platform == "win32")
def test_memory_mapped_file_section_exclusive_mode():
    mmf = juce.MemoryMappedFile(data_folder.getChildFile("somefile.txt"), juce.Range[int64](0, 10), juce.MemoryMappedFile.readOnly, exclusive=True)
    assert mmf.getData() is not None
    assert mmf.getSize() == 10
