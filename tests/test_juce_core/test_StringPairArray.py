import popsicle as juce

#==================================================================================================

def test_constructor_empty():
    array = juce.StringPairArray()
    assert array.size() == 0
    assert len(array) == 0

#==================================================================================================

def test_string_pair_array_copy_constructor():
    original = juce.StringPairArray()
    original.set("key", "value")
    copy = juce.StringPairArray(original)
    assert copy.size() == 1
    assert copy["key"] == "value"

#==================================================================================================

def test_string_pair_array_assignment_operator():
    original = juce.StringPairArray()
    original.set("key", "value")
    copy = juce.StringPairArray()
    copy = original
    assert copy.size() == 1
    assert copy["key"] == "value"

#==================================================================================================

def test_string_pair_array_equality_operator():
    array1 = juce.StringPairArray()
    array1.set("key", "value")
    array2 = juce.StringPairArray()
    array2.set("key", "value")
    assert array1 == array2

#==================================================================================================

def test_string_pair_array_inequality_operator():
    array1 = juce.StringPairArray()
    array1.set("key", "value")
    array2 = juce.StringPairArray()
    array2.set("key", "value2")
    assert array1 != array2

#==================================================================================================

def test_string_pair_array_get_value_with_default():
    array = juce.StringPairArray()
    array.set("key", "value")
    assert array.getValue("key", "default") == "value"
    assert array.getValue("nonexistent", "default") == "default"

#==================================================================================================

def test_string_pair_array_contains_key():
    array = juce.StringPairArray()
    array.set("key", "value")
    assert array.containsKey("key")
    assert not array.containsKey("nonexistent")

#==================================================================================================

def test_string_pair_array_get_all_keys():
    array = juce.StringPairArray()
    array.set("key1", "value1")
    array.set("key2", "value2")
    keys = array.getAllKeys()
    assert "key1" in keys
    assert "key2" in keys
    assert len(keys) == 2

#==================================================================================================

def test_string_pair_array_get_all_values():
    array = juce.StringPairArray()
    array.set("key1", "value1")
    array.set("key2", "value2")
    values = array.getAllValues()
    assert "value1" in values
    assert "value2" in values
    assert len(values) == 2

#==================================================================================================

def test_string_pair_array_set():
    array = juce.StringPairArray()
    array.set("key", "value")
    assert array["key"] == "value"

#==================================================================================================

def test_string_pair_array_add_array():
    array1 = juce.StringPairArray()
    array1.set("key1", "value1")
    array2 = juce.StringPairArray()
    array2.set("key2", "value2")
    array1.addArray(array2)
    assert array1.size() == 2
    assert array1["key2"] == "value2"

#==================================================================================================

def test_string_pair_array_clear():
    array = juce.StringPairArray()
    array.set("key", "value")
    array.clear()
    assert array.size() == 0

#==================================================================================================

def test_string_pair_array_remove_by_key():
    array = juce.StringPairArray()
    array.set("key", "value")
    array.remove("key")
    assert array.size() == 0

#==================================================================================================

def test_string_pair_array_remove_by_index():
    array = juce.StringPairArray()
    array.set("key", "value")
    array.remove(0)
    assert array.size() == 0

#==================================================================================================

def test_string_pair_array_set_ignores_case():
    array = juce.StringPairArray(False)
    array.set("KEY", "value")
    assert not array.containsKey("key")
    array.setIgnoresCase(True)
    assert array.containsKey("key")

#==================================================================================================

def test_string_pair_array_get_ignores_case():
    array = juce.StringPairArray(True)
    assert array.getIgnoresCase()
    array.setIgnoresCase(False)
    assert not array.getIgnoresCase()

#==================================================================================================

def test_string_pair_array_get_description():
    array = juce.StringPairArray()
    array.set("key", "value")
    description = array.getDescription()
    assert "key" in description
    assert "value" in description

#==================================================================================================

def test_string_pair_array_minimise_storage_overheads():
    array = juce.StringPairArray()
    for i in range(100):
        array.set(f"key{i}", f"value{i}")
    for i in range(50):
        array.remove(i)
    array.minimiseStorageOverheads()
    assert len(array) == 50

#==================================================================================================

def test_string_pair_array_add_map():
    array = juce.StringPairArray()
    map_to_add = {"key1": "value1", "key2": "value2"}
    array.addMap(map_to_add)
    assert array.size() == 2
    assert array["key1"] == "value1"
    assert array["key2"] == "value2"

#==================================================================================================

def test_string_pair_array_add_unordered_map():
    array = juce.StringPairArray()
    unordered_map_to_add = {"key3": "value3", "key4": "value4"}
    array.addUnorderedMap(unordered_map_to_add)
    assert array.size() == 2
    assert array["key3"] == "value3"
    assert array["key4"] == "value4"
