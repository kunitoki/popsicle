import os
import pytest

from ..utilities import get_runtime_data_folder
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

    a.removeAttribute(name_type("nonexisting"))
    assert a.getNumAttributes() == 4

    a.removeAttribute(name_type("test4"))
    assert a.getNumAttributes() == 3

    a.removeAllAttributes()
    assert a.getNumAttributes() == 0

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

def test_text_element():
    a = juce.XmlElement("root")
    assert not a.isTextElement()

    a.addChildElement(juce.XmlElement.createTextElement("abcdefghijklmnopqrstuvwxyz"))

    child = a.getChildElement(0)
    assert child.getTagName() == ""
    assert child.isTextElement()
    assert child.getText() == "abcdefghijklmnopqrstuvwxyz"
    assert child.getAllSubText() == "abcdefghijklmnopqrstuvwxyz"
    assert a.getAllSubText() == "abcdefghijklmnopqrstuvwxyz"

    format = juce.XmlElement.TextFormat().singleLine().withoutHeader()
    assert a.toString(format) == """<root>abcdefghijklmnopqrstuvwxyz</root>"""

    child.setTagName("child")
    assert a.toString(format) == """<root><child text="abcdefghijklmnopqrstuvwxyz"/></root>"""
    assert a.getAllSubText() == ""
    assert child.getAllSubText() == ""

#==================================================================================================

def test_child_elements_manipulation():
    parent = juce.XmlElement("PARENT")
    child1 = juce.XmlElement("CHILD1")
    child2 = juce.XmlElement("CHILD2")

    parent.addChildElement(child1)
    parent.prependChildElement(child2)

    assert parent.getNumChildElements() == 2
    assert parent.getFirstChildElement().getTagName() == "CHILD2"

    parent.removeChildElement(child1, True)
    assert parent.getNumChildElements() == 1
    parent.deleteAllChildElements()
    assert parent.getNumChildElements() == 0

#==================================================================================================

def test_invalid_xml_name():
    assert not juce.XmlElement.isValidXmlName("1InvalidName")

#==================================================================================================

def test_namespace_handling():
    elem = juce.XmlElement("ns:ANIMAL")
    assert elem.getTagName() == "ns:ANIMAL"
    assert elem.getNamespace() == "ns"
    assert elem.getTagNameWithoutNamespace() == "ANIMAL"
    assert elem.hasTagName("ns:ANIMAL")
    assert elem.hasTagNameIgnoringNamespace("ANIMAL")

#==================================================================================================

def test_deeply_nested_elements():
    root = juce.XmlElement("root")
    level1 = root.createNewChildElement("level1")
    level2 = level1.createNewChildElement("level2")
    level3 = level2.createNewChildElement("level3")

    assert root.findParentElementOf(level3) is level2
    assert root.getNumChildElements() == 1
    assert root.getFirstChildElement().getFirstChildElement().getTagName() == "level2"

#==================================================================================================

def test_sorting_child_elements():
    root = juce.XmlElement("root")
    for name in ["banana", "apple", "cherry"]:
        child = root.createNewChildElement("item")
        child.setAttribute("name", name)

    class Comparator(juce.XmlElement.Comparator):
        def compareElements(self, a, b):
            x = a.getStringAttribute("name")
            y = b.getStringAttribute("name")
            if x == y: return 0
            elif x < y: return -1
            else: return 1

    root.sortChildElements(Comparator())

    names = [child.getStringAttribute("name") for child in root.getChildIterator()]
    assert names == ["apple", "banana", "cherry"]

#==================================================================================================

def test_sorting_child_elements_lambda():
    root = juce.XmlElement("root")
    for name in ["banana", "apple", "cherry"]:
        child = root.createNewChildElement("item")
        child.setAttribute("name", name)

    def strncmp(a, b):
        if a == b: return 0
        elif a < b: return -1
        else: return 1

    root.sortChildElements(lambda x, y: strncmp(x.getStringAttribute("name"), y.getStringAttribute("name")))

    names = [child.getStringAttribute("name") for child in root.getChildIterator()]
    assert names == ["apple", "banana", "cherry"]

#==================================================================================================

def test_complex_xml_structure_to_string():
    root = juce.XmlElement("menu")
    item = root.createNewChildElement("item")
    item.setAttribute("name", "coffee")
    item.setAttribute("price", "$1")
    item.addTextElement("Hot and invigorating")

    xml_string = root.toString()
    assert "<menu>" in xml_string
    assert "<item name=\"coffee\" price=\"$1\">" in xml_string
    assert "Hot and invigorating" in xml_string
    assert "</item>" in xml_string
    assert "</menu>" in xml_string

#==================================================================================================

def test_error_adding_child_to_self():
    elem = juce.XmlElement("self")
    elem.addChildElement(elem)
    assert elem.getNumChildElements() == 1

#==================================================================================================

def test_attribute_value_comparisons():
    elem = juce.XmlElement("item")
    elem.setAttribute("available", "true")
    assert elem.compareAttribute("available", "true")
    assert not elem.compareAttribute("available", "false")
    assert elem.getBoolAttribute("available")

#==================================================================================================

def test_handling_invalid_characters_in_text_elements():
    elem = juce.XmlElement("text")
    elem.addTextElement("Invalid character: \u0000")
    assert elem.getChildElement(0).getAllSubText() == "Invalid character: "

#==================================================================================================

def test_write_to_file():
    elem = juce.XmlElement("note")
    elem.addTextElement("This is a note")
    temp_file = get_runtime_data_folder().getChildFile("test_write_to_file.xml")
    assert elem.writeTo(temp_file)
    with open(temp_file.getFullPathName(), "r") as file:
        content = file.read()
    assert "note" in content
    assert "This is a note" in content

#==================================================================================================

def test_write_to_stream():
    elem = juce.XmlElement("note")
    elem.addTextElement("This is a note")
    temp_file = get_runtime_data_folder().getChildFile("test_write_to_stream.xml")
    elem.writeTo(temp_file.createOutputStream())
    with open(temp_file.getFullPathName(), "r") as file:
        content = file.read()
    assert "note" in content
    assert "This is a note" in content

#==================================================================================================

def test_read_from_file():
    temp_file = get_runtime_data_folder().getChildFile("test_read_from_file.xml")
    with open(temp_file.getFullPathName(), "w") as file:
        file.write("<note><to>User</to><message>Hello, World!</message></note>")

    root = juce.XmlDocument.parse(temp_file)
    assert root.getTagName() == "note"
    assert root.getChildElement(0).getAllSubText() == "User"
    assert root.getChildElement(1).getAllSubText() == "Hello, World!"
