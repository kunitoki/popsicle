import os

import popsicle as juce

this_file = os.path.abspath(__file__)
this_folder = os.path.dirname(this_file)

#==================================================================================================

def test_iterate_this_folder():
    folder = juce.File(this_folder)

    for entry in juce.RangedDirectoryIterator(folder, isRecursive=False):
        f = entry.getFile()

        assert f.getParentDirectory() == folder
        if f.getFileName() == "data" or f.getFileName() == "__pycache__":
            assert f.isDirectory()
            assert entry.isDirectory()
            assert not entry.isHidden()
            assert not entry.isReadOnly()

        if f.getFileExtension() == ".py":
            assert f.existsAsFile()
            assert not entry.isDirectory()
            assert not entry.isHidden()
            assert not entry.isReadOnly()
            assert entry.getFileSize() >= 0

#==================================================================================================

def test_iterate_progress():
    folder = juce.File(this_folder)
    lastProgress = -1.0

    for entry in juce.RangedDirectoryIterator(folder, isRecursive=False):
        progress = entry.getEstimatedProgress()

        assert progress > lastProgress or progress <= 0.0 or progress >= 1.0

        lastProgress = entry.getEstimatedProgress()
