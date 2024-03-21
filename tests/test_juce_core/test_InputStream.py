import pytest
import struct
import math

import popsicle as juce

#==================================================================================================

class CustomFullInputStream(juce.InputStream):
    data: bytes = bytes()
    offset: int = 0

    def __init__(self, data):
        juce.InputStream.__init__(self)
        self.data = data
        self.offset = 0

    def getTotalLength(self):
        return len(self.data)

    def isExhausted(self):
        return self.offset >= len(self.data)

    def read(self, destBuffer):
        maxSize = min(len(destBuffer), len(self.data) - self.offset)
        if maxSize > 0:
            destBuffer[:] = self.data[self.offset:self.offset+maxSize]
            self.offset += maxSize
        return maxSize

    def readByte(self):
        value = self.data[self.offset]
        self.offset += 1
        return chr(value)

    def readShort(self):
        value = struct.unpack("<h", self.data[self.offset:self.offset + 2])
        self.offset += 2
        return value[0]

    def readShortBigEndian(self):
        value = struct.unpack(">h", self.data[self.offset:self.offset + 2])
        self.offset += 2
        return value[0]

    def readInt(self):
        value = struct.unpack("<i", self.data[self.offset:self.offset + 4])
        self.offset += 4
        return value[0]

    def readIntBigEndian(self):
        value = struct.unpack(">i", self.data[self.offset:self.offset + 4])
        self.offset += 4
        return value[0]

    def readInt64(self):
        value = struct.unpack("<q", self.data[self.offset:self.offset + 8])
        self.offset += 8
        return value[0]

    def readInt64BigEndian(self):
        value = struct.unpack(">q", self.data[self.offset:self.offset + 8])
        self.offset += 8
        return value[0]

    def readFloat(self):
        value = struct.unpack("<f", self.data[self.offset:self.offset + 4])
        self.offset += 4
        return value[0]

    def readFloatBigEndian(self):
        value = struct.unpack(">f", self.data[self.offset:self.offset + 4])
        self.offset += 4
        return value[0]

    def readDouble(self):
        value = struct.unpack("<d", self.data[self.offset:self.offset + 8])
        self.offset += 8
        return value[0]

    def readDoubleBigEndian(self):
        value = struct.unpack(">d", self.data[self.offset:self.offset + 8])
        self.offset += 8
        return value[0]

    def readCompressedInt(self):
        sizeByte = ord(self.readByte())
        if sizeByte == 0:
            return 0

        numBytes = sizeByte & 0x7f
        if numBytes > 4:
            return 0 # trying to read corrupt data

        b = bytearray(numBytes)
        if self.read(b) != numBytes:
            return 0

        while len(b) < 4:
            b.append(0)

        num = juce.ByteOrder.littleEndianInt(b)
        return -num if (sizeByte >> 7) else num

    def readNextLine(self):
        data = bytearray()

        while self.offset < len(self.data):
            value = self.data[self.offset]
            self.offset += 1
            data.append(value)

            value_bytes = value.to_bytes(length=1, byteorder="little")
            if value_bytes == b"\n" or value_bytes == b"\0":
                data = data[:-1]
                break

        return data.decode("utf-8")

    def readString(self):
        data = bytearray()

        while self.offset < len(self.data):
            value = self.data[self.offset]
            self.offset += 1
            data.append(value)

            value_bytes = value.to_bytes(length=1, byteorder="little")
            if value_bytes == b"\0":
                data = data[:-1]
                break

        return data.decode("utf-8")

    def readEntireStreamAsString(self):
        if not self.data or self.isExhausted():
            return ""

        return self.data[self.offset:].decode("utf-8")

    def readIntoMemoryBlock(self, destBlock, maxNumBytesToRead):
        if maxNumBytesToRead >= 0:
            maxSize = min(maxNumBytesToRead, len(self.data) - self.offset)
        else:
            maxSize = len(self.data) - self.offset

        if maxSize > 0:
            destBlock.append(self.data[self.offset:maxSize])
            self.offset += maxSize

        return maxSize

    def getPosition(self):
        return self.offset

    def setPosition(self, newPosition):
        self.offset = newPosition

    def skipNextBytes (self, newPosition):
        self.offset += newPosition

#==================================================================================================

class CustomMinimalInputStream(juce.InputStream):
    data: bytes = bytes()
    offset: int = 0

    def __init__(self, data):
        juce.InputStream.__init__(self)
        self.data = data
        self.offset = 0

    def getTotalLength(self):
        return len(self.data)

    def isExhausted(self):
        return self.offset >= len(self.data)

    def read(self, destBuffer):
        maxSize = min(len(destBuffer), len(self.data) - self.offset)
        if maxSize > 0:
            destBuffer[:] = self.data[self.offset:self.offset+maxSize]
            self.offset += maxSize
        return maxSize

    def getPosition(self):
        return self.offset

    def setPosition(self, newPosition):
        self.offset = newPosition

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_input_stream(input_stream):
    i = input_stream(b"custom string")
    assert i.getTotalLength() == 13
    assert i.getPosition() == 0
    assert i.isExhausted() == False

    i.skipNextBytes(i.getTotalLength())
    assert i.getPosition() == i.getTotalLength()
    assert i.isExhausted() == True

    i.setPosition(0)
    assert i.getPosition() == 0
    assert i.isExhausted() == False

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_entire_stream_as_string(input_stream):
    i = input_stream(b"custom string")

    text = i.readEntireStreamAsString()
    assert text == "custom string"

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_string(input_stream):
    i = input_stream(b"custom\0string\0")

    text = i.readString()
    assert text == "custom"

    text = i.readString()
    assert text == "string"

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_new_line(input_stream):
    i = input_stream(b"custom\nstring\n")

    text = i.readNextLine()
    assert text == "custom"

    text = i.readNextLine()
    assert text == "string"

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_into_memory_block(input_stream):
    i = input_stream(b"the\ncustom\nstring\nline")

    mb = juce.MemoryBlock()
    i.setPosition(0)
    readBytes = i.readIntoMemoryBlock(mb, -1)
    assert readBytes == len(i.data)
    assert mb.toString() == "the\ncustom\nstring\nline"

    mb = juce.MemoryBlock()
    i.setPosition(0)
    readBytes = i.readIntoMemoryBlock(mb, 0)
    assert readBytes == 0
    assert mb.toString() == ""

    mb = juce.MemoryBlock()
    i.setPosition(0)
    readBytes = i.readIntoMemoryBlock(mb, 3)
    assert readBytes == 3
    assert mb.toString() == "the"

    mb = juce.MemoryBlock()
    i.setPosition(0)
    readBytes = i.readIntoMemoryBlock(mb, 100)
    assert readBytes == len(i.data)
    assert mb.toString() == "the\ncustom\nstring\nline"

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_byte(input_stream):
    i = input_stream(b"a")
    b = i.readByte()
    assert b == "a"

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_short(input_stream):
    i = input_stream(b"\xff\x7f")
    b = i.readShort()
    assert b == 32767

    i = input_stream(b"\x7f\xff")
    b = i.readShortBigEndian()
    assert b == 32767

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_int(input_stream):
    i = input_stream(b"\xff\xff\xff\x7f")
    b = i.readInt()
    assert b == 2147483647

    i = input_stream(b"\x7f\xff\xff\xff")
    b = i.readIntBigEndian()
    assert b == 2147483647

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_int64(input_stream):
    i = input_stream(b"\xff\xff\xff\xff\xff\xff\xff\x7f")
    b = i.readInt64()
    assert b == 9223372036854775807

    i = input_stream(b"\x7f\xff\xff\xff\xff\xff\xff\xff")
    b = i.readInt64BigEndian()
    assert b == 9223372036854775807

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_float(input_stream):
    i = input_stream(struct.pack('<f', math.pi))
    b = i.readFloat()
    assert b == pytest.approx(math.pi)

    i = input_stream(struct.pack('>f', math.pi))
    b = i.readFloatBigEndian()
    assert b == pytest.approx(math.pi)

#==================================================================================================

@pytest.mark.parametrize("input_stream", [CustomFullInputStream, CustomMinimalInputStream])
def test_read_double(input_stream):
    i = input_stream(struct.pack('<d', math.pi))
    b = i.readDouble()
    assert b == pytest.approx(math.pi)

    i = input_stream(struct.pack('>d', math.pi))
    b = i.readDoubleBigEndian()
    assert b == pytest.approx(math.pi)
