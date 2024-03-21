import pytest

import popsicle as juce

#==================================================================================================

@pytest.fixture
def create_array():
    def _create_array(type, *args):
        return juce.Array[type](*args)
    return _create_array

#==================================================================================================

@pytest.mark.parametrize("array_type", [bool, int, float])
def test_array_default_constructor(array_type, create_array):
    arr = create_array(array_type)
    assert arr.size() == 0

#==================================================================================================

def test_array_bool_copy_constructor(create_array):
    original = create_array(bool, True, True, False)
    copy = create_array(bool, original)
    assert copy.size() == 3
    assert copy[0] == True
    assert copy[1] == True
    assert copy[2] == False

def test_array_int_copy_constructor(create_array):
    original = create_array(int, 1, 2, 3)
    copy = create_array(int, original)
    assert copy.size() == 3
    assert copy[0] == 1
    assert copy[1] == 2
    assert copy[2] == 3

def test_array_float_copy_constructor(create_array):
    original = create_array(float, 1.0, 2.0, 3.0)
    copy = create_array(float, original)
    assert copy.size() == 3
    assert copy[0] == pytest.approx(1.0)
    assert copy[1] == pytest.approx(2.0)
    assert copy[2] == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_from_list_constructor(create_array):
    arr = create_array(bool, [True, False, True])
    assert arr.size() == 3
    assert arr[0] == True
    assert arr[1] == False
    assert arr[2] == True

def test_array_int_from_list_constructor(create_array):
    arr = create_array(int, [1, 2, 3])
    assert arr.size() == 3
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

def test_array_float_from_list_constructor(create_array):
    arr = create_array(float, [1.0, 2.0, 3.0])
    assert arr.size() == 3
    assert arr[0] == pytest.approx(1.0)
    assert arr[1] == pytest.approx(2.0)
    assert arr[2] == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_single_element_constructor(create_array):
    arr = create_array(bool, False)
    assert arr.size() == 1
    assert arr[0] == False

def test_array_int_single_element_constructor(create_array):
    arr = create_array(int, 1)
    assert arr.size() == 1
    assert arr[0] == 1

def test_array_float_single_element_constructor(create_array):
    arr = create_array(float, 1.0)
    assert arr.size() == 1
    assert arr[0] == pytest.approx(1.0)

#==================================================================================================

def test_array_bool_clear(create_array):
    arr = create_array(bool, True, False, True)
    arr.clear()
    assert arr.size() == 0

def test_array_int_clear(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.clear()
    assert arr.size() == 0

def test_array_float_clear(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.clear()
    assert arr.size() == 0

#==================================================================================================

def test_array_bool_clear_quick(create_array):
    arr = create_array(bool, True, False, True)
    arr.clearQuick()
    assert arr.size() == 0

def test_array_int_clear_quick(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.clearQuick()
    assert arr.size() == 0

def test_array_float_clear_quick(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.clearQuick()
    assert arr.size() == 0

#==================================================================================================

def test_array_bool_fill(create_array):
    arr = create_array(bool, True, False, True)
    arr.fill(False)
    assert all(arr[i] == False for i in range(arr.size()))

def test_array_int_fill(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.fill(9)
    assert all(arr[i] == 9 for i in range(arr.size()))

def test_array_float_fill(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.fill(9.0)
    assert all(arr[i] == pytest.approx(9.0) for i in range(arr.size()))

#==================================================================================================

def test_array_bool_size(create_array):
    arr = create_array(bool, True, False, True)
    assert arr.size() == 3

def test_array_int_size(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.size() == 3

def test_array_float_size(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    assert arr.size() == 3

#==================================================================================================

def test_array_bool_is_empty(create_array):
    empty_arr = create_array(bool)
    non_empty_arr = create_array(bool, True)
    assert empty_arr.isEmpty()
    assert not non_empty_arr.isEmpty()

def test_array_int_is_empty(create_array):
    empty_arr = create_array(int, )
    non_empty_arr = create_array(int, 1)
    assert empty_arr.isEmpty()
    assert not non_empty_arr.isEmpty()

def test_array_float_is_empty(create_array):
    empty_arr = create_array(float)
    non_empty_arr = create_array(float, 1.0)
    assert empty_arr.isEmpty()
    assert not non_empty_arr.isEmpty()

#==================================================================================================

def test_array_bool_index_of(create_array):
    arr = create_array(bool, True, False, True)
    assert arr.indexOf(True) == 0
    assert arr.indexOf(False) == 1

def test_array_int_index_of(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.indexOf(2) == 1
    assert arr.indexOf(4) == -1

def test_array_float_index_of(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    assert arr.indexOf(2.0) == 1
    assert arr.indexOf(4.0) == -1

#==================================================================================================

def test_array_bool_contains(create_array):
    arr = create_array(bool, True, True, True)
    assert arr.contains(True)
    assert not arr.contains(False)

def test_array_int_contains(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.contains(2)
    assert not arr.contains(4)

def test_array_float_contains(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    assert arr.contains(2.0)
    assert not arr.contains(4.0)

#==================================================================================================

def test_array_bool_add(create_array):
    arr = create_array(bool)
    arr.add(False)
    assert arr.size() == 1
    assert arr[0] == False

def test_array_int_add(create_array):
    arr = create_array(int, )
    arr.add(1)
    assert arr.size() == 1
    assert arr[0] == 1

def test_array_float_add(create_array):
    arr = create_array(float)
    arr.add(1.0)
    assert arr.size() == 1
    assert arr[0] == pytest.approx(1.0)

#==================================================================================================

def test_array_bool_insert(create_array):
    arr = create_array(bool, True, False)
    arr.insert(1, True)
    assert arr.size() == 3
    assert arr[1] == True

def test_array_int_insert(create_array):
    arr = create_array(int, 1, 3)
    arr.insert(1, 2)
    assert arr.size() == 3
    assert arr[1] == 2

def test_array_float_insert(create_array):
    arr = create_array(float, 1.0, 3.0)
    arr.insert(1, 2.0)
    assert arr.size() == 3
    assert arr[1] == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_insert_multiple(create_array):
    arr = create_array(bool, True, False)
    arr.insertMultiple(1, False, 2)
    assert arr.size() == 4
    assert arr[1] == False
    assert arr[2] == False

def test_array_int_insert_multiple(create_array):
    arr = create_array(int, 1, 4)
    arr.insertMultiple(1, 2, 2)
    assert arr.size() == 4
    assert arr[1] == 2
    assert arr[2] == 2

def test_array_float_insert_multiple(create_array):
    arr = create_array(float, 1.0, 4.0)
    arr.insertMultiple(1, 2.0, 2)
    assert arr.size() == 4
    assert arr[1] == pytest.approx(2.0)
    assert arr[2] == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_add_if_not_already_there(create_array):
    arr = create_array(bool, True, False)
    added = arr.addIfNotAlreadyThere(False)  # Assuming adding duplicates is not allowed
    not_added = arr.addIfNotAlreadyThere(True)
    assert not added
    assert not not_added
    assert arr.size() == 2  # No change, as both elements were already there

def test_array_int_add_if_not_already_there(create_array):
    arr = create_array(int, 1, 2, 3)
    added = arr.addIfNotAlreadyThere(4)
    not_added = arr.addIfNotAlreadyThere(3)
    assert added
    assert not not_added
    assert arr.size() == 4

def test_array_float_add_if_not_already_there(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    added = arr.addIfNotAlreadyThere(4.0)
    not_added = arr.addIfNotAlreadyThere(3.0)
    assert added
    assert not not_added
    assert arr.size() == 4

#==================================================================================================

def test_array_bool_set(create_array):
    arr = create_array(bool, True, False, True)
    arr.set(1, True)
    assert arr[1] == True

def test_array_int_set(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.set(1, 9)
    assert arr[1] == 9

def test_array_float_set(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.set(1, 9.0)
    assert arr[1] == pytest.approx(9.0)

#==================================================================================================

def test_array_bool_remove(create_array):
    arr = create_array(bool, True, False, True)
    arr.remove(1)
    assert arr.size() == 2
    assert arr[1] == True

def test_array_int_remove(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.remove(1)
    assert arr.size() == 2
    assert arr[1] == 3

def test_array_float_remove(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.remove(1)
    assert arr.size() == 2
    assert arr[1] == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_remove_and_return(create_array):
    arr = create_array(bool, True, False, True)
    removed = arr.removeAndReturn(1)
    assert removed == False
    assert arr.size() == 2

def test_array_int_remove_and_return(create_array):
    arr = create_array(int, 1, 2, 3)
    removed = arr.removeAndReturn(1)
    assert removed == 2
    assert arr.size() == 2

def test_array_float_remove_and_return(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    removed = arr.removeAndReturn(1)
    assert removed == pytest.approx(2.0)
    assert arr.size() == 2

#==================================================================================================

def test_array_bool_remove_first_matching_value(create_array):
    arr = create_array(bool, True, False, False, True)
    index = arr.removeFirstMatchingValue(False)
    assert index == 1
    assert arr.size() == 3
    assert arr[1] == False

def test_array_int_remove_first_matching_value(create_array):
    arr = create_array(int, 1, 2, 2, 3)
    index = arr.removeFirstMatchingValue(2)
    assert index == 1
    assert arr.size() == 3
    assert arr[1] == 2

def test_array_float_remove_first_matching_value(create_array):
    arr = create_array(float, 1.0, 2.0, 2.0, 3.0)
    index = arr.removeFirstMatchingValue(2.0)
    assert index == 1
    assert arr.size() == 3
    assert arr[1] == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_remove_all_instances_of(create_array):
    arr = create_array(bool, True, False, False, True)
    count = arr.removeAllInstancesOf(False)
    assert count == 2
    assert arr.size() == 2

def test_array_int_remove_all_instances_of(create_array):
    arr = create_array(int, 1, 2, 2, 3)
    count = arr.removeAllInstancesOf(2)
    assert count == 2
    assert arr.size() == 2

def test_array_float_remove_all_instances_of(create_array):
    arr = create_array(float, 1.0, 2.0, 2.0, 3.0)
    count = arr.removeAllInstancesOf(2.0)
    assert count == 2
    assert arr.size() == 2

#==================================================================================================

def test_array_bool_remove_if(create_array):
    arr = create_array(bool, True, False, True, False, True)
    count = arr.removeIf(lambda x: not x)  # Remove False values
    assert count == 2
    assert arr.size() == 3
    assert not arr.contains(False)

def test_array_int_remove_if(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    count = arr.removeIf(lambda x: x % 2 == 0)
    assert count == 2
    assert arr.size() == 3
    assert not arr.contains(2)
    assert not arr.contains(4)

def test_array_float_remove_if(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    count = arr.removeIf(lambda x: x % 2 == 0)  # Remove even numbers
    assert count == 2
    assert arr.size() == 3
    assert not arr.contains(2.0)
    assert not arr.contains(4.0)

#==================================================================================================

def test_array_bool_remove_range(create_array):
    arr = create_array(bool, True, False, True, False, True)
    arr.removeRange(1, 3)
    assert arr.size() == 2
    assert arr[0] == True
    assert arr[1] == True

def test_array_int_remove_range(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    arr.removeRange(1, 3)
    assert arr.size() == 2
    assert arr[0] == 1
    assert arr[1] == 5

def test_array_float_remove_range(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    arr.removeRange(1, 3)
    assert arr.size() == 2
    assert arr[0] == pytest.approx(1.0)
    assert arr[1] == pytest.approx(5.0)

#==================================================================================================

def test_array_bool_swap(create_array):
    arr = create_array(bool, True, False, True, True)
    arr.swap(1, 3)
    assert arr[1] == True
    assert arr[3] == False

def test_array_int_swap(create_array):
    arr = create_array(int, 1, 2, 3, 4)
    arr.swap(1, 3)
    assert arr[1] == 4
    assert arr[3] == 2

def test_array_float_swap(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0)
    arr.swap(1, 3)
    assert arr[1] == pytest.approx(4.0)
    assert arr[3] == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_move(create_array):
    arr = create_array(bool, True, False, True, False)
    arr.move(0, 3)
    assert arr[0] == False
    assert arr[3] == True

def test_array_int_move(create_array):
    arr = create_array(int, 1, 2, 3, 4)
    arr.move(0, 3)
    assert arr[0] == 2
    assert arr[3] == 1

def test_array_float_move(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0)
    arr.move(0, 3)
    assert arr[0] == pytest.approx(2.0)
    assert arr[3] == pytest.approx(1.0)

#==================================================================================================

def test_array_bool_resize(create_array):
    arr = create_array(bool, True, False, True)
    arr.resize(5)
    assert arr.size() == 5
    arr.resize(2)
    assert arr.size() == 2
    assert arr[1] == False

def test_array_int_resize(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.resize(5)
    assert arr.size() == 5
    arr.resize(2)
    assert arr.size() == 2
    assert arr[1] == 2

def test_array_float_resize(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    arr.resize(5)
    assert arr.size() == 5
    arr.resize(2)
    assert arr.size() == 2
    assert arr[1] == pytest.approx(2.0)

#==================================================================================================

class CustomComparatorBool(juce.Array[bool].Comparator):
    def compareElements(self, x, y):
        if x == y: return 0
        elif x < y: return -1
        else: return 1

def test_array_bool_add_sorted_comparator(create_array):
    arr = create_array(bool)

    comparator = CustomComparatorBool()
    arr.addSorted(comparator, True)
    arr.addSorted(comparator, True)
    arr.addSorted(comparator, False)
    assert arr[0] == False
    assert arr[1] == True
    assert arr[2] == True

class CustomComparatorInt(juce.Array[int].Comparator):
    def compareElements(self, x, y):
        if x == y: return 0
        elif x < y: return -1
        else: return 1

def test_array_int_add_sorted_comparator(create_array):
    arr = create_array(int)

    comparator = CustomComparatorInt()
    arr.addSorted(comparator, 2)
    arr.addSorted(comparator, 1)
    arr.addSorted(comparator, 3)
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

class CustomComparatorFloat(juce.Array[float].Comparator):
    def compareElements(self, x, y):
        if x == y: return 0
        elif x < y: return -1
        else: return 1

def test_array_float_add_sorted_comparator(create_array):
    arr = create_array(float)

    comparator = CustomComparatorFloat()
    arr.addSorted(comparator, 2.1)
    arr.addSorted(comparator, 1.1)
    arr.addSorted(comparator, 3.1)
    assert arr[0] == pytest.approx(1.1)
    assert arr[1] == pytest.approx(2.1)
    assert arr[2] == pytest.approx(3.1)

#==================================================================================================
@pytest.mark.skip(reason="Temporarily not available")
def test_array_int_add_sorted_lambda(create_array):
    arr = create_array(int)
    comparator = lambda x, y: x - y
    arr.addSorted(comparator, 2)
    arr.addSorted(comparator, 1)
    arr.addSorted(comparator, 3)
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

#==================================================================================================

def test_array_bool_add_using_default_sort(create_array):
    arr = create_array(bool)
    arr.addUsingDefaultSort(False)
    arr.addUsingDefaultSort(True)
    arr.addUsingDefaultSort(False)  # Adding a second False to see sorting effect
    assert arr[0] == False
    assert arr[1] == False  # Verify that both Falses are at the beginning
    assert arr[2] == True

def test_array_int_add_using_default_sort(create_array):
    arr = create_array(int)
    arr.addUsingDefaultSort(2)
    arr.addUsingDefaultSort(1)
    arr.addUsingDefaultSort(3)
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

def test_array_float_add_using_default_sort(create_array):
    arr = create_array(float)
    arr.addUsingDefaultSort(2.0)
    arr.addUsingDefaultSort(1.0)
    arr.addUsingDefaultSort(3.0)
    assert arr[0] == pytest.approx(1.0)
    assert arr[1] == pytest.approx(2.0)
    assert arr[2] == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_sort(create_array):
    arr = create_array(bool, True, False, True, False)
    arr.sort()
    assert arr[0] == False
    assert arr[1] == False  # Verify that both Falses are at the beginning after sort
    assert arr[2] == True
    assert arr[3] == True

def test_array_int_sort(create_array):
    arr = create_array(int, 3, 1, 2)
    arr.sort()
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

def test_array_float_sort(create_array):
    arr = create_array(float, 3.0, 1.0, 2.0)
    arr.sort()
    assert arr[0] == pytest.approx(1.0)
    assert arr[1] == pytest.approx(2.0)
    assert arr[2] == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_minimise_storage_overheads(create_array):
    arr = create_array(bool, True, False, True, False, True)
    arr.remove(1)  # Remove one instance of False
    arr.remove(2)  # Remove another instance of True, expecting index adjustment
    assert arr.size() == 3
    arr.minimiseStorageOverheads()
    assert arr.size() == 3  # Size remains unchanged; method optimizes storage without altering size

def test_array_int_minimise_storage_overheads(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    arr.remove(1)
    arr.remove(2)
    assert arr.size() == 3
    arr.minimiseStorageOverheads()
    assert arr.size() == 3

def test_array_float_minimise_storage_overheads(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    arr.remove(1)  # Remove 2.0
    arr.remove(2)  # Remove what is now at index 2, originally 4.0
    assert arr.size() == 3
    arr.minimiseStorageOverheads()
    assert arr.size() == 3

#==================================================================================================

def test_array_bool_ensure_storage_allocated(create_array):
    arr = create_array(bool, True, False, True)
    assert arr.size() == 3
    arr.ensureStorageAllocated(10)  # Ensure space for 10 elements is allocated
    assert arr.size() == 3  # Size should remain unchanged

def test_array_int_ensure_storage_allocated(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    assert arr.size() == 5
    arr.ensureStorageAllocated(10)
    assert arr.size() == 5

def test_array_float_ensure_storage_allocated(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    assert arr.size() == 3
    arr.ensureStorageAllocated(10)
    assert arr.size() == 3

#==================================================================================================

def test_array_bool_get_first(create_array):
    arr = create_array(bool, False, True, True)
    first_element = arr.getFirst()
    assert first_element == False

def test_array_int_get_first(create_array):
    arr = create_array(int, 1, 2, 3)
    first_element = arr.getFirst()
    assert first_element == 1


def test_array_float_get_first(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    first_element = arr.getFirst()
    assert first_element == pytest.approx(1.0)

#==================================================================================================

def test_array_bool_get_last(create_array):
    arr = create_array(bool, True, False, True)
    last_element = arr.getLast()
    assert last_element == True

def test_array_int_get_last(create_array):
    arr = create_array(int, 1, 2, 3)
    last_element = arr.getLast()
    assert last_element == 3

def test_array_float_get_last(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    last_element = arr.getLast()
    assert last_element == pytest.approx(3.0)

#==================================================================================================

def test_array_bool_get_unchecked(create_array):
    arr = create_array(bool, True, False, True)
    element = arr.getUnchecked(1)
    assert element == False

def test_array_int_get_unchecked(create_array):
    arr = create_array(int, 1, 2, 3)
    element = arr.getUnchecked(1)
    assert element == 2

def test_array_float_get_unchecked(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    element = arr.getUnchecked(1)
    assert element == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_get_reference(create_array):
    arr = create_array(bool, True, False, True)
    ref = arr.getReference(1)
    assert ref == False

def test_array_int_get_reference(create_array):
    arr = create_array(int, 1, 2, 3)
    ref = arr.getReference(1)
    assert ref == 2

def test_array_float_get_reference(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    ref = arr.getReference(1)
    assert ref == pytest.approx(2.0)

#==================================================================================================

def test_array_bool_operator_equals(create_array):
    arr1 = create_array(bool, True, False, True)
    arr2 = create_array(bool, True, False, True)
    assert arr1 == arr2

def test_array_int_operator_equals(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 1, 2, 3)
    assert arr1 == arr2

def test_array_float_operator_equals(create_array):
    arr1 = create_array(float, 1.0, 2.0, 3.0)
    arr2 = create_array(float, 1.0, 2.0, 3.0)
    assert arr1 == arr2

#==================================================================================================

def test_array_bool_operator_not_equals(create_array):
    arr1 = create_array(bool, True, False, True)
    arr2 = create_array(bool, False, True, False)
    assert arr1 != arr2

def test_array_int_operator_not_equals(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    assert arr1 != arr2

def test_array_float_operator_not_equals(create_array):
    arr1 = create_array(float, 1.0, 2.0, 3.0)
    arr2 = create_array(float, 4.0, 5.0, 6.0)
    assert arr1 != arr2

#==================================================================================================

def test_array_bool_add_array(create_array):
    arr1 = create_array(bool, True, False, True)
    arr2 = create_array(bool, False, True, False)
    arr1.addArray(arr2)
    assert arr1.size() == 6
    assert arr1[3] == False
    assert arr1[4] == True

def test_array_int_add_array(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    arr1.addArray(arr2)
    assert arr1.size() == 6
    assert arr1[3] == 4
    assert arr1[4] == 5
    assert arr1[5] == 6

def test_array_float_add_array(create_array):
    arr1 = create_array(float, 1.0, 2.0, 3.0)
    arr2 = create_array(float, 4.0, 5.0, 6.0)
    arr1.addArray(arr2)
    assert arr1.size() == 6
    assert arr1[3] == pytest.approx(4.0)
    assert arr1[4] == pytest.approx(5.0)

#==================================================================================================

def test_array_bool_swap_with(create_array):
    arr1 = create_array(bool, True, False, True)
    arr2 = create_array(bool, False, True, False)
    arr1.swapWith(arr2)
    assert arr1.size() == 3
    assert arr2.size() == 3
    assert arr1[0] == False
    assert arr2[0] == True

def test_array_int_swap_with(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    arr1.swapWith(arr2)
    assert arr1.size() == 3
    assert arr2.size() == 3
    assert arr1[0] == 4
    assert arr2[0] == 1

def test_array_float_swap_with(create_array):
    arr1 = create_array(float, 1.0, 2.0, 3.0)
    arr2 = create_array(float, 4.0, 5.0, 6.0)
    arr1.swapWith(arr2)
    assert arr1.size() == 3
    assert arr2.size() == 3
    assert arr1[0] == pytest.approx(4.0)
    assert arr2[0] == pytest.approx(1.0)

#==================================================================================================

def test_array_bool_remove_last(create_array):
    arr = create_array(bool, True, False, True, False, True)
    arr.removeLast(2)  # Remove the last two elements
    assert arr.size() == 3
    assert arr[2] == True  # The last remaining element is True

def test_array_int_remove_last(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    arr.removeLast(2)
    assert arr.size() == 3
    assert arr[2] == 3

def test_array_float_remove_last(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    arr.removeLast(2)  # Remove the last two elements
    assert arr.size() == 3
    assert arr[2] == pytest.approx(3.0)  # The last remaining element is 3.0

#==================================================================================================

def test_array_bool_remove_values_in(create_array):
    arr = create_array(bool, True, False, True, False, True)
    other_arr = create_array(bool, False)
    arr.removeValuesIn(other_arr)
    assert arr.size() == 3
    assert arr.contains(True)
    assert not arr.contains(False)  # After removal, False should not be present

def test_array_int_remove_values_in(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    other_arr = create_array(int, 2, 4)
    arr.removeValuesIn(other_arr)
    assert arr.size() == 3
    assert arr.contains(1)
    assert arr.contains(3)
    assert arr.contains(5)

def test_array_float_remove_values_in(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    other_arr = create_array(float, 2.0, 4.0)
    arr.removeValuesIn(other_arr)
    assert arr.size() == 3
    assert arr.contains(1.0)
    assert arr.contains(3.0)
    assert arr.contains(5.0)
    assert not arr.contains(2.0)
    assert not arr.contains(4.0)

#==================================================================================================

def test_array_bool_remove_values_not_in(create_array):
    arr = create_array(bool, True, False, True, False, True)
    other_arr = create_array(bool, True)
    arr.removeValuesNotIn(other_arr)
    assert arr.size() == 3
    assert arr.contains(True)
    assert not arr.contains(False)  # False should be removed since it's not in other_arr

def test_array_int_remove_values_not_in(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    other_arr = create_array(int, 2, 4)
    arr.removeValuesNotIn(other_arr)
    assert arr.size() == 2
    assert arr.contains(2)
    assert arr.contains(4)

def test_array_float_remove_values_not_in(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0, 4.0, 5.0)
    other_arr = create_array(float, 2.0, 4.0)
    arr.removeValuesNotIn(other_arr)
    assert arr.size() == 2
    assert arr.contains(2.0)
    assert arr.contains(4.0)
    assert not arr.contains(1.0)
    assert not arr.contains(3.0)
    assert not arr.contains(5.0)

#==================================================================================================

def test_array_bool_begin_end(create_array):
    arr = create_array(bool, True, False, True)
    elements = [x for x in arr]  # Assuming direct iteration over Array is supported
    assert elements == [True, False, True]

def test_array_int_begin_end(create_array):
    arr = create_array(int, 1, 2, 3)
    elements = [x for x in arr]  # Assuming direct iteration over Array is supported
    assert elements == [1, 2, 3]

def test_array_float_begin_end(create_array):
    arr = create_array(float, 1.0, 2.0, 3.0)
    elements = [x for x in arr]  # Assuming direct iteration over Array is supported
    assert elements == [pytest.approx(1.0), pytest.approx(2.0), pytest.approx(3.0)]
