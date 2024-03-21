import os

import popsicle as juce

#==================================================================================================

def test_wildcard_file_filter_matches_correct_files():
    filter = juce.WildcardFileFilter("*.txt", "", "description for text files")
    assert filter.isFileSuitable(juce.File(os.path.abspath(__file__)).getSiblingFile("example.txt")) == True

#==================================================================================================

def test_wildcard_file_filter_excludes_incorrect_files():
    filter = juce.WildcardFileFilter("*.txt", "", "description for text files")
    assert filter.isFileSuitable(juce.File(os.path.abspath(__file__)).getSiblingFile("example.jpg")) == False

#==================================================================================================

def test_wildcard_file_filter_with_multiple_patterns():
    filter = juce.WildcardFileFilter("*.txt;*.jpg", "", "description for text and image files")
    assert filter.isFileSuitable(juce.File(os.path.abspath(__file__)).getSiblingFile("example.txt")) == True
    assert filter.isFileSuitable(juce.File(os.path.abspath(__file__)).getSiblingFile("example.jpg")) == True
    assert filter.isFileSuitable(juce.File(os.path.abspath(__file__)).getSiblingFile("example.pdf")) == False
