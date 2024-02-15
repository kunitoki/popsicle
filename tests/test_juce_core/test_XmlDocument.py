import os
import pytest

import popsicle as juce


#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))

@pytest.fixture
def xml_file():
    yield juce.File(this_file.getSiblingFile("data").getChildFile("test.xml"))

@pytest.fixture
def broken_xml_file():
    yield juce.File(this_file.getSiblingFile("data").getChildFile("test_broken.xml"))

#==================================================================================================

def test_construct_file(xml_file, broken_xml_file):
    a = juce.XmlDocument(broken_xml_file)
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is None
    assert a.getLastParseError()

    a = juce.XmlDocument(xml_file.getParentDirectory().getChildFile("noexistent.xml"))
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is None
    assert a.getLastParseError()

    a = juce.XmlDocument(xml_file)
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is not None
    assert not a.getLastParseError()

    assert a.getDocumentElementIfTagMatches("root") is not None
    assert a.getDocumentElementIfTagMatches("nonexistent") is None

#==================================================================================================

def test_construct_string(xml_file, broken_xml_file):
    a = juce.XmlDocument("")
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is None
    assert a.getLastParseError()

    a = juce.XmlDocument(broken_xml_file.loadFileAsString())
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is None
    assert a.getLastParseError()

    a = juce.XmlDocument(xml_file.loadFileAsString())
    assert not a.getLastParseError()
    el = a.getDocumentElement()
    assert el is not None
    assert not a.getLastParseError()

    assert a.getDocumentElementIfTagMatches("root") is not None
    assert a.getDocumentElementIfTagMatches("nonexistent") is None

#==================================================================================================

def test_construct_parse_file(xml_file, broken_xml_file):
    el = juce.XmlDocument.parse(broken_xml_file)
    assert el is None

    el = juce.XmlDocument.parse(xml_file.getParentDirectory().getChildFile("noexistent.xml"))
    assert el is None

    el = juce.XmlDocument.parse(xml_file)
    assert el is not None
    assert el.getTagName() == "root"

#==================================================================================================

def test_construct_parse_string(xml_file, broken_xml_file):
    el = juce.XmlDocument.parse("")
    assert el is None

    el = juce.XmlDocument.parse(broken_xml_file.loadFileAsString())
    assert el is None

    el = juce.XmlDocument.parse(xml_file.loadFileAsString())
    assert el is not None
    assert el.getTagName() == "root"
