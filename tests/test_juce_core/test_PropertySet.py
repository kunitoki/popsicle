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
