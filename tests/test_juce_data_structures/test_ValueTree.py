import popsicle as juce


#==================================================================================================

class CustomListener(juce.ValueTree.Listener):
    changes = []

    def valueTreePropertyChanged(self, tree, property):
        self.changes.append(("propertyChanged", tree, property))

    def valueTreeChildAdded(self, parentTree, childWhichHasBeenAdded):
        self.changes.append(("childAdded", parentTree, childWhichHasBeenAdded.getType()))

    def valueTreeChildRemoved(self, parentTree, childWhichHasBeenRemoved, indexFromWhichChildWasRemoved):
        self.changes.append(("childRemoved", parentTree, childWhichHasBeenRemoved.getType(), indexFromWhichChildWasRemoved))

    def valueTreeChildOrderChanged(self, parentTreeWhoseChildrenHaveMoved, oldIndex, newIndex):
        self.changes.append(("childOrderChanged", parentTreeWhoseChildrenHaveMoved, oldIndex, newIndex))

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

def test_copy_constructor():
    original = juce.ValueTree("Original")
    copy = juce.ValueTree(original)
    assert copy.getType() == "Original"

#==================================================================================================

def test_equality_operators():
    vt1 = juce.ValueTree("EqualTest")
    vt2 = juce.ValueTree(vt1)
    assert vt1 == vt2
    vt3 = juce.ValueTree("EqualTest")
    assert vt1 != vt3

#==================================================================================================

def test_is_equivalent_to():
    vt1 = juce.ValueTree("EquivalentTest")
    vt2 = juce.ValueTree(vt1)
    assert vt1.isEquivalentTo(vt2)
    vt3 = juce.ValueTree("EquivalentTest")
    assert vt1.isEquivalentTo(vt3)

#==================================================================================================

def test_deep_copy_and_equivalence():
    vt1 = juce.ValueTree("Parent")
    child = juce.ValueTree("Child")
    child.setProperty("key", "value", None)
    vt1.appendChild(child, None)

    # Test deep copy
    vt2 = vt1.createCopy()
    assert vt2.isEquivalentTo(vt1)
    assert vt2 != vt1  # Checks that they are not the same instance but equivalent in content

    # Modify the copy and ensure they are no longer equivalent
    vt2.getChild(0).setProperty("key", "newValue", None)
    assert not vt2.isEquivalentTo(vt1)

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

def test_create_copy():
    vt = juce.ValueTree("CopyTest")
    copy = vt.createCopy()
    assert copy.getType() == "CopyTest"
    assert not (vt == copy)

#==================================================================================================

def test_set_get_property():
    vt = juce.ValueTree("Props")
    vt.setProperty("key", "value", None)
    assert vt.getProperty("key") == "value"

#==================================================================================================

def test_remove_property():
    vt = juce.ValueTree("Props")
    vt.setProperty("key", "value", None)
    vt.removeProperty("key", None)
    assert vt.getProperty("key") is None

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

#==================================================================================================

def test_add_get_remove_child():
    parent = juce.ValueTree("Parent")
    child = juce.ValueTree("Child")
    parent.appendChild(child, None)
    assert parent.getNumChildren() == 1
    assert parent.getChild(0).getType() == "Child"
    parent.removeChild(child, None)
    assert parent.getNumChildren() == 0

#==================================================================================================

def test_get_child_with_property():
    parent = juce.ValueTree("Parent")
    child = juce.ValueTree("Child")
    child.setProperty("name", "testChild", None)
    parent.appendChild(child, None)
    result = parent.getChildWithProperty("name", "testChild")
    assert result.isValid()
    assert result.getProperty("name") == "testChild"

#==================================================================================================

def test_serialize_deserialize():
    vt = juce.ValueTree("Serialization")
    vt.setProperty("key", "value", None)
    child = juce.ValueTree("Child")
    vt.appendChild(child, None)

    stream = juce.MemoryOutputStream()
    vt.writeToStream(stream)
    data = stream.getMemoryBlock()

    inputStream = juce.MemoryInputStream(data, keepInternalCopyOfData=False)
    deserialized = juce.ValueTree.readFromStream(inputStream)

    assert deserialized.getType() == "Serialization"
    assert deserialized.getProperty("key") == "value"
    assert deserialized.getNumChildren() == 1
    assert deserialized.getChild(0).getType() == "Child"

#==================================================================================================

def test_undo_redo_property_change():
    undoManager = juce.UndoManager()
    vt = juce.ValueTree("TestUndo")
    vt.setProperty("key", "initialValue", None)

    # Change the property with undo support
    vt.setProperty("key", "newValue", undoManager)
    assert vt.getProperty("key") == "newValue"

    # Undo the change
    undoManager.undo()
    assert vt.getProperty("key") == "initialValue"

    # Redo the change
    undoManager.redo()
    assert vt.getProperty("key") == "newValue"

#==================================================================================================

def test_complex_property_and_child_manipulation():
    undoManager = juce.UndoManager()
    parent = juce.ValueTree("Parent")
    child1 = juce.ValueTree("Child1")
    child2 = juce.ValueTree("Child2")

    # Add children with properties
    child1.setProperty("name", "Child1", undoManager)
    child2.setProperty("name", "Child2", undoManager)
    parent.appendChild(child1, undoManager)
    parent.appendChild(child2, undoManager)

    # Modify child property and then undo
    undoManager.beginNewTransaction()
    child1.setProperty("name", "ModifiedChild1", undoManager)
    assert child1.getProperty("name") == "ModifiedChild1"
    undoManager.undo()
    assert child1.getProperty("name") == "Child1"

    # Remove a child and undo the removal
    undoManager.beginNewTransaction()
    parent.removeChild(child2, undoManager)
    assert parent.getNumChildren() == 1
    undoManager.undo()
    assert parent.getNumChildren() == 2

#==================================================================================================

def test_property_and_child_iteration():
    vt = juce.ValueTree("Parent")
    vt.setProperty("key1", "value1", None)
    vt.setProperty("key2", "value2", None)
    child1 = juce.ValueTree("Child1")
    child2 = juce.ValueTree("Child2")
    vt.appendChild(child1, None)
    vt.appendChild(child2, None)

    # Test iterating over properties
    keys = [vt.getPropertyName(i) for i in range(vt.getNumProperties())]
    assert "key1" in keys
    assert "key2" in keys

    # Test iterating over children
    child_types = [child.getType() for child in vt]
    assert "Child1" in child_types
    assert "Child2" in child_types

#==================================================================================================

def test_value_tree_listener():
    vt = juce.ValueTree("ListenerTest")
    listener = CustomListener()
    vt.addListener(listener)

    # Change a property and see if the listener catches it
    vt.setProperty("key", "value", None)
    assert ("propertyChanged", vt, "key") in listener.changes

    vt.removeListener(listener)
    # Clear changes and verify no more changes are recorded
    listener.changes.clear()
    vt.setProperty("anotherKey", "anotherValue", None)
    assert not listener.changes

#==================================================================================================

def test_listener_on_child_added():
    parent = juce.ValueTree("Parent")
    listener = CustomListener()
    parent.addListener(listener)

    child = juce.ValueTree("Child")
    parent.appendChild(child, None)

    assert ("childAdded", parent, "Child") in listener.changes

#==================================================================================================

def test_listener_on_child_removed():
    parent = juce.ValueTree("Parent")
    child = juce.ValueTree("Child")
    parent.appendChild(child, None)  # Set up initial state

    listener = CustomListener()
    parent.addListener(listener)

    parent.removeChild(child, None)

    assert ("childRemoved", parent, "Child", 0) in listener.changes

#==================================================================================================

def test_listener_on_child_order_changed():
    parent = juce.ValueTree("Parent")
    child1 = juce.ValueTree("Child1")
    child2 = juce.ValueTree("Child2")
    parent.appendChild(child1, None)
    parent.appendChild(child2, None)  # Ensure there are two children for the order change

    listener = CustomListener()
    parent.addListener(listener)

    # Assuming moveChild is zero-indexed and correctly bound
    parent.moveChild(0, 1, None)  # Move the first child to the position of the second

    assert ("childOrderChanged", parent, 0, 1) in listener.changes

#==================================================================================================

def test_listener_removal():
    vt = juce.ValueTree("ListenerTest")
    listener = CustomListener()
    vt.addListener(listener)

    # Change a property to ensure listener is initially active
    vt.setProperty("initialKey", "initialValue", None)
    assert ("propertyChanged", vt, "initialKey") in listener.changes

    # Remove listener and verify it no longer receives updates
    vt.removeListener(listener)
    vt.setProperty("newKey", "newValue", None)
    assert ("propertyChanged", vt, "newKey") not in listener.changes

#==================================================================================================

def test_basic_xml_export():
    # Create a ValueTree with some properties and children
    parent = juce.ValueTree("Parent")
    parent.setProperty("parentProp", "parentValue", None)
    child = juce.ValueTree("Child")
    child.setProperty("childProp", "childValue", None)
    parent.appendChild(child, None)

    # Export to XML
    xml = parent.createXml()

    # Convert XML to string for easier assertion (assuming createXml() returns an XmlElement equivalent)
    xmlString = xml.toString()

    # Check the structure and properties in the XML string
    assert "<Parent parentProp=\"parentValue\">" in xmlString
    assert "<Child childProp=\"childValue\"/>" in xmlString
    assert "</Parent>" in xmlString

#==================================================================================================

def test_nested_xml_export():
    parent = juce.ValueTree("Grandparent")
    parent.setProperty("gpProp", "gpValue", None)

    child = juce.ValueTree("Parent")
    child.setProperty("pProp", "pValue", None)

    grandchild = juce.ValueTree("Child")
    grandchild.setProperty("cProp", "cValue", None)

    child.appendChild(grandchild, None)
    parent.appendChild(child, None)

    # Export to XML
    xml = parent.createXml()
    xmlString = xml.toString()

    # Assertions to verify the nested structure and properties
    assert "<Grandparent gpProp=\"gpValue\">" in xmlString
    assert "<Parent pProp=\"pValue\">" in xmlString
    assert "<Child cProp=\"cValue\"/>" in xmlString
    assert "</Parent>" in xmlString
    assert "</Grandparent>" in xmlString

#==================================================================================================

def test_xml_round_trip():
    original = juce.ValueTree("RoundTrip")
    original.setProperty("rtProp", "rtValue", None)
    child = juce.ValueTree("Child")
    child.setProperty("childProp", "childValue", None)
    original.appendChild(child, None)

    newTree = juce.ValueTree.fromXml(original.createXml())

    assert newTree.isEquivalentTo(original)
    assert newTree.getType() == "RoundTrip"
    assert newTree.getProperty("rtProp") == "rtValue"
    assert newTree.getChild(0).getType() == "Child"
    assert newTree.getChild(0).getProperty("childProp") == "childValue"

#==================================================================================================

def test_stress_large_data_sets():
    parent = juce.ValueTree("Parent")

    for i in range(1000):
        child = juce.ValueTree(f"Child{i}")

        for j in range(10):
            child.setProperty(f"key{j}", f"value{j}", None)

        parent.appendChild(child, None)

    assert parent.getNumChildren() == 1000

    for i, child in enumerate(parent):
        assert child.hasType(f"Child{i}")
        assert child.getNumProperties() == 10
