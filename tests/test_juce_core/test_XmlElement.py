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

def test_construct_tag():
    a = juce.XmlElement("root")
    assert a.getTagName() == "root"

#==================================================================================================

def test_to_string_text_format():
    a = juce.XmlElement("root")
    assert a.toString() == """<?xml version="1.0" encoding="UTF-8"?>\r\n\r\n<root/>\r\n"""

    t1 = juce.XmlElement.TextFormat()
    t1.addDefaultHeader = False
    assert a.toString(t1) == """<root/>\r\n"""

    t2 = juce.XmlElement.TextFormat()
    t2.newLineChars = None
    assert a.toString(t2) == """<?xml version="1.0" encoding="UTF-8"?> <root/>"""

    t3 = juce.XmlElement.TextFormat()
    t3.customEncoding = "ISO-8859-1"
    assert a.toString(t3) == """<?xml version="1.0" encoding="ISO-8859-1"?>\r\n\r\n<root/>\r\n"""

    t4 = juce.XmlElement.TextFormat().singleLine()
    t4.addDefaultHeader = False
    assert a.toString(t4) == """<root/>"""

    t5 = juce.XmlElement.TextFormat().singleLine()
    t5.dtd = """<!DOCTYPE root SYSTEM "root.dtd">"""
    assert a.toString(t5) == """<?xml version="1.0" encoding="UTF-8"?> <!DOCTYPE root SYSTEM "root.dtd"> <root/>"""

#==================================================================================================

def test_tag_name_with_namespace():
    a = juce.XmlElement("popsicle:root")
    assert a.hasTagName("popsicle:root") == True
    assert a.getTagName() == "popsicle:root"

    assert a.getNamespace() == "popsicle"
    assert a.hasTagNameIgnoringNamespace("root") == True
    assert a.getTagNameWithoutNamespace() == "root"

    a.setTagName("other")
    assert a.hasTagName("popsicle:root") == False
    assert a.hasTagName("other") == True
    assert a.getTagName() == "other"

    assert a.getNamespace() == "other"
    assert a.hasTagNameIgnoringNamespace("root") == False
    assert a.hasTagNameIgnoringNamespace("other") == True
    assert a.getTagNameWithoutNamespace() == "other"

#==================================================================================================

@pytest.mark.parametrize("name_type", (juce.Identifier, str))
def test_attributes(name_type):
    a = juce.XmlElement("root")
    assert a.getNumAttributes() == 0

    a.setAttribute(name_type("test1"), "123")
    assert a.getNumAttributes() == 1
    assert a.hasAttribute("test1") == True
    assert a.hasAttribute("noexistent") == False
    assert a.getAttributeName(0) == "test1"
    assert a.getAttributeValue(0) == "123"
    assert a.getStringAttribute("test1") == "123"
    assert a.getStringAttribute("test_x") == ""
    assert a.getStringAttribute("test_x", "xxx") == "xxx"

    a.setAttribute(name_type("test2"), 1)
    assert a.getNumAttributes() == 2
    assert a.hasAttribute("test2") == True
    assert a.hasAttribute("noexistent") == False
    assert a.getAttributeName(1) == "test2"
    assert a.getAttributeValue(1) == "1"
    assert a.getIntAttribute("test2") == 1
    assert a.getIntAttribute("test_x") == 0
    assert a.getIntAttribute("test_x", 42) == 42

    a.setAttribute(name_type("test3"), 1.111111)
    assert a.getNumAttributes() == 3
    assert a.hasAttribute("test3") == True
    assert a.hasAttribute("noexistent") == False
    assert a.getAttributeName(2) == "test3"
    assert a.getAttributeValue(2) == "1.111111"
    assert a.getDoubleAttribute("test3") == pytest.approx(1.111111)
    assert a.getDoubleAttribute("test_x") == pytest.approx(0.0)
    assert a.getDoubleAttribute("test_x", 9.999999) == pytest.approx(9.999999)

    a.setAttribute(name_type("test4"), True)
    assert a.getNumAttributes() == 4
    assert a.hasAttribute("test4") == True
    assert a.hasAttribute("noexistent") == False
    assert a.getAttributeName(3) == "test4"
    assert a.getAttributeValue(3) == "1"
    assert a.getBoolAttribute("test4") == True
    assert a.getBoolAttribute("test_x") == False
    assert a.getBoolAttribute("test_x", True) == True

#==================================================================================================

def test_compare_attributes():
    a = juce.XmlElement("root")

    a.setAttribute("test1", "123")
    a.setAttribute("test2", "abcdef")

    assert a.compareAttribute("test1", "123")
    assert not a.compareAttribute("test1", "abcdef")

    assert a.compareAttribute("test2", "abcdef")
    assert not a.compareAttribute("test2", "ABCDEF")
    assert a.compareAttribute("test2", "ABCDEF", True)

    assert not a.compareAttribute("nonexistent", "abc", True)
    assert not a.compareAttribute("nonexistent", "ABC", False)

#==================================================================================================

def test_child_elements():
    a = juce.XmlElement("root")

    a.addChildElement(juce.XmlElement("child"))
    a.addChildElement(juce.XmlElement("child"))
    assert a.getNumChildElements() == 2

#==================================================================================================

def test_construct_parse_string(xml_file, broken_xml_file):
    el = juce.XmlDocument.parse("")
    assert el is None

    el = juce.XmlDocument.parse(broken_xml_file.loadFileAsString())
    assert el is None

    el = juce.XmlDocument.parse(xml_file.loadFileAsString())
    assert el is not None
    assert el.getTagName() == "root"
