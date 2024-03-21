import os
import textwrap
import pytest

from ..utilities import get_runtime_data_file
import popsicle as juce


#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))
data_folder = this_file.getSiblingFile("data")

#==================================================================================================

def test_file_output_stream_write_from_input_all():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    assert a.openedOk()

    b = juce.FileOutputStream(get_runtime_data_file("test_file_output_stream_write_from_input_all.txt"))
    assert b.openedOk()

    assert b.writeFromInputStream(a, -1) == 886
    b.flush()

    with open(a.getFile().getFullPathName(), "r") as f1, open(b.getFile().getFullPathName(), "r") as f2:
        assert f1.read() == f2.read()

#==================================================================================================

def test_file_output_stream_write_from_input_some():
    a = juce.FileInputStream(data_folder.getChildFile("somefile.txt"))
    assert a.openedOk()

    b = juce.FileOutputStream(get_runtime_data_file("test_file_input_stream_nonexisting.txt"))
    assert b.openedOk()

    assert b.writeFromInputStream(a, 100) == 100
    b.flush()

    with open(a.getFile().getFullPathName(), "r") as f1, open(b.getFile().getFullPathName(), "r") as f2:
        assert f1.read(100) == f2.read()

#==================================================================================================

def test_file_output_stream_write():
    b = juce.FileOutputStream(get_runtime_data_file("test_file_output_stream_write.txt"))
    assert b.writeBool(True)
    assert b.writeByte('-')
    assert b.writeShort(11565)
    assert b.writeShortBigEndian(11586)
    assert b.writeInt(1313425221)
    assert b.writeIntBigEndian(542266177)
    assert b.writeInt64(4995689725513846816)
    assert b.writeInt64BigEndian(2327029881779334445)
    assert b.writeCompressedInt(117)
    assert b.writeFloat(839842.8125)
    assert b.writeFloatBigEndian(800132.0625)
    assert b.writeDouble(1.4120143627839163e+84)
    assert b.writeDoubleBigEndian(3.889046183921725e+16)
    assert b.writeString(textwrap.dedent("""
        first line
        NiwIXNWyi6EJ260r0legE/V/dFQC4kWwzC1VPHsrjbulfr2n+Ez7MrfZwlTwPb0B
        last line"""))
    b.flush()

    a = juce.FileInputStream(get_runtime_data_file("test_file_output_stream_write.txt"))
    assert a.readBool() != False
    assert a.readByte() == '-'
    assert a.readShort() == 11565
    assert a.readShortBigEndian() == 11586
    assert a.readInt() == 1313425221
    assert a.readIntBigEndian() == 542266177
    assert a.readInt64() == 4995689725513846816
    assert a.readInt64BigEndian() == 2327029881779334445
    assert a.readCompressedInt() == 117
    assert a.readFloat() == pytest.approx(839842.8125)
    assert a.readFloatBigEndian() == pytest.approx(800132.0625)
    assert a.readDouble() == pytest.approx(1.4120143627839163e+84)
    assert a.readDoubleBigEndian() == pytest.approx(3.889046183921725e+16)
    a.readNextLine()
    a.readNextLine()
    assert a.readNextLine() == 'NiwIXNWyi6EJ260r0legE/V/dFQC4kWwzC1VPHsrjbulfr2n+Ez7MrfZwlTwPb0B'
