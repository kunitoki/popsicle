import popsicle as juce

#==================================================================================================

def test_empty_constructor():
    a = juce.ValueTree()
    assert not a.isValid()
    assert a.isEquivalentTo(a)
    assert not a.hasType("abc")
    assert a.getType() == juce.Identifier()
    assert not a.getRoot().isValid()
    assert not a.getParent().isValid()
    assert not a.getSibling(0).isValid()
    assert not a.getSibling(1).isValid()
    assert a.getNumChildren() == 0

#==================================================================================================

def test_simple_constructor():
    a = juce.ValueTree("abc")
    assert a.isValid()
    assert a.isEquivalentTo(a)
    assert not a.hasType("123")
    assert a.getType() == juce.Identifier("abc")
    assert a.getType() == "abc"
    assert a.hasType("abc")
    assert a.getRoot() == a
    assert not a.getSibling(0).isValid()
    assert not a.getParent().isValid()
    assert not a.getSibling(1).isValid()
    assert a.getNumProperties() == 0
    assert a.getNumChildren() == 0

#==================================================================================================

def test_python_constructor():
    a = juce.ValueTree("PythonValueTree", {
        "a": 1,
        "b": "2",
        "c": True,
        "d": None
    })
    assert a.isValid()
    assert a.hasType("PythonValueTree")
    assert a.getNumProperties() == 4
    assert a.getProperty("a") == 1
    assert a.getProperty("b") == "2"
    assert a.getProperty("c") == True
    assert a.getProperty("d") == None

#==================================================================================================

def test_properties():
    a = juce.ValueTree("data")
    assert a.getNumProperties() == 0
    assert a.getNumChildren() == 0

    p1 = a.getProperty("property1")
    assert not a.hasProperty("property1")
    assert p1 is None

    p2 = a.getProperty("property2", 1337)
    assert p2 == 1337

    a.setProperty("property1", "abc", None)
    assert a.getNumProperties() == 1
    assert a.hasProperty("property1")
    p1 = a.getProperty("property1")
    assert p1 == "abc"

#==================================================================================================

def test_children():
    a = juce.ValueTree("parent")
    assert a.getNumChildren() == 0

    b1 = juce.ValueTree("child1")
    a.addChild(b1, -1, None)
    assert a.getNumChildren() == 1
    assert a.getChild(0) == b1

    b2 = juce.ValueTree("child2")
    a.addChild(b2, -1, None)
    assert a.getNumChildren() == 2
    assert a.getChild(1) == b2

    b3 = juce.ValueTree("child3")
    a.appendChild(b3, None)
    assert a.getNumChildren() == 3
    assert a.getChild(2) == b3

    assert b1.getRoot() == a
    assert b2.getRoot() == a
    assert b3.getRoot() == a

    assert b1.getParent() == a
    assert b2.getParent() == a
    assert b3.getParent() == a

    assert b1.getSibling(1) == b2
    assert b1.getSibling(2) == b3

    assert b2.getSibling(-1) == b1
    assert b2.getSibling(1) == b3

    assert b3.getSibling(-2) == b1
    assert b3.getSibling(-1) == b2

    a.removeChild(0, None)
    assert a.getNumChildren() == 2
    assert a.getChild(0) == b2
    assert a.getChild(1) == b3
