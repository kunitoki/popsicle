import os

import popsicle as juce


#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))
data_folder = this_file.getSiblingFile("data")

#==================================================================================================

def test_file_input_stream_nonexisting():
    a = juce.FileInputStream(data_folder.getChildFile("nonexisting.txt"))
    assert a.failedToOpen()
    assert a.getFile() == data_folder.getChildFile("nonexisting.txt")
    assert a.getStatus().failed()

#==================================================================================================

def test_file_input_stream():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
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
