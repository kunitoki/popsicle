import pytest

import popsicle as juce

#==================================================================================================

def test_read_within_subregion():
    # Setup: Create a memory stream and wrap it in a SubregionStream
    source_data = b'This is a test stream'
    source_stream = juce.MemoryInputStream(source_data, False)
    subregion_stream = juce.SubregionStream(source_stream, 5, 9, False)

    # Act: Read from the subregion stream
    buffer = bytearray(9)
    num_read = subregion_stream.read(buffer)

    # Assert: Correct data read, and correct number of bytes read
    assert num_read == 9
    assert buffer[:num_read] == b'is a test'

#==================================================================================================

def test_read_past_subregion_end():
    # Setup: similar to above
    source_data = b'This is a test stream'
    source_stream = juce.MemoryInputStream(source_data, False)
    subregion_stream = juce.SubregionStream(source_stream, 5, 9, False)

    # Act: Attempt to read more data than the subregion contains
    buffer = bytearray(15)  # larger than subregion size
    num_read = subregion_stream.read(buffer)

    # Assert: Only reads up to the end of the subregion
    assert num_read == 9  # since the subregion is only 9 bytes long
    assert buffer[:num_read] == b'is a test'

#==================================================================================================

def test_seek_within_subregion():
    # Setup: similar to above
    source_data = b'This is a test stream'
    source_stream = juce.MemoryInputStream(source_data, False)
    subregion_stream = juce.SubregionStream(source_stream, 5, 15, False)

    # Act: Seek to a position within the subregion and read
    subregion_stream.setPosition(3)  # Seek to 'a test stream'
    buffer = bytearray(4)
    num_read = subregion_stream.read(buffer)

    # Assert: Correct position and data read
    assert num_read == 4
    assert buffer[:num_read] == b'a te'

#==================================================================================================

def test_seek_past_subregion_end():
    # Setup: similar to above
    source_data = b'This is a test stream'
    source_stream = juce.MemoryInputStream(source_data, False)
    subregion_stream = juce.SubregionStream(source_stream, 5, 11, False)

    # Act: Seek past the end of the subregion
    seek_result = subregion_stream.setPosition(100)  # Far past the subregion

    assert seek_result # Assuming setPosition returns success status
    assert subregion_stream.getPosition() == 16
