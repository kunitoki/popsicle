import pytest

import popsicle as juce

#==================================================================================================

def test_constructor_raises():
    with pytest.raises(Exception):
        cv = juce.CachedValue[int](None, "testBool", None)

    with pytest.raises(Exception):
        cv = juce.CachedValue[int](None, "testBool", None, 1)

    with pytest.raises(Exception):
        tree = juce.ValueTree("Settings")
        cv = juce.CachedValue[int](tree, None, None)

    with pytest.raises(Exception):
        tree = juce.ValueTree("Settings")
        cv = juce.CachedValue[int](tree, None, None, 1)

#==================================================================================================

def test_construct_with_valuetree_property():
    tree = juce.ValueTree("Settings")
    tree.setProperty("testBool", True, None)

    cv = juce.CachedValue[bool](tree, "testBool", None)
    assert cv.get() == True

#==================================================================================================

def test_construct_with_default_value():
    tree = juce.ValueTree("Settings")

    cv = juce.CachedValue[int](tree, "testInt", None, 42)
    assert cv.getPropertyID() == "testInt"
    assert cv.getDefault() == 42
    assert cv.get() == 42

#==================================================================================================

def test_get_and_set_value():
    tree = juce.ValueTree("Settings")

    cv = juce.CachedValue[float](tree, "testFloat", None, 3.14)
    assert cv.get() == pytest.approx(3.14)

    cv.setValue(2.71, None)
    assert cv.get() == pytest.approx(2.71)

#==================================================================================================

def test_get_property_as_value():
    tree = juce.ValueTree("Settings")
    tree.setProperty("testFloat", 2.71, None)

    cv = juce.CachedValue[float](tree, "testFloat", None)
    propValue = cv.getPropertyAsValue()
    assert isinstance(propValue, juce.Value)
    assert propValue.getValue() == pytest.approx(2.71)

#==================================================================================================

def test_reset_to_default():
    tree = juce.ValueTree("Settings")
    cv = juce.CachedValue[str](tree, "xxx", None, "default")

    cv.setValue("changed", None)
    assert cv.get() == "changed"

    cv.resetToDefault()
    assert cv.get() == "default"

#==================================================================================================

def test_comparison_operators():
    tree = juce.ValueTree("Settings")

    cv = juce.CachedValue[int](tree, "abc", None, 10)
    assert (cv.get() == 10)
    assert not (cv.get() == 9)
    assert (cv.get() != 9)
    assert not (cv.get() != 10)

    cv2 = juce.CachedValue[int](tree, "def", None, 11)
    cv3 = juce.CachedValue[int](tree, "ghi", None, 10)
    assert (cv == cv3)
    assert not (cv == cv2)
    assert (cv != cv2)
    assert not (cv != cv3)

#==================================================================================================

def test_refer_to_raises():
    with pytest.raises(Exception):
        cv = juce.CachedValue[str]()
        cv.referTo(None, "testBool", None)

    with pytest.raises(Exception):
        cv = juce.CachedValue[str]()
        cv.referTo(None, "testBool", None, "1")

    with pytest.raises(Exception):
        tree = juce.ValueTree("Settings")
        cv = juce.CachedValue[str]()
        cv.referTo(tree, None, None)

    with pytest.raises(Exception):
        tree = juce.ValueTree("Settings")
        cv = juce.CachedValue[str]()
        cv.referTo(tree, None, None, "1")

#==================================================================================================

def test_refer_to_and_valuetree_changes():
    tree = juce.ValueTree("Settings")

    cv = juce.CachedValue[bool]()
    cv.referTo(tree, "testBool", None, False)
    assert cv.get() == False

    tree.setProperty("testBool", True, None)
    cv.forceUpdateOfCachedValue()
    assert cv.get() == True

#==================================================================================================

def test_refer_to_with_default():
    tree = juce.ValueTree("Settings")

    cv = juce.CachedValue[str]()
    cv.referTo(tree, "testString", None, "default")
    assert cv.get() == "default"

    tree.setProperty("testString", "updated", None)
    cv.forceUpdateOfCachedValue()
    assert cv.get() == "updated"

#==================================================================================================

def test_force_update_of_cached_value():
    tree = juce.ValueTree("Settings")
    tree.setProperty("testInt", 42, None)

    cv = juce.CachedValue[int](tree, "testInt", None)
    assert cv.get() == 42

    tree.setProperty("testInt", 24, None)
    cv.forceUpdateOfCachedValue()
    assert cv.get() == 24

#==================================================================================================

def test_is_using_default():
    cv = juce.CachedValue[int]()
    assert cv.isUsingDefault()

    tree = juce.ValueTree("Settings")
    cv = juce.CachedValue[int](tree, "prop", None, 5)
    assert cv.isUsingDefault()

    tree = juce.ValueTree("Settings", { "prop": 1 })
    cv = juce.CachedValue[int](tree, "prop", None, 5)
    assert not cv.isUsingDefault()

#==================================================================================================

def test_value_change_with_undo_manager():
    tree = juce.ValueTree("Settings")
    um = juce.UndoManager()

    cv = juce.CachedValue[int](tree, "testInt", um, 10)
    assert cv.getUndoManager() == um

    um.beginNewTransaction()
    cv.setValue(20, um)
    assert tree["testInt"] == 20

    um.undo()
    assert cv.get() == 10
