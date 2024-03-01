import pytest
import os

from ..utilities import get_runtime_data_file
import popsicle as juce

#==================================================================================================

def test_create_temporary_file():
    temp_file = juce.TemporaryFile(".txt")
    assert not temp_file.getFile().existsAsFile()
    assert not temp_file.getTargetFile().existsAsFile()

#==================================================================================================

def test_write_to_temporary_file():
    temp_file = juce.TemporaryFile(".txt")

    temp_file.getFile().appendText("Hello, World!", False)
    assert temp_file.getFile().existsAsFile()

    with open(temp_file.getFile().getFullPathName(), "r") as file:
        assert file.read() == "Hello, World!"

#==================================================================================================

def test_delete_temporary_file():
    temp_file = juce.TemporaryFile(".txt")

    temp_file.getFile().appendText("Hello, World!", False)
    assert temp_file.getFile().existsAsFile()
    assert not temp_file.getTargetFile().existsAsFile()

    assert temp_file.deleteTemporaryFile()
    assert not temp_file.getFile().existsAsFile()
    assert not temp_file.getTargetFile().existsAsFile()

#==================================================================================================

def test_replace_target_file_with_temporary():
    original_file_path = get_runtime_data_file("test_replace_target_file_with_temporary.txt")
    with open(original_file_path.getFullPathName(), "w") as file:
        file.write("Old Content")

    temp_file = juce.TemporaryFile(original_file_path)

    temp_file.getFile().appendText("New Content", False)

    with open(original_file_path.getFullPathName(), "r") as file:
        assert file.read() == "Old Content"

    assert temp_file.overwriteTargetFileWithTemporary()

    with open(original_file_path.getFullPathName(), "r") as file:
        assert file.read() == "New Content"

#==================================================================================================

def test_replace_target_file_with_temporary_with():
    original_file_path = get_runtime_data_file("test_replace_target_file_with_temporary.txt")
    with open(original_file_path.getFullPathName(), "w") as file:
        file.write("Old Content")

    with juce.TemporaryFile(original_file_path) as temp_file:
        temp_file.getFile().appendText("New Content", False)

        with open(original_file_path.getFullPathName(), "r") as file:
            assert file.read() == "Old Content"

    with open(original_file_path.getFullPathName(), "r") as file:
        assert file.read() == "New Content"

#==================================================================================================

def test_temporary_file_deletion_on_destruction():
    temp_file = juce.TemporaryFile(".txt")
    file_path = temp_file.getFile().getFullPathName()
    del temp_file
    assert not os.path.exists(file_path)

#==================================================================================================

def test_fail_to_replace_nonexistent_target():
    non_existent_path = get_runtime_data_file("test_fail_to_replace_nonexistent_target_src.txt")
    destination = get_runtime_data_file("test_fail_to_replace_nonexistent_target_dst.txt")

    temp_file = juce.TemporaryFile(destination, non_existent_path)
    temp_file.getFile().appendText("Trying to replace a non-existent file", False)

    assert temp_file.overwriteTargetFileWithTemporary()
