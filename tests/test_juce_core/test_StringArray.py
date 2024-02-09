import popsicle as juce

#==================================================================================================

def test_constructor_empty():
    string_array = juce.StringArray()
    assert len(string_array) == 0

#==================================================================================================

def test_constructor_copy():
    original = juce.StringArray(["one", "two", "three"])
    copied = juce.StringArray(original)
    assert len(copied) == 3
    assert copied[0] == "one"
    assert copied[1] == "two"
    assert copied[2] == "three"

#==================================================================================================

def test_constructor_list():
    string_array = juce.StringArray(["apple", "orange", "banana"])
    assert len(string_array) == 3
    assert string_array[0] == "apple"
    assert string_array[1] == "orange"
    assert string_array[2] == "banana"

#==================================================================================================

def test_constructor_single():
    string_array = juce.StringArray("one")
    assert len(string_array) == 1
    assert string_array[0] == "one"

#==================================================================================================

def test_constructor_args():
    string_array = juce.StringArray("one", "two", "three")
    assert len(string_array) == 3
    assert string_array[0] == "one"
    assert string_array[1] == "two"
    assert string_array[2] == "three"

#==================================================================================================

def test_comparison_operators():
    array1 = juce.StringArray(["apple", "banana", "orange"])
    array2 = juce.StringArray(["apple", "banana", "orange"])
    array3 = juce.StringArray(["apple", "banana", "kiwi"])

    assert array1 == array2
    assert array1 != array3

#==================================================================================================

def test_size_method():
    string_array = juce.StringArray(["one", "two", "three"])
    assert string_array.size() == 3

#==================================================================================================

def test_is_empty_method():
    empty_array = juce.StringArray()
    non_empty_array = juce.StringArray(["apple", "banana"])

    assert empty_array.isEmpty() is True
    assert non_empty_array.isEmpty() is False

#==================================================================================================

def test_index_operator():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    assert string_array[0] == "apple"
    assert string_array[1] == "banana"
    assert string_array[2] == "orange"

#==================================================================================================

def test_swap_with_method():
    array1 = juce.StringArray(["one", "two", "three"])
    array2 = juce.StringArray(["apple", "banana"])

    array1.swapWith(array2)

    assert len(array1) == 2
    assert array1[0] == "apple"
    assert array1[1] == "banana"

    assert len(array2) == 3
    assert array2[0] == "one"
    assert array2[1] == "two"
    assert array2[2] == "three"

#==================================================================================================

def test_contains_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])

    assert string_array.contains("banana") is True
    assert string_array.contains("grape") is False

#==================================================================================================

def test_index_of_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])

    assert string_array.indexOf("banana") == 1
    assert string_array.indexOf("grape") == -1

#==================================================================================================

def test_add_method():
    string_array = juce.StringArray()
    string_array.add("apple")
    assert len(string_array) == 1
    assert string_array[0] == "apple"

#==================================================================================================

def test_insert_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.insert(1, "grape")
    assert len(string_array) == 4
    assert string_array[1] == "grape"
    assert string_array[2] == "banana"

#==================================================================================================

def test_set_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.set(1, "grape")
    assert len(string_array) == 3
    assert string_array[1] == "grape"

#==================================================================================================

def test_add_if_not_already_there_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    result = string_array.addIfNotAlreadyThere("grape")
    assert result is True
    assert len(string_array) == 4
    assert string_array[3] == "grape"

    # Adding an existing string should return False
    result = string_array.addIfNotAlreadyThere("banana")
    assert result is False
    assert len(string_array) == 4

#==================================================================================================

def test_add_array_method():
    string_array1 = juce.StringArray(["apple", "banana"])
    string_array2 = juce.StringArray(["orange", "grape"])
    string_array1.addArray(string_array2)
    assert len(string_array1) == 4
    assert string_array1[2] == "orange"
    assert string_array1[3] == "grape"

#==================================================================================================

def test_merge_array_method():
    string_array1 = juce.StringArray(["apple", "banana"])
    string_array2 = juce.StringArray(["orange", "banana"])
    string_array1.mergeArray(string_array2)
    assert len(string_array1) == 3
    assert string_array1[2] == "orange"

    string_array1 = juce.StringArray(["apple", "BANANA"])
    string_array2 = juce.StringArray(["orange", "banana"])
    string_array1.mergeArray(string_array2, True)
    assert len(string_array1) == 3
    assert string_array1[0] == "apple"
    assert string_array1[1] == "BANANA"
    assert string_array1[2] == "orange"

    string_array1 = juce.StringArray(["apple", "BANANA"])
    string_array2 = juce.StringArray(["orange", "banana"])
    string_array1.mergeArray(string_array2, False)
    assert len(string_array1) == 4
    assert string_array1[0] == "apple"
    assert string_array1[1] == "BANANA"
    assert string_array1[2] == "orange"
    assert string_array1[3] == "banana"

#==================================================================================================

def test_remove_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.remove(1)
    assert len(string_array) == 2
    assert string_array[1] == "orange"

#==================================================================================================

def test_remove_string_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.removeString("banana")
    assert len(string_array) == 2
    assert string_array[1] == "orange"

#==================================================================================================

def test_remove_range_method():
    string_array = juce.StringArray(["apple", "banana", "orange", "grape"])
    string_array.removeRange(1, 2)
    assert len(string_array) == 2
    assert string_array[0] == "apple"
    assert string_array[1] == "grape"

#==================================================================================================

def test_remove_duplicates_method():
    string_array = juce.StringArray(["apple", "banana", "orange", "banana", "grape"])
    string_array.removeDuplicates(False)
    assert len(string_array) == 4
    assert string_array[1] == "banana"
    assert string_array[3] == "grape"

#==================================================================================================

def test_remove_empty_strings_method():
    string_array = juce.StringArray(["apple", "", "banana", "  ", "orange", ""])
    string_array.removeEmptyStrings()
    assert len(string_array) == 3
    assert string_array[0] == "apple"
    assert string_array[1] == "banana"
    assert string_array[2] == "orange"

#==================================================================================================

def test_move_method():
    string_array = juce.StringArray(["apple", "banana", "orange", "grape"])
    string_array.move(1, 2)
    assert len(string_array) == 4
    assert string_array[1] == "orange"
    assert string_array[2] == "banana"

#==================================================================================================

def test_trim_method():
    string_array = juce.StringArray(["  apple  ", "  banana  ", "  orange  ", "  "])
    string_array.trim()
    assert len(string_array) == 4
    assert string_array[0] == "apple"
    assert string_array[1] == "banana"
    assert string_array[2] == "orange"
    assert string_array[3] == ""

#==================================================================================================

def test_join_into_string_method():
    string_array = juce.StringArray()
    result = string_array.joinIntoString(", ")
    assert result == ""

    string_array = juce.StringArray(["apple", "banana", "orange"])
    result = string_array.joinIntoString(", ")
    assert result == "apple, banana, orange"

#==================================================================================================

def test_sort_method():
    string_array = juce.StringArray(["orange", "banana", "apple", "grape"])
    string_array.sort(False)
    assert len(string_array) == 4
    assert string_array[0] == "apple"
    assert string_array[3] == "orange"

    string_array = juce.StringArray(["Apple", "orange", "banana", "apple"])
    string_array.sort(True)
    assert len(string_array) == 4
    assert string_array[0] == "Apple"
    assert string_array[3] == "orange"

    string_array = juce.StringArray(["orange", "banana", "apple", "Apple"])
    string_array.sort(False)
    assert len(string_array) == 4
    assert string_array[0] == "Apple"
    assert string_array[3] == "orange"

#==================================================================================================

def test_sort_natural_method():
    string_array = juce.StringArray(["apple 10", "apple 2", "banana 1", "banana 10"])
    string_array.sortNatural()
    assert len(string_array) == 4
    assert string_array[0] == "apple 2"
    assert string_array[3] == "banana 10"

#==================================================================================================

def test_ensure_storage_allocated_method():
    string_array = juce.StringArray()
    string_array.ensureStorageAllocated(5)
    assert len(string_array) == 0  # No change in size, only storage allocation

#==================================================================================================

def test_minimise_storage_overheads_method():
    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.ensureStorageAllocated(10)
    string_array.minimiseStorageOverheads()
    assert len(string_array) == 3  # No change in size, only reduced storage overhead

#==================================================================================================

def test_from_tokens_method():
    string_to_tokenise = "apple banana orange"
    result = juce.StringArray.fromTokens(string_to_tokenise, preserveQuotedStrings=False)
    assert len(result) == 3
    assert result[0] == "apple"
    assert result[2] == "orange"

#==================================================================================================

def test_from_tokens_methodwith_break_character():
    string_to_tokenise = "apple, banana, orange"
    result = juce.StringArray.fromTokens(string_to_tokenise, ",", "")
    assert len(result) == 3
    assert result[0] == "apple"
    assert result[1] == " banana"
    assert result[2] == " orange"

    result = juce.StringArray.fromTokens(string_to_tokenise, ", ", "")
    assert len(result) == 5
    assert result[0] == "apple"
    assert result[1] == ""
    assert result[2] == "banana"
    assert result[3] == ""
    assert result[4] == "orange"

#==================================================================================================

def test_from_tokens_method_with_quotes():
    string_to_tokenise = 'apple "banana and kiwi" orange'
    result = juce.StringArray.fromTokens(string_to_tokenise, preserveQuotedStrings=True)
    assert len(result) == 3
    assert result[0] == "apple"
    assert result[1] == "\"banana and kiwi\""
    assert result[2] == "orange"

#==================================================================================================

def test_from_lines_method():
    result = juce.StringArray.fromLines("")
    assert len(result) == 0

    result = juce.StringArray.fromLines("\napple\n\nbanana")
    assert len(result) == 4
    assert result[0] == ""
    assert result[1] == "apple"
    assert result[2] == ""
    assert result[3] == "banana"

    result = juce.StringArray.fromLines("apple\nbanana\norange")
    assert len(result) == 3
    assert result[0] == "apple"
    assert result[1] == "banana"
    assert result[2] == "orange"

#==================================================================================================

def test_clear_method():
    string_array = juce.StringArray()
    string_array.clear()
    assert len(string_array) == 0

    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.clear()
    assert len(string_array) == 0

#==================================================================================================

def test_clear_quick_method():
    string_array = juce.StringArray()
    string_array.clearQuick()
    assert len(string_array) == 0

    string_array = juce.StringArray(["apple", "banana", "orange"])
    string_array.clearQuick()
    assert len(string_array) == 0
