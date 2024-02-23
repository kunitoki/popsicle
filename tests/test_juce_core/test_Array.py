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

def test_array_int_from_list_constructor(create_array):
    arr = create_array(int, [1, 2, 3])
    assert arr.size() == 3
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

#==================================================================================================

def test_array_int_single_element_constructor(create_array):
    arr = create_array(int, 1)
    assert arr.size() == 1
    assert arr[0] == 1

#==================================================================================================

def test_array_int_clear(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.clear()
    assert arr.size() == 0

#==================================================================================================

def test_array_int_clear_quick(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.clearQuick()
    assert arr.size() == 0

#==================================================================================================

def test_array_int_fill(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.fill(9)
    assert all(arr[i] == 9 for i in range(arr.size()))

#==================================================================================================

def test_array_int_size(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.size() == 3

#==================================================================================================

def test_array_int_is_empty(create_array):
    empty_arr = create_array(int, )
    non_empty_arr = create_array(int, 1)
    assert empty_arr.isEmpty()
    assert not non_empty_arr.isEmpty()

#==================================================================================================

def test_array_int_index_of(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.indexOf(2) == 1
    assert arr.indexOf(4) == -1

#==================================================================================================

def test_array_int_contains(create_array):
    arr = create_array(int, 1, 2, 3)
    assert arr.contains(2)
    assert not arr.contains(4)

#==================================================================================================

def test_array_int_add(create_array):
    arr = create_array(int, )
    arr.add(1)
    assert arr.size() == 1
    assert arr[0] == 1

#==================================================================================================

def test_array_int_insert(create_array):
    arr = create_array(int, 1, 3)
    arr.insert(1, 2)
    assert arr.size() == 3
    assert arr[1] == 2

#==================================================================================================

def test_array_int_insert_multiple(create_array):
    arr = create_array(int, 1, 4)
    arr.insertMultiple(1, 2, 2)
    assert arr.size() == 4
    assert arr[1] == 2
    assert arr[2] == 2

#==================================================================================================

def test_array_int_add_if_not_already_there(create_array):
    arr = create_array(int, 1, 2, 3)
    added = arr.addIfNotAlreadyThere(4)
    not_added = arr.addIfNotAlreadyThere(3)
    assert added
    assert not not_added
    assert arr.size() == 4

#==================================================================================================

def test_array_int_set(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.set(1, 9)
    assert arr[1] == 9

#==================================================================================================

def test_array_int_remove(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.remove(1)
    assert arr.size() == 2
    assert arr[1] == 3

#==================================================================================================

def test_array_int_remove_and_return(create_array):
    arr = create_array(int, 1, 2, 3)
    removed = arr.removeAndReturn(1)
    assert removed == 2
    assert arr.size() == 2

#==================================================================================================

def test_array_int_remove_first_matching_value(create_array):
    arr = create_array(int, 1, 2, 2, 3)
    index = arr.removeFirstMatchingValue(2)
    assert index == 1
    assert arr.size() == 3
    assert arr[1] == 2

#==================================================================================================

def test_array_int_remove_all_instances_of(create_array):
    arr = create_array(int, 1, 2, 2, 3)
    count = arr.removeAllInstancesOf(2)
    assert count == 2
    assert arr.size() == 2

#==================================================================================================

def test_array_int_remove_if(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    count = arr.removeIf(lambda x: x % 2 == 0)
    assert count == 2
    assert arr.size() == 3
    assert not arr.contains(2)
    assert not arr.contains(4)

#==================================================================================================

def test_array_int_remove_range(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    arr.removeRange(1, 3)
    assert arr.size() == 2
    assert arr[0] == 1
    assert arr[1] == 5

#==================================================================================================

def test_array_int_swap(create_array):
    arr = create_array(int, 1, 2, 3, 4)
    arr.swap(1, 3)
    assert arr[1] == 4
    assert arr[3] == 2

#==================================================================================================

def test_array_int_move(create_array):
    arr = create_array(int, 1, 2, 3, 4)
    arr.move(1, 3)
    assert arr[2] == 4
    assert arr[3] == 2

#==================================================================================================

def test_array_int_resize(create_array):
    arr = create_array(int, 1, 2, 3)
    arr.resize(5)
    assert arr.size() == 5
    arr.resize(2)
    assert arr.size() == 2
    assert arr[1] == 2

#==================================================================================================

class CustomComparatorSub(juce.Array[int].Comparator):
    def compareElements(self, x, y):
        return x - y

def test_array_int_add_sorted_comparator(create_array):
    arr = create_array(int)

    comparator = CustomComparatorSub()
    arr.addSorted(comparator, 2)
    arr.addSorted(comparator, 1)
    arr.addSorted(comparator, 3)
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

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

def test_array_int_add_using_default_sort(create_array):
    arr = create_array(int)
    arr.addUsingDefaultSort(2)
    arr.addUsingDefaultSort(1)
    arr.addUsingDefaultSort(3)
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

#==================================================================================================

def test_array_int_sort(create_array):
    arr = create_array(int, 3, 1, 2)
    arr.sort()
    assert arr[0] == 1
    assert arr[1] == 2
    assert arr[2] == 3

#==================================================================================================

def test_array_int_minimise_storage_overheads(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    arr.remove(1)
    arr.remove(2)
    assert arr.size() == 3
    arr.minimiseStorageOverheads()
    assert arr.size() == 3

#==================================================================================================

def test_array_int_ensure_storage_allocated(create_array):
    arr = create_array(int, 1, 2, 3, 4, 5)
    assert arr.size() == 5
    arr.ensureStorageAllocated(10)
    assert arr.size() == 5

#==================================================================================================

def test_array_int_get_first(create_array):
    arr = create_array(int, 1, 2, 3)
    first_element = arr.getFirst()
    assert first_element == 1

#==================================================================================================

def test_array_int_get_last(create_array):
    arr = create_array(int, 1, 2, 3)
    last_element = arr.getLast()
    assert last_element == 3

#==================================================================================================

def test_array_int_get_unchecked(create_array):
    arr = create_array(int, 1, 2, 3)
    element = arr.getUnchecked(1)
    assert element == 2

#==================================================================================================

def test_array_int_get_reference(create_array):
    arr = create_array(int, 1, 2, 3)
    ref = arr.getReference(1)
    assert ref == 2

#==================================================================================================

def test_array_int_operator_equals(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 1, 2, 3)
    assert arr1 == arr2

#==================================================================================================

def test_array_int_operator_not_equals(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    assert arr1 != arr2

#==================================================================================================

def test_array_int_add_array(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    arr1.addArray(arr2)
    assert arr1.size() == 6
    assert arr1[3] == 4
    assert arr1[4] == 5
    assert arr1[5] == 6

#==================================================================================================

def test_array_int_swap_with(create_array):
    arr1 = create_array(int, 1, 2, 3)
    arr2 = create_array(int, 4, 5, 6)
    arr1.swapWith(arr2)
    assert arr1.size() == 3
    assert arr2.size() == 3
    assert arr1[0] == 4
    assert arr2[0] == 1
