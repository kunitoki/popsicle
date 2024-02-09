import os
import sys
import pytest

from ..utilities import get_runtime_data_file
import popsicle as juce

this_file = os.path.abspath(__file__)
this_folder = os.path.dirname(this_file)

nonexisting_file = "C:\\path\\to\\some\\file.txt" if sys.platform == "win32" else "/path/to/some/file.txt"
nonexisting_folder = os.path.dirname(nonexisting_file)

#==================================================================================================

def test_constructor():
    a = juce.File(this_file)
    assert a.getFullPathName() == this_file
    assert a.getFileName() == os.path.split(this_file)[1]

#==================================================================================================

def test_construct_empty():
    file = juce.File()
    assert not file.exists()

#==================================================================================================

def test_construct_with_path():
    file = juce.File(nonexisting_file)
    assert file.getFullPathName() == nonexisting_file

#==================================================================================================

def test_copy_constructor():
    original_file = juce.File("/original/file.txt")
    copied_file = juce.File(original_file)
    assert copied_file.getFullPathName() == original_file.getFullPathName()

#==================================================================================================

def test_exists():
    existing_file = juce.File(this_file)
    non_existing_file = juce.File(nonexisting_file)

    assert existing_file.exists()
    assert not non_existing_file.exists()

#==================================================================================================

def test_exists_as_file():
    existing_file = juce.File(this_file)
    existing_folder = juce.File(this_folder)
    non_existing_file = juce.File(nonexisting_file)
    assert existing_file.existsAsFile()
    assert not existing_folder.existsAsFile()
    assert not non_existing_file.existsAsFile()

#==================================================================================================

def test_is_directory():
    directory = juce.File(this_folder)
    nonexisting = juce.File(nonexisting_folder)
    file = juce.File(this_file)

    assert directory.isDirectory()
    assert not nonexisting.isDirectory()
    assert not file.isDirectory()

#==================================================================================================

def test_is_root():
    root_directory = juce.File("C:") if sys.platform == "win32" else juce.File("/")
    non_root_directory = juce.File("C:\\path\\to\\directory") if sys.platform == "win32" else juce.File("/path/to/directory")

    assert root_directory.isRoot()
    assert not non_root_directory.isRoot()

#==================================================================================================

def test_get_size():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert file.getSize() == (900 if sys.platform == "win32" else 886)

#==================================================================================================

def test_description_of_size_in_bytes():
    size_description = juce.File.descriptionOfSizeInBytes(1024)
    assert isinstance(size_description, str)

#==================================================================================================

def test_get_full_path_name():
    file = juce.File(this_file)
    assert file.getFullPathName() == this_file

#==================================================================================================

def test_get_file_name():
    file_path = os.path.join(this_folder, "test_file.txt")
    file_obj = juce.File(file_path)
    assert file_obj.getFileName() == "test_file.txt"

#==================================================================================================

def test_get_relative_path_from():
    base_directory = juce.File(this_folder)
    file_path = os.path.join(this_folder, "test_file.txt")
    file_obj = juce.File(file_path)
    relative_path = file_obj.getRelativePathFrom(base_directory)
    assert relative_path == "test_file.txt"

#==================================================================================================

def test_get_file_extension():
    file_path = os.path.join(this_folder, "test_file.txt")
    file_obj = juce.File(file_path)
    assert file_obj.getFileExtension() == ".txt"

#==================================================================================================

def test_has_file_extension():
    file = juce.File(os.path.join(this_folder, "test_file.txt"))
    assert file.hasFileExtension(".txt")

#==================================================================================================

def test_with_file_extension():
    file = juce.File(os.path.join(this_folder, "test_file.txt"))
    new_file = file.withFileExtension(".xml")
    assert new_file.getFileName() == "test_file.xml"

#==================================================================================================

def test_get_file_name_without_extension():
    file = juce.File(os.path.join(this_folder, "test_file.txt"))
    assert file.getFileNameWithoutExtension() == "test_file"

#==================================================================================================

def test_hash_code():
    file_instance = juce.File()
    assert isinstance(file_instance.hashCode(), int)

#==================================================================================================

def test_hash_code_64():
    file_instance = juce.File()
    assert isinstance(file_instance.hashCode64(), int)

#==================================================================================================

def test_get_child_file():
    file_instance = juce.File(this_folder)
    child_file = file_instance.getChildFile("child_file.txt")
    assert isinstance(child_file, juce.File)

#==================================================================================================

def test_get_sibling_file():
    file1 = juce.File(this_folder).getChildFile("file1")
    sibling_file = file1.getSiblingFile("sibling.txt")
    assert sibling_file.getFileName() == "sibling.txt"
    assert sibling_file.getParentDirectory().getFileName() == juce.File(this_folder).getFileName()

#==================================================================================================

def test_get_parent_directory():
    file1 = juce.File(this_folder).getChildFile("file1")
    parent_dir = file1.getParentDirectory()
    assert parent_dir.getFileName() == juce.File(this_folder).getFileName()

#==================================================================================================

def test_is_a_child_of():
    file1 = juce.File(this_folder).getChildFile("file1")
    parent_dir = juce.File(this_folder)
    assert file1.isAChildOf(parent_dir)

#==================================================================================================

def test_get_nonexistent_child_file():
    file1 = juce.File(this_folder).getChildFile("data")
    nonexistent_file = file1.getNonexistentChildFile("prefix", ".suffix")
    assert "prefix" in nonexistent_file.getFileName()
    assert ".suffix" in nonexistent_file.getFileExtension()

#==================================================================================================

def test_get_nonexistent_sibling():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    sibling = file.getNonexistentSibling()
    assert sibling.exists() is False

#==================================================================================================

def test_operators():
    file1 = juce.File(this_folder).getChildFile("file1.txt")
    file2 = juce.File(this_folder).getChildFile("file2.txt")
    assert file1 == file1
    assert file1 != file2
    assert file1 < file2
    assert file2 > file1

#==================================================================================================

@pytest.mark.skip(reason="Needs to be rewritten to be usable")
def test_set_execute_permission():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert file.setExecutePermission(True) is False

#==================================================================================================

def test_is_hidden():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert file.isHidden() is False

#==================================================================================================

def test_get_file_identifier():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    identifier = file.getFileIdentifier()
    assert isinstance(identifier, int)

#==================================================================================================

def test_get_last_access_time():
    file_instance = juce.File(get_runtime_data_file("test_get_last_access_time.txt"))
    file_instance.replaceWithText("abc")
    assert isinstance(file_instance.getLastAccessTime(), juce.Time)
    assert file_instance.getLastAccessTime() <= juce.Time.getCurrentTime()
    file_instance.deleteFile()

#==================================================================================================

def test_get_creation_time():
    file_instance = juce.File(get_runtime_data_file("test_get_creation_time.txt"))
    file_instance.replaceWithText("abc")
    assert isinstance(file_instance.getCreationTime(), juce.Time)
    assert file_instance.getCreationTime() <= juce.Time.getCurrentTime()
    file_instance.deleteFile()

#==================================================================================================

def test_get_last_modification_time():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    modification_time = file_instance.getLastModificationTime()
    assert isinstance(modification_time, juce.Time)
    assert modification_time <= juce.Time.getCurrentTime()

#==================================================================================================

def test_set_last_modification_time():
    file_instance = juce.File(get_runtime_data_file("test_set_last_modification_time.txt"))
    file_instance.replaceWithText("abc")
    result = file_instance.setLastModificationTime(juce.Time())
    # assert result == True
    # assert file_instance.getLastModificationTime() == juce.Time()
    file_instance.deleteFile()

#==================================================================================================

def test_get_special_location():
    special_location = juce.File.getSpecialLocation(juce.File.SpecialLocationType.userHomeDirectory)
    assert special_location.exists()

#==================================================================================================

def test_create_temp_file():
    temp_file = juce.File.createTempFile("test")
    assert temp_file.getFileName().endswith(".test")

#==================================================================================================

def test_get_current_working_directory():
    current_directory = juce.File.getCurrentWorkingDirectory()
    assert current_directory.exists()
    assert current_directory == juce.File(os.path.abspath(os.getcwd()))

#==================================================================================================

@pytest.mark.skip(reason="This will fail other tests")
def test_set_as_current_working_directory():
    temp_directory = juce.File.createTempFile("temp_directory")
    assert temp_directory.setAsCurrentWorkingDirectory()
    assert juce.File.getCurrentWorkingDirectory() == temp_directory

#==================================================================================================

def test_get_separator_char():
    separator_char = juce.File.getSeparatorChar()
    assert isinstance(separator_char, int)
    assert separator_char == (92 if sys.platform == "win32" else 47)

#==================================================================================================

def test_has_write_access():
    file_instance = juce.File(get_runtime_data_file("test_has_write_access.txt"))
    assert file_instance.hasWriteAccess() == True

#==================================================================================================

def test_has_read_access():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert file_instance.hasReadAccess() == True

#==================================================================================================

def test_set_read_only():
    file_instance = juce.File(get_runtime_data_file("test_set_read_only.txt"))
    file_instance.replaceWithText("abc")
    assert file_instance.setReadOnly(True) == True
    # assert not file_instance.hasReadAccess()
    assert file_instance.setReadOnly(False) == True
    assert file_instance.hasReadAccess()
    file_instance.deleteFile()

#==================================================================================================

def test_is_absolute_path():
    assert juce.File.isAbsolutePath("C:\\absolute\\path" if sys.platform == "win32" else "/absolute/path")
    assert not juce.File.isAbsolutePath("relative/path" if sys.platform == "win32" else "absolute/path")
    assert not juce.File.isAbsolutePath("relative\\path" if sys.platform == "win32" else "absolute\\path")

#==================================================================================================

def test_get_version():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert isinstance(file_instance.getVersion(), str)

#==================================================================================================

def test_create():
    file_instance = juce.File(get_runtime_data_file("test_create.txt"))
    result = file_instance.create()
    assert result.wasOk()
    assert file_instance.deleteFile()

#==================================================================================================

def test_create_directory():
    file_instance = juce.File(get_runtime_data_file("test_create_directory"))
    result = file_instance.createDirectory()
    assert result.wasOk()
    assert file_instance.deleteFile()

#==================================================================================================

def test_delete_file():
    file_instance = juce.File(get_runtime_data_file("test_delete_file.txt"))
    assert file_instance.create().wasOk()
    assert file_instance.deleteFile()

#==================================================================================================

def test_delete_recursively():
    folder_instance = juce.File(get_runtime_data_file("test_delete_recursively"))
    folder_instance.createDirectory()

    child_instance = folder_instance.getChildFile("test_delete_recursively_file.txt")
    child_instance.create()

    result = folder_instance.deleteRecursively()
    assert result == True

#==================================================================================================

@pytest.mark.skip(reason="This will fail other tests")
def test_move_to_trash():
    file_instance = juce.File(get_runtime_data_file("test_move_to_trash.txt"))
    file_instance.create()
    assert isinstance(file_instance.moveToTrash(), bool)

#==================================================================================================

def test_move_file_to():
    folder1 = juce.File(get_runtime_data_file("test_move_file_to_1"))
    folder1.createDirectory()

    folder2 = juce.File(get_runtime_data_file("test_move_file_to_2"))
    folder2.createDirectory()

    child_instance = folder1.getChildFile("test_move_file_to.txt")
    child_instance.create()
    assert child_instance.moveFileTo(folder2)
    assert child_instance.getParentDirectory() == folder1
    assert not child_instance.existsAsFile()
    assert not folder2.getChildFile("test_move_file_to.txt").existsAsFile()

    folder1.deleteRecursively()
    folder2.deleteRecursively()

#==================================================================================================

def test_copy_file_to():
    folder1 = juce.File(get_runtime_data_file("test_copy_file_to_1"))
    folder1.createDirectory()

    folder2 = juce.File(get_runtime_data_file("test_copy_file_to_2"))
    folder2.createDirectory()

    child_instance = folder1.getChildFile("test_copy_file_to.txt")
    child_instance.create()
    assert child_instance.copyFileTo(folder2.getChildFile("test_copy_file_to.txt"))
    assert child_instance.getParentDirectory() == folder1
    assert child_instance.existsAsFile()
    assert folder2.getChildFile("test_copy_file_to.txt").existsAsFile()

    folder1.deleteRecursively()
    folder2.deleteRecursively()

#==================================================================================================

def test_replace_file_in():
    child_instance1 = juce.File(get_runtime_data_file("test_replace_file_in_1.txt"))
    child_instance1.replaceWithText("abc")

    child_instance2 = juce.File(get_runtime_data_file("test_replace_file_in_2.txt"))
    child_instance2.replaceWithText("123")

    assert child_instance1.replaceFileIn(child_instance2)
    assert child_instance2.existsAsFile()
    assert child_instance2.loadFileAsString() == "abc"

    child_instance1.deleteFile()
    child_instance2.deleteFile()

#==================================================================================================

def test_copy_directory_to():
    folder1 = juce.File(get_runtime_data_file("test_copy_directory_to_1"))
    folder1.createDirectory()

    folder2 = juce.File(get_runtime_data_file("test_copy_directory_to_2"))

    child_instance = folder1.getChildFile("test_copy_directory_to.txt")
    child_instance.create()

    assert folder1.copyDirectoryTo(folder2)
    assert folder2.isDirectory()
    assert folder2.getChildFile("test_copy_directory_to.txt").existsAsFile()

    folder1.deleteRecursively()
    folder2.deleteRecursively()

#==================================================================================================

def test_get_number_of_child_files():
    file_instance = juce.File(this_folder)
    num_files = file_instance.getNumberOfChildFiles(int(juce.File.findFiles))
    num_folders = file_instance.getNumberOfChildFiles(int(juce.File.findDirectories))
    num_file_and_folders = file_instance.getNumberOfChildFiles(int(juce.File.findFilesAndDirectories))
    assert num_files > 0
    assert num_folders > 0
    assert num_file_and_folders == (num_files + num_folders)

#==================================================================================================

def test_contains_subdirectories():
    file_instance = juce.File(this_folder)
    assert file_instance.containsSubDirectories()

#==================================================================================================

def test_read_lines():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    lines = juce.StringArray()
    file_instance.readLines(lines)
    assert lines[0] == "-----BEGIN RSA PRIVATE KEY-----"

#==================================================================================================

def test_append_data():
    file_instance = juce.File(get_runtime_data_file("test_append_data.txt"))

    assert file_instance.appendData(b"data\xff")
    mb = juce.MemoryBlock()
    assert file_instance.loadFileAsData(mb) and mb == juce.MemoryBlock(b"data\xff")

    assert file_instance.appendData(b"data\xff")
    mb = juce.MemoryBlock()
    assert file_instance.loadFileAsData(mb) and mb == juce.MemoryBlock(b"data\xffdata\xff")

    file_instance.deleteFile()

#==================================================================================================

def test_replace_with_data():
    file_instance = juce.File(get_runtime_data_file("test_replace_with_data.txt"))

    assert file_instance.replaceWithData(b"data\xff")
    mb = juce.MemoryBlock()
    assert file_instance.loadFileAsData(mb) and mb == juce.MemoryBlock(b"data\xff")

    assert file_instance.replaceWithData(b"1234\xff")
    mb = juce.MemoryBlock()
    assert file_instance.loadFileAsData(mb) and mb == juce.MemoryBlock(b"1234\xff")

    file_instance.deleteFile()

#==================================================================================================

def test_append_text():
    file_instance = juce.File(get_runtime_data_file("test_append_text.txt"))

    assert file_instance.appendText("data")
    assert file_instance.loadFileAsString() == "data"
    assert file_instance.appendText("data")
    assert file_instance.loadFileAsString() == "datadata"

    file_instance.deleteFile()

#==================================================================================================

def test_replace_with_text():
    file_instance = juce.File(get_runtime_data_file("test_replace_with_text.txt"))

    assert file_instance.replaceWithText("data")
    assert file_instance.loadFileAsString() == "data"
    assert file_instance.replaceWithText("1234")
    assert file_instance.loadFileAsString() == "1234"

    file_instance.deleteFile()

#==================================================================================================

def test_has_identical_content_to():
    file1 = juce.File(get_runtime_data_file("test_has_identical_content_to_1.txt"))
    assert file1.replaceWithText("data")
    file2 = juce.File(get_runtime_data_file("test_has_identical_content_to_2.txt"))
    assert file2.replaceWithText("data")
    file3 = juce.File(get_runtime_data_file("test_has_identical_content_to_3.txt"))
    assert file3.replaceWithText("1234567890")
    file4 = juce.File(get_runtime_data_file("test_has_identical_content_to_4.txt"))
    assert file4.create().wasOk()

    assert file1.hasIdenticalContentTo(file1)
    assert file1.hasIdenticalContentTo(file2)
    assert not file1.hasIdenticalContentTo(file3)
    assert not file1.hasIdenticalContentTo(file4)
    assert file4.hasIdenticalContentTo(file4)
    assert not file4.hasIdenticalContentTo(file1)

    file1.deleteFile()
    file2.deleteFile()
    file3.deleteFile()
    file4.deleteFile()

#==================================================================================================

@pytest.mark.skip(reason="This will likely fail as it is a dangerous operation, investigate")
def test_create_file_without_checking_path():
    file = juce.File.createFileWithoutCheckingPath("/valid/path")
    assert file.exists()

#==================================================================================================

def test_create_legal_file_name():
    illegal_file_name = "file?name*with/illegal:characters"
    legal_file_name = juce.File.createLegalFileName(illegal_file_name)
    assert "/" not in legal_file_name and "?" not in legal_file_name

#==================================================================================================

def test_create_legal_path_name():
    illegal_path_name = "/path/wi*th/illegal:characters"
    legal_path_name = juce.File.createLegalPathName(illegal_path_name)
    assert ":" not in legal_path_name and "*" not in legal_path_name

#==================================================================================================

def test_create_input_stream():
    file_obj = juce.File(this_file)
    input_stream = file_obj.createInputStream()
    assert input_stream is not None

#==================================================================================================

def test_create_output_stream():
    file_obj = juce.File(get_runtime_data_file("test_create_output_stream.txt"))
    output_stream = file_obj.createOutputStream(0x8000)
    assert output_stream is not None
    del output_stream

#==================================================================================================

def test_load_file_as_data():
    file_obj = juce.File(this_file)
    memory_block = juce.MemoryBlock()
    assert file_obj.loadFileAsData(memory_block)
    assert memory_block.getSize() > 0

#==================================================================================================

def test_load_file_as_string():
    file_obj = juce.File(nonexisting_file)
    file_content = file_obj.loadFileAsString()
    assert isinstance(file_content, str)
    assert len(file_content) == 0

    file_obj = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    file_content = file_obj.loadFileAsString()
    assert isinstance(file_content, str)
    assert len(file_content) > 0

#==================================================================================================

def test_find_child_files():
    file_instance = juce.File(this_folder).getChildFile("data")
    child_files = file_instance.findChildFiles(int(juce.File.findFiles), True)
    assert isinstance(child_files, juce.Array[juce.File])
    assert child_files.size() > 0
    for child_file in child_files:
        assert isinstance(child_file, juce.File)

#==================================================================================================

def test_find_child_files_reference():
    file_instance = juce.File(this_folder).getChildFile("data")
    child_files = juce.Array[juce.File]()
    assert file_instance.findChildFiles(child_files, int(juce.File.findFiles), True)
    assert child_files.size() > 0
    for child_file in child_files:
        assert isinstance(child_file, juce.File)

#==================================================================================================

def test_create_symbolic_link():
    link_obj = get_runtime_data_file("test_create_symbolic_link.txt")
    target_obj = juce.File(this_file)
    assert target_obj.createSymbolicLink(link_obj, True)
    assert link_obj.exists()
    link_obj.deleteFile()

#==================================================================================================

def test_is_symbolic_link():
    link_obj = get_runtime_data_file("test_is_symbolic_link.txt")
    target_obj = juce.File(this_file)
    target_obj.createSymbolicLink(link_obj, True)
    assert link_obj.isSymbolicLink()
    link_obj.deleteFile()

#==================================================================================================

@pytest.mark.skipif(sys.platform == "win32", reason="Fails on windows for whatever reason")
def test_get_linked_target():
    link_obj = get_runtime_data_file("test_get_linked_target.txt")
    target_obj = juce.File(this_file)
    assert target_obj.createSymbolicLink(link_obj, True)
    assert link_obj.getLinkedTarget() == target_obj
    link_obj.deleteFile()

#==================================================================================================

def test_get_bytes_free_on_volume():
    file1 = juce.File.getSpecialLocation(juce.File.SpecialLocationType.userDocumentsDirectory)
    bytes_free = file1.getBytesFreeOnVolume()
    assert bytes_free >= 0

#==================================================================================================

def test_get_volume_total_size():
    file1 = juce.File.getSpecialLocation(juce.File.SpecialLocationType.userDocumentsDirectory)
    total_size = file1.getVolumeTotalSize()
    assert total_size >= 0

#==================================================================================================

def test_get_volume_label():
    file_instance = juce.File(this_file)
    assert isinstance(file_instance.getVolumeLabel(), str)

#==================================================================================================

def test_get_volume_serial_number():
    file_instance = juce.File(this_file)
    assert isinstance(file_instance.getVolumeSerialNumber(), int)

#==================================================================================================

@pytest.mark.skip(reason="This is hardly testable as it is now")
def test_reveal_to_user():
    file = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    file.revealToUser()

#==================================================================================================

def test_get_separator_string():
    separator = juce.File.getSeparatorString()
    assert isinstance(separator, str)

#==================================================================================================

def test_are_file_names_case_sensitive():
    case_sensitive = juce.File.areFileNamesCaseSensitive()
    assert isinstance(case_sensitive, bool)

#==================================================================================================

def test_add_trailing_separator():
    path = "/path/to/directory"
    path_with_separator = juce.File.addTrailingSeparator(path)
    assert path_with_separator.endswith(juce.File.getSeparatorString())

#==================================================================================================

def test_get_native_linked_target():
    file = juce.File("/path/to/somefile.txt")
    linked_target = file.getNativeLinkedTarget()
    assert isinstance(linked_target, str)

#==================================================================================================

def test_find_file_system_roots():
    roots = juce.Array[juce.File]()
    juce.File.findFileSystemRoots(roots)
    assert len(roots) > 0

#==================================================================================================

@pytest.mark.skip(reason="This is hardly testable as it is now")
def test_is_on_cdrom_drive():
    file1 = juce.File("D:/path/to/file1")
    assert not file1.isOnCDRomDrive()

#==================================================================================================

@pytest.mark.skip(reason="This is hardly testable as it is now")
def test_is_on_hard_disk():
    file1 = juce.File("C:/path/to/file1")
    assert not file1.isOnHardDisk()

#==================================================================================================

@pytest.mark.skip(reason="This is hardly testable as it is now")
def test_is_on_removable_drive():
    file1 = juce.File("E:/path/to/file1")
    assert not file1.isOnRemovableDrive()

#==================================================================================================

@pytest.mark.skip(reason="This is hardly testable as it is now")
def test_start_as_process():
    file1 = juce.File("path/to/executable.exe")
    assert file1.startAsProcess()

#==================================================================================================

@pytest.mark.skipif(sys.platform != "win32", reason="Platform-specific test")
def test_create_shortcut():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    file_shortcut = get_runtime_data_file("test_create_shortcut.lnk")
    assert file_instance.createShortcut("description", file_shortcut) == True
    file_shortcut.deleteFile()

#==================================================================================================

@pytest.mark.skipif(sys.platform != "win32", reason="Platform-specific test")
def test_is_shortcut():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert not file_instance.isShortcut()
    file_shortcut = get_runtime_data_file("test_is_shortcut.lnk")
    assert file_instance.createShortcut("description", file_shortcut) == True
    assert file_shortcut.isShortcut()
    file_shortcut.deleteFile()

#==================================================================================================

@pytest.mark.skipif(sys.platform != "darwin", reason="Platform-specific test")
def test_is_bundle():
    file_obj = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert not file_obj.isBundle()

#==================================================================================================

@pytest.mark.skip(reason="Plainly annoying when running locally")
def test_add_to_dock():
    file_obj = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    file_obj.addToDock()

#==================================================================================================

@pytest.mark.skipif(sys.platform != "darwin", reason="Platform-specific test")
def test_get_mac_os_type():
    file_instance = juce.File(this_folder).getChildFile("data").getChildFile("somefile.txt")
    assert isinstance(file_instance.getMacOSType(), int)

#==================================================================================================

@pytest.mark.skipif(sys.platform != "darwin", reason="Platform-specific test")
def test_get_container_for_security_application_group_identifier():
    app_group = "com.example.app"
    container = juce.File.getContainerForSecurityApplicationGroupIdentifier(app_group)
    assert isinstance(container, juce.File)
