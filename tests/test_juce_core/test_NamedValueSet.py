import popsicle as juce

NV = juce.NamedValueSet.NamedValue

#==================================================================================================

def test_constructor_empty():
    named_value_set = juce.NamedValueSet()
    assert named_value_set.size() == 0
    assert named_value_set.isEmpty()

#==================================================================================================

def test_constructor_list_of_namedvalues():
    named_value_set = juce.NamedValueSet([
        NV(juce.Identifier("name1"), 42),
        NV(juce.Identifier("name2"), "value"),
        NV(juce.Identifier("name3"), True)
    ])

    assert named_value_set.size() == 3
    assert not named_value_set.isEmpty()

    named_value_set = juce.NamedValueSet([
        NV("name1", 42),
        NV("name2", "value"),
        NV("name3", True)
    ])

    assert named_value_set.size() == 3
    assert not named_value_set.isEmpty()

#==================================================================================================

def test_constructor_dict():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.size() == 3
    assert not named_value_set.isEmpty()

#==================================================================================================

def test_copy_constructor():
    original_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    copied_set = juce.NamedValueSet(original_set)

    assert original_set.size() == copied_set.size()
    assert not copied_set.isEmpty()

#==================================================================================================

def test_equality_operators():
    set1 = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    set2 = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    set3 = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": False,
        "name4": 1.2
    })

    assert set1 == set2
    assert set1 != set3

#==================================================================================================

def test_operator_bracket():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set["name1"] == 42
    assert named_value_set[juce.Identifier("name2")] == "value"
    assert named_value_set[juce.Identifier("name3")] == True

#==================================================================================================

def test_get_with_default():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.getWithDefault(juce.Identifier("name1"), 100) == 42
    assert named_value_set.getWithDefault(juce.Identifier("name4"), "default") == "default"

#==================================================================================================

def test_set():
    # Test set method
    named_value_set = juce.NamedValueSet()

    # Adding a new named value
    assert named_value_set.set(juce.Identifier("name1"), 42) is True
    assert named_value_set.size() == 1

    # Modifying an existing named value
    assert named_value_set.set(juce.Identifier("name1"), 100) is True
    assert named_value_set.size() == 1
    assert named_value_set["name1"] == 100

#==================================================================================================

def test_contains():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.contains(juce.Identifier("name1")) is True
    assert named_value_set.contains(juce.Identifier("name4")) is False

#==================================================================================================

def test_remove():
    # Test remove method
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    # Removing an existing named value
    assert named_value_set.remove("name2") is True
    assert named_value_set.size() == 2
    assert named_value_set.contains(juce.Identifier("name2")) is False

    # Removing a non-existing named value
    assert named_value_set.remove(juce.Identifier("name4")) is False
    assert named_value_set.size() == 2

#==================================================================================================

def test_get_name():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.getName(0) == juce.Identifier("name1")
    assert named_value_set.getName(1) == juce.Identifier("name2")
    assert named_value_set.getName(2) == "name3"

#==================================================================================================

def test_clear():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    named_value_set.clear()
    assert named_value_set.size() == 0
    assert named_value_set.isEmpty()

#==================================================================================================

def test_index_of():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.indexOf(juce.Identifier("name1")) == 0
    assert named_value_set.indexOf("name3") == 2
    assert named_value_set.indexOf("name4") == -1

#==================================================================================================

def test_get_var_pointer():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    var_pointer = named_value_set.getVarPointer("name2")
    assert var_pointer is not None
    assert var_pointer == "value"

    var_pointer_non_existing = named_value_set.getVarPointer(juce.Identifier("name4"))
    assert var_pointer_non_existing is None

#==================================================================================================

def test_getValueAt():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    assert named_value_set.getValueAt(0) == 42
    assert named_value_set.getValueAt(1) == "value"
    assert named_value_set.getValueAt(2) == True

#==================================================================================================

def test_get_var_pointer_at():
    named_value_set = juce.NamedValueSet([
        NV(juce.Identifier("name1"), 42),
        NV(juce.Identifier("name2"), "value"),
        NV(juce.Identifier("name3"), True)
    ])

    var_pointer_at_1 = named_value_set.getVarPointerAt(1)
    assert var_pointer_at_1 is not None
    assert var_pointer_at_1 == "value"

    var_pointer_at_invalid = named_value_set.getVarPointerAt(3)
    assert var_pointer_at_invalid is None

#==================================================================================================

def test_copy_to_xml_attributes():
    named_value_set = juce.NamedValueSet({
        "name1": 42,
        "name2": "value",
        "name3": True
    })

    xml_element = juce.XmlElement("test_element")
    named_value_set.copyToXmlAttributes(xml_element)

    assert xml_element.getStringAttribute("name1") == "42"
    assert xml_element.getStringAttribute("name2") == "value"
    assert xml_element.getStringAttribute("name3") == "1"

    assert xml_element.getIntAttribute("name1") == 42
    assert xml_element.getStringAttribute("name2") == "value"
    assert xml_element.getBoolAttribute("name3") is True

#==================================================================================================

def test_set_from_xml_attributes():
    named_value_set = juce.NamedValueSet()

    xml_element = juce.XmlElement("test_element")
    xml_element.setAttribute("name1", 42)
    xml_element.setAttribute("name2", "value")
    xml_element.setAttribute("name3", True)

    named_value_set.setFromXmlAttributes(xml_element)

    assert named_value_set.size() == 3
    assert named_value_set[juce.Identifier("name1")] == "42"
    assert named_value_set[juce.Identifier("name2")] == "value"
    assert named_value_set[juce.Identifier("name3")] == "1"
