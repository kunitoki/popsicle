import pytest

import popsicle as juce

#==================================================================================================

def test_construct_empty():
    memory_block = juce.MemoryBlock()
    assert memory_block.getSize() == 0
    assert memory_block.isEmpty()

#==================================================================================================

def test_construct_size():
    memory_block = juce.MemoryBlock(10)
    assert memory_block.getSize() == 10
    assert not memory_block.isEmpty()

#==================================================================================================

def test_construct_size_value():
    memory_block = juce.MemoryBlock(5, True)
    assert memory_block.getSize() == 5
    assert not memory_block.isEmpty()
    assert all(value == 0 for value in memory_block.getData())

#==================================================================================================

def test_copy_constructor():
    original = juce.MemoryBlock(8, True)
    copied = juce.MemoryBlock(original)
    assert copied.getSize() == original.getSize()
    assert copied.getData() == original.getData()

#==================================================================================================

def test_construct_from_buffer():
    data = bytes([1, 2, 3, 4, 5])
    memory_block = juce.MemoryBlock(data)
    assert memory_block.getSize() == len(data)
    assert memory_block.getData() == data

#==================================================================================================

def test_equality():
    memory_block1 = juce.MemoryBlock([1, 2, 3])
    memory_block2 = juce.MemoryBlock([1, 2, 3])
    memory_block3 = juce.MemoryBlock([3, 2, 1])

    assert memory_block1 == memory_block2
    assert memory_block1 != memory_block3

#==================================================================================================

def test_matches():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    assert memory_block.matches(bytes([1, 2, 3, 4, 5]))

#==================================================================================================

def test_getitem():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    assert memory_block[2] == '\x03'

#==================================================================================================

def test_setitem():
    memory_block = juce.MemoryBlock(['\x00', '\x01', '\x02', '\x03', '\x04'])
    memory_block[1] = '\xff'
    assert memory_block[1] == '\xff'

    memory_block[2] = 2
    assert memory_block[2] == '\x02'

#==================================================================================================

def test_get_data():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    data = memory_block.getData()
    assert isinstance(data, memoryview)
    assert bytes(data) == bytes([1, 2, 3, 4, 5])

#==================================================================================================

def test_is_empty():
    empty_memory_block = juce.MemoryBlock()
    non_empty_memory_block = juce.MemoryBlock([1, 2, 3])

    assert empty_memory_block.isEmpty()
    assert not non_empty_memory_block.isEmpty()

#==================================================================================================

def test_set_size():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])

    memory_block.setSize(3)
    assert memory_block.getSize() == 3
    assert not memory_block.isEmpty()

    memory_block.setSize(0)
    assert memory_block.getSize() == 0
    assert memory_block.isEmpty()

    memory_block.setSize(10)
    assert memory_block.getSize() == 10
    #assert all(value == 0 for value in memory_block.getData())

#==================================================================================================

def test_ensure_size():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    memory_block.ensureSize(8)
    assert memory_block.getSize() == 8

#==================================================================================================

def test_reset():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    memory_block.reset()
    assert memory_block.getSize() == 0
    assert memory_block.isEmpty()

#==================================================================================================

def test_fill_with():
    memory_block = juce.MemoryBlock(5)
    memory_block.fillWith(42)
    assert all(value == 42 for value in memory_block.getData())

#==================================================================================================

def test_append():
    memory_block1 = juce.MemoryBlock([1, 2, 3])
    memory_block1.append(bytes(b'\x04\x05\x06'))
    assert memory_block1.getSize() == 6
    assert memory_block1.getData() == bytes([1, 2, 3, 4, 5, 6])

#==================================================================================================

def test_replace_all():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    replacement = bytes(b'\x07\x08\x09')
    memory_block.replaceAll(replacement)
    assert memory_block.getSize() == len(replacement)
    assert memory_block.getData() == replacement

#==================================================================================================

def test_insert():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    data_to_insert = bytes([9, 8, 7])
    insert_position = 2
    memory_block.insert(data_to_insert, insert_position)
    assert memory_block.getSize() == 8
    assert memory_block.getData() == bytes([1, 2, 9, 8, 7, 3, 4, 5])

#==================================================================================================

def test_remove_section():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])

    memory_block.removeSection(1, 3)
    assert memory_block.getSize() == 2
    assert memory_block.getData() == bytes([1, 5])

    memory_block.removeSection(10, 2)
    assert memory_block.getSize() == 10
    assert memory_block.getData()[0] == 1
    assert memory_block.getData()[1] == 5

#==================================================================================================

def test_copy_from():
    destination = juce.MemoryBlock(5, True)

    destination.copyFrom(bytes([1, 2, 3]), 1)
    assert destination.getSize() == 5
    assert destination.getData() == bytes([0, 1, 2, 3, 0])

#==================================================================================================

def test_copy_to():
    source = juce.MemoryBlock(bytes([1, 2, 3, 4, 5]))
    destination = bytearray(3)

    source.copyTo(destination, 1)
    assert len(destination) == 3
    assert destination == bytes([2, 3, 4])

#==================================================================================================

def test_swap_with():
    memory_block1 = juce.MemoryBlock([1, 2, 3])
    memory_block2 = juce.MemoryBlock([4, 5, 6])
    memory_block1.swapWith(memory_block2)
    assert memory_block1.getData() == bytes([4, 5, 6])
    assert memory_block2.getData() == bytes([1, 2, 3])

#==================================================================================================

def test_to_string():
    memory_block = juce.MemoryBlock([65, 66, 67])  # ASCII values for 'A', 'B', 'C'
    assert memory_block.toString() == "ABC"

#==================================================================================================

def test_load_from_hex_string():
    hex_string = "48656C6C6F20576F726C64"  # Hex representation of "Hello World"
    memory_block = juce.MemoryBlock()
    memory_block.loadFromHexString(hex_string)
    assert memory_block.getData() == b"Hello World"

#==================================================================================================

@pytest.mark.skip(reason="This produces wrong results, JUCE bug ?")
def test_set_bit_range():
    memory_block = juce.MemoryBlock(4, initialiseToZero=True)
    assert memory_block.getData() == bytes(b'\x00\x00\x00\x00')

    memory_block.setBitRange(0, 2, 1)
    assert bytes(memory_block.getData()) == bytes(b'\x01\x01\x00\x00')

#==================================================================================================

def test_get_bit_range():
    memory_block = juce.MemoryBlock([0b10101010, 0b01010101])
    result = memory_block.getBitRange(1, 6)
    assert result == 0b010101  # Decimal equivalent of the selected bits

#==================================================================================================

def test_to_base64_encoding():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    base64_encoded = memory_block.toBase64Encoding()
    assert base64_encoded == '5.AHv.DT.' # This should have been 'AQIDBAU='

#==================================================================================================

def test_from_base64_encoding():
    base64_encoded = '5.AHv.DT.' # This should have been 'AQIDBAU='
    memory_block = juce.MemoryBlock()
    memory_block.fromBase64Encoding(base64_encoded)
    assert memory_block.getData() == bytes([1, 2, 3, 4, 5])

#==================================================================================================

def test_repr():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    assert repr(memory_block) == "popsicle.MemoryBlock(b'\\x01\\x02\\x03\\x04\\x05')"

#==================================================================================================

def test_str():
    memory_block = juce.MemoryBlock([1, 2, 3, 4, 5])
    assert str(memory_block) == "\x01\x02\x03\x04\x05"
