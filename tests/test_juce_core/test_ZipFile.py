import pytest
import os
import sys

import popsicle as juce

from ..utilities import get_runtime_data_folder


#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))

@pytest.fixture
def zip_file():
    yield juce.File(this_file.getSiblingFile("data").getChildFile("archive.zip"))

@pytest.fixture
def text_file():
    yield juce.File(this_file.getSiblingFile("data").getChildFile("somefile.txt"))

@pytest.fixture
def image_file():
    yield juce.File(this_file.getSiblingFile("data").getChildFile("pysound.jpg"))

#==================================================================================================

def test_constructor_with_file(zip_file):
    z = juce.ZipFile(zip_file)
    assert z.getNumEntries() == 2

#==================================================================================================

def test_constructor_with_nonexisting_file():
    z = juce.ZipFile(this_file.getChildFile("nonexisting.zip"))
    assert z.getNumEntries() == 0

#==================================================================================================

def test_constructor_with_input_stream(zip_file):
    with open(zip_file.getFullPathName(), "rb") as file:
        mb = juce.MemoryBlock(file.read())
        input_stream = juce.MemoryInputStream(mb, False)
        zip_file = juce.ZipFile(input_stream)
        assert zip_file.getNumEntries() == 2

#==================================================================================================

def test_constructor_with_input_source(zip_file):
    input_source = juce.FileInputSource(zip_file, False)
    zip_file = juce.ZipFile(input_source)
    assert zip_file.getNumEntries() == 2

#==================================================================================================

def test_get_entry_by_index(zip_file):
    zip_file = juce.ZipFile(zip_file)

    entry = zip_file.getEntry(0)
    assert entry.filename == "somefile.txt"
    assert entry.isSymbolicLink == False
    assert entry.externalFileAttributes == 2175008768
    assert entry.uncompressedSize == (900 if sys.platform == "win32" else 886)
    assert entry.fileTime == juce.Time.fromISO8601("20240209T183446.000+0100")

    entry = zip_file.getEntry(1)
    assert entry.filename == "pysound.jpg"
    assert entry.isSymbolicLink == False
    assert entry.externalFileAttributes == 2175008768
    assert entry.uncompressedSize == (172506 if sys.platform == "win32" else 172506)
    assert entry.fileTime == juce.Time.fromISO8601("20240215T083304.000+0100")

#==================================================================================================

def test_get_index_of_file_name(zip_file):
    zip_file = juce.ZipFile(zip_file)
    index = zip_file.getIndexOfFileName("test_file.txt")
    assert index == -1
    index = zip_file.getIndexOfFileName("somefile.txt")
    assert index == 0
    index = zip_file.getIndexOfFileName("SOMEFILE.txt")
    assert index == -1
    index = zip_file.getIndexOfFileName("SOMEFILE.txt", True)
    assert index == 0

#==================================================================================================

def test_get_entry_by_file_name(zip_file):
    zip_file = juce.ZipFile(zip_file)
    entry = zip_file.getEntry("somefile.txt")
    assert entry.filename == "somefile.txt"
    assert entry.isSymbolicLink == False
    assert entry.externalFileAttributes == 2175008768
    assert entry.uncompressedSize == (900 if sys.platform == "win32" else 886)
    assert entry.fileTime == juce.Time.fromISO8601("20240209T183446.000+0100")

#==================================================================================================

def test_sort_entries_by_filename(zip_file):
    zip_file = juce.ZipFile(zip_file)
    assert zip_file.getEntry(0).filename == "somefile.txt"
    assert zip_file.getEntry(1).filename == "pysound.jpg"

    zip_file.sortEntriesByFilename()
    assert zip_file.getEntry(0).filename == "pysound.jpg"
    assert zip_file.getEntry(1).filename == "somefile.txt"

#==================================================================================================

def test_create_stream_for_entry_by_index(zip_file):
    zip_file = juce.ZipFile(zip_file)
    stream = zip_file.createStreamForEntry(0)
    assert stream is not None

#==================================================================================================

def test_create_stream_for_entry_by_entry(zip_file, text_file):
    zip_file = juce.ZipFile(zip_file)
    entry = zip_file.getEntry(0)
    stream = zip_file.createStreamForEntry(entry)
    assert stream is not None
    assert stream.readEntireStreamAsString() == juce.File(text_file).loadFileAsString()

#==================================================================================================

def test_uncompress_to(zip_file, text_file, image_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_to")
    target_dir.createDirectory()

    target_file1 = target_dir.getChildFile("somefile.txt")
    target_file2 = target_dir.getChildFile("pysound.jpg")

    z = juce.ZipFile(zip_file)

    result = z.uncompressTo(target_dir)
    assert result.wasOk()
    assert target_file1.existsAsFile()
    assert target_file2.existsAsFile()

    target_file1.replaceWithText("1234567890")
    target_file2.replaceWithData(b"\x00\x00")

    result = z.uncompressTo(target_dir, True)
    assert result.wasOk()
    assert target_file1.loadFileAsString() == text_file.loadFileAsString()

    mb1 = juce.MemoryBlock()
    mb2 = juce.MemoryBlock()
    assert target_file2.loadFileAsData(mb1)
    assert image_file.loadFileAsData(mb2)
    assert mb1 == mb2

#==================================================================================================

def test_uncompress_to_not_overwrite(zip_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_to_not_overwrite")
    target_dir.createDirectory()

    target_file1 = target_dir.getChildFile("somefile.txt")
    target_file2 = target_dir.getChildFile("pysound.jpg")

    z = juce.ZipFile(zip_file)

    result = z.uncompressTo(target_dir)
    assert result.wasOk()
    assert target_file1.existsAsFile()
    assert target_file2.existsAsFile()

    target_file1.replaceWithText("1234567890")
    target_file2.replaceWithData(b"\x00\x00")

    result = z.uncompressTo(target_dir, False)
    assert result.wasOk()

    assert target_file1.loadFileAsString() == "1234567890"

    mb = juce.MemoryBlock()
    assert target_file2.loadFileAsData(mb)
    assert mb.getData() == b"\x00\x00"

#==================================================================================================

def test_uncompress_to_nonexistent(zip_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_to_nonexistent")

    result = juce.ZipFile(zip_file).uncompressTo(target_dir)
    assert result.wasOk()
    assert target_dir.isDirectory()
    assert target_dir.getChildFile("somefile.txt").existsAsFile()
    assert target_dir.getChildFile("pysound.jpg").existsAsFile()

#==================================================================================================

def test_uncompress_entry(zip_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_entry")
    target_dir.createDirectory()

    result = juce.ZipFile(zip_file).uncompressEntry(0, target_dir)
    assert result.wasOk()
    assert target_dir.getChildFile("somefile.txt").existsAsFile()
    assert not target_dir.getChildFile("pysound.jpg").existsAsFile()

#==================================================================================================

def test_uncompress_entry_with_overwrite_bool_false(zip_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_entry_with_overwrite_bool_false")
    target_dir.createDirectory()

    target_file = target_dir.getChildFile("somefile.txt")

    z = juce.ZipFile(zip_file)
    result = z.uncompressEntry(0, target_dir, shouldOverwriteFiles=True)
    assert result.wasOk()
    assert target_file.existsAsFile()

    target_file.replaceWithText("1234567890")

    result = z.uncompressEntry(0, target_dir, shouldOverwriteFiles=False)
    assert result.wasOk()
    assert target_file.loadFileAsString() == "1234567890"

#==================================================================================================

def test_uncompress_entry_with_overwrite_bool_true(zip_file, text_file):
    target_dir = get_runtime_data_folder().getChildFile("test_uncompress_entry_with_overwrite_bool_true")
    target_dir.createDirectory()

    target_file = target_dir.getChildFile("somefile.txt")

    z = juce.ZipFile(zip_file)
    result = z.uncompressEntry(0, target_dir, shouldOverwriteFiles=False)
    assert result.wasOk()
    assert target_file.existsAsFile()

    target_file.replaceWithText("1234567890")

    result = z.uncompressEntry(0, target_dir, shouldOverwriteFiles=True)
    assert result.wasOk()
    assert target_file.loadFileAsString() == text_file.loadFileAsString()

#==================================================================================================

def test_builder_add_file(text_file):
    output = get_runtime_data_folder().getChildFile("test_builder_add_file.zip")

    for level in range(0, 9):
        builder = juce.ZipFile.Builder()
        builder.addFile(text_file, level)
        assert builder.writeToStream(output.createOutputStream())
        assert output.existsAsFile()

        z = juce.ZipFile(output)
        assert z.getNumEntries() == 1
        assert z.getEntry(0).filename == text_file.getFileName()

        output.deleteFile()

#==================================================================================================

def test_builder_add_stream_entry(text_file):
    output = get_runtime_data_folder().getChildFile("test_builder_add_stream_entry.zip")

    for level in range(0, 9):
        builder = juce.ZipFile.Builder()
        input_stream = juce.FileInputStream(text_file)
        builder.addEntry(input_stream, level, text_file.getFileName(), juce.Time())
        assert builder.writeToStream(output.createOutputStream())
        assert output.existsAsFile()

        z = juce.ZipFile(output)
        assert z.getNumEntries() == 1
        assert z.getEntry(0).filename == text_file.getFileName()

        output.deleteFile()
