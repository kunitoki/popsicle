import popsicle as juce

#==================================================================================================

def test_constructor():
    a = juce.PropertySet()
    assert not a.containsKey("abc")

    a.setValue("abc", 1)
    assert a.containsKey("abc")
    assert a.getValue("abc", "99") == "1"
    assert a.getValue("123", "99") == "99"
    assert a.getIntValue("abc", 99) == 1
    assert a.getIntValue("123", 99) == 99
    assert a.getDoubleValue("abc", 99.0) == 1.0
    assert a.getDoubleValue("123", 99.0) == 99.0
    assert a.getBoolValue("abc", False) == True
    assert a.getBoolValue("123", False) == False

    a.removeValue("123")
    assert a.containsKey("abc")

    a.removeValue("abc")
    assert not a.containsKey("abc")

    a.setValue("abc", 1)
    a.setValue("def", "qwertyuiop")
    a.setValue("ghi", True)

    b = juce.PropertySet(a)
    assert b.containsKey("abc")
    assert b.containsKey("def")
    assert b.containsKey("ghi")

    c = juce.PropertySet()
    assert not c.containsKey("abc")
    assert not c.containsKey("def")
    assert not c.containsKey("ghi")

    c.addAllPropertiesFrom(b)
    assert c.containsKey("abc")
    assert c.containsKey("def")
    assert c.containsKey("ghi")

    c.clear()
    assert not c.containsKey("abc")
    assert not c.containsKey("def")
    assert not c.containsKey("ghi")

#==================================================================================================

def test_property_set_default_construction():
    prop_set = juce.PropertySet()
    assert prop_set is not None

#==================================================================================================

def test_property_set_ignore_case_construction():
    prop_set = juce.PropertySet(True)
    assert prop_set is not None

#==================================================================================================

def test_property_set_copy_construction():
    original = juce.PropertySet()
    original.setValue("key1", "value1")
    copy = juce.PropertySet(original)
    assert copy.getValue("key1") == "value1"

#==================================================================================================

def test_property_set_assignment():
    original = juce.PropertySet()
    original.setValue("key1", "value1")
    copy = juce.PropertySet()
    copy = original
    assert copy.getValue("key1") == "value1"

#==================================================================================================

def test_set_get_value_string():
    prop_set = juce.PropertySet()
    prop_set.setValue("key1", "value1")
    assert prop_set.getValue("key1") == "value1"

#==================================================================================================

def test_set_get_value_int():
    prop_set = juce.PropertySet()
    prop_set.setValue("key2", 42)
    assert prop_set.getIntValue("key2") == 42

#==================================================================================================

def test_set_get_value_double():
    prop_set = juce.PropertySet()
    prop_set.setValue("key3", 3.14)
    assert prop_set.getDoubleValue("key3") == 3.14

#==================================================================================================

def test_set_get_value_bool():
    prop_set = juce.PropertySet()
    prop_set.setValue("key4", True)
    assert prop_set.getBoolValue("key4") is True

#==================================================================================================

def test_set_get_value_xml():
    prop_set = juce.PropertySet()
    xml_string = "<test><element>Test</element></test>"
    xml = juce.XmlDocument.parse(xml_string)
    prop_set.setValue("key5", xml)
    xml_element = prop_set.getXmlValue("key5")
    assert xml_element.toString(juce.XmlElement.TextFormat().withoutHeader().singleLine()) == xml_string

#==================================================================================================

def test_add_all_properties_from():
    source = juce.PropertySet()
    source.setValue("key1", "value1")
    source.setValue("key2", 42)

    target = juce.PropertySet()
    target.addAllPropertiesFrom(source)

    assert target.getValue("key1") == "value1"
    assert target.getIntValue("key2") == 42

#==================================================================================================

def test_remove_value():
    prop_set = juce.PropertySet()
    prop_set.setValue("key1", "value1")
    prop_set.removeValue("key1")
    assert prop_set.getValue("key1", "default") == "default"

#==================================================================================================

def test_contains_key():
    prop_set = juce.PropertySet()
    prop_set.setValue("key1", "value1")
    assert prop_set.containsKey("key1") is True
    assert prop_set.containsKey("key2") is False

#==================================================================================================

def test_clear():
    prop_set = juce.PropertySet()
    prop_set.setValue("key1", "value1")
    prop_set.clear()
    assert prop_set.getValue("key1", "default") == "default"

#==================================================================================================

def test_xml_serialization():
    prop_set = juce.PropertySet()
    prop_set.setValue("key1", "value1")
    prop_set.setValue("key2", 42)

    xml = prop_set.createXml("properties")
    new_prop_set = juce.PropertySet()
    new_prop_set.restoreFromXml(xml)

    assert new_prop_set.getValue("key1") == "value1"
    assert new_prop_set.getIntValue("key2") == 42

#==================================================================================================

def test_fallback_property_set():
    fallback = juce.PropertySet()
    fallback.setValue("fallbackKey", "fallbackValue")

    prop_set = juce.PropertySet()
    prop_set.setFallbackPropertySet(fallback)

    assert prop_set.getValue("fallbackKey") == "fallbackValue"
    assert prop_set.getValue("nonexistentKey", "default") == "default"
