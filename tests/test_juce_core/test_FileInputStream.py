import os
import pytest

import popsicle as juce


#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))
data_folder = this_file.getSiblingFile("data")

#==================================================================================================

def test_file_input_stream_nonexisting():
    a = juce.FileInputStream(data_folder.getChildFile("nonexisting.txt"))
    assert not a.openedOk()
    assert a.failedToOpen()
    assert a.getFile() == data_folder.getChildFile("nonexisting.txt")
    assert a.getStatus().failed()

#==================================================================================================

def test_file_input_stream():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    assert a.openedOk()
    assert not a.failedToOpen()
    assert a.getFile() == data_folder.getChildFile("somefile.txt")
    assert a.getStatus().wasOk()

#==================================================================================================

def test_file_input_stream_read():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    assert a.getNumBytesRemaining() > 0
    assert a.getTotalLength() == a.getNumBytesRemaining()
    assert a.getPosition() == 0

    x = bytearray(31)
    assert a.read(x) == 31
    assert x.decode("UTF8") == "-----BEGIN RSA PRIVATE KEY-----"
    assert a.getTotalLength() > a.getNumBytesRemaining()
    assert a.getPosition() == 31

    next_byte = a.readByte()
    assert next_byte == "\n" or next_byte == "\r"

#==================================================================================================

def test_file_input_stream_read_ints():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))

    assert a.readBool() != False
    assert a.readByte() == '-'
    assert a.readShort() == 11565
    assert a.readShortBigEndian() == 11586
    assert a.readInt() == 1313425221
    assert a.readIntBigEndian() == 542266177
    assert a.readInt64() == 4995689725513846816
    assert a.readInt64BigEndian() == 2327029881779334445
    # assert a.readCompressedInt() == 1
    assert a.readFloat() == pytest.approx(839842.8125)
    assert a.readFloatBigEndian() == pytest.approx(800132.0625)
    assert a.readDouble() == pytest.approx(1.4120143627839163e+84)
    assert a.readDoubleBigEndian() == pytest.approx(3.889046183921725e+16)
    a.readNextLine()
    assert a.readNextLine() == 'NiwIXNWyi6EJ260r0legE/V/dFQC4kWwzC1VPHsrjbulfr2n+Ez7MrfZwlTwPb0B'

#==================================================================================================

def test_file_input_stream_string():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    result = a.readString()

    with open(a.getFile().getFullPathName(), "r") as f:
        assert f.read() == result

#==================================================================================================

def test_file_input_stream_read_entire_as_string():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    result = a.readEntireStreamAsString()

    with open(a.getFile().getFullPathName(), "r") as f:
        assert f.read() == result

#==================================================================================================

def test_file_input_stream_read_into_memory_block():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    mb = juce.MemoryBlock()

    a.readIntoMemoryBlock(mb)

    with open(a.getFile().getFullPathName(), "r") as f:
        assert f.read() == mb.toString()
