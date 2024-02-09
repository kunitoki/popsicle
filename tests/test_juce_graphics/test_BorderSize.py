from math import isclose

import popsicle as juce

#==================================================================================================

def test_construct_default():
    border = juce.BorderSize[int]()
    assert border.getTop() == 0
    assert border.getLeft() == 0
    assert border.getBottom() == 0
    assert border.getRight() == 0
    assert border.isEmpty()

    border = juce.BorderSize[float]()
    assert isclose(border.getTop(), 0)
    assert isclose(border.getLeft(), 0)
    assert isclose(border.getBottom(), 0)
    assert isclose(border.getRight(), 0)
    assert border.isEmpty()

#==================================================================================================

def test_construct_with_values():
    border = juce.BorderSize[int](2, 3, 4, 5)
    assert border.getTop() == 2
    assert border.getLeft() == 3
    assert border.getBottom() == 4
    assert border.getRight() == 5
    assert not border.isEmpty()

    border = juce.BorderSize[float](2, 3, 4, 5)
    assert isclose(border.getTop(), 2)
    assert isclose(border.getLeft(), 3)
    assert isclose(border.getBottom(), 4)
    assert isclose(border.getRight(), 5)
    assert not border.isEmpty()

#==================================================================================================

def test_construct_with_single_value():
    border = juce.BorderSize[int](7)
    assert border.getTop() == 7
    assert border.getLeft() == 7
    assert border.getBottom() == 7
    assert border.getRight() == 7
    assert not border.isEmpty()

    border = juce.BorderSize[float](7)
    assert isclose(border.getTop(), 7)
    assert isclose(border.getLeft(), 7)
    assert isclose(border.getBottom(), 7)
    assert isclose(border.getRight(), 7)
    assert not border.isEmpty()

#==================================================================================================

def test_get_top_and_bottom():
    border = juce.BorderSize[int](2, 3, 4, 5)
    assert border.getTopAndBottom() == 6

    border = juce.BorderSize[float](2, 3, 4, 5)
    assert isclose(border.getTopAndBottom(), 6)

#==================================================================================================

def test_get_left_and_right():
    border = juce.BorderSize[int](2, 3, 4, 5)
    assert border.getLeftAndRight() == 8

    border = juce.BorderSize[float](2, 3, 4, 5)
    assert isclose(border.getLeftAndRight(), 8)

#==================================================================================================

def test_set_top():
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.setTop(10)
    assert border.getTop() == 10

    border = juce.BorderSize[float](2, 3, 4, 5)
    border.setTop(10)
    assert isclose(border.getTop(), 10)

#==================================================================================================

def test_set_left():
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.setLeft(20)
    assert border.getLeft() == 20

    border = juce.BorderSize[float](2, 3, 4, 5)
    border.setLeft(20)
    assert isclose(border.getLeft(), 20)

#==================================================================================================

def test_set_bottom():
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.setBottom(30)
    assert border.getBottom() == 30

    border = juce.BorderSize[float](2, 3, 4, 5)
    border.setBottom(30)
    assert isclose(border.getBottom(), 30)

#==================================================================================================

def test_set_right():
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.setRight(40)
    assert border.getRight() == 40

    border = juce.BorderSize[float](2, 3, 4, 5)
    border.setRight(40)
    assert isclose(border.getRight(), 40)

#==================================================================================================

def test_subtract_from_rectangle():
    original_rect = juce.Rectangle[int](10, 20, 30, 40)
    border = juce.BorderSize[int](2, 3, 4, 5)
    result_rect = border.subtractedFrom(original_rect)
    assert result_rect == juce.Rectangle[int](13, 22, 22, 34)

    original_rect = juce.Rectangle[float](10, 20, 30, 40)
    border = juce.BorderSize[float](2, 3, 4, 5)
    result_rect = border.subtractedFrom(original_rect)
    assert result_rect == juce.Rectangle[float](13, 22, 22, 34)

#==================================================================================================

def test_subtract_from_rectangle_in_place():
    original_rect = juce.Rectangle[int](10, 20, 30, 40)
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.subtractFrom(original_rect)
    assert original_rect == juce.Rectangle[int](13, 22, 22, 34)

    original_rect = juce.Rectangle[float](10, 20, 30, 40)
    border = juce.BorderSize[float](2, 3, 4, 5)
    border.subtractFrom(original_rect)
    assert original_rect == juce.Rectangle[float](13, 22, 22, 34)

#==================================================================================================

def test_add_to_rectangle():
    original_rect = juce.Rectangle[int](10, 20, 30, 40)
    border = juce.BorderSize[int](2, 3, 4, 5)
    result_rect = border.addedTo(original_rect)
    assert result_rect == juce.Rectangle[int](7, 18, 38, 46)

    original_rect = juce.Rectangle[float](10, 20, 30, 40)
    border = juce.BorderSize[float](2, 3, 4, 5)
    result_rect = border.addedTo(original_rect)
    assert result_rect == juce.Rectangle[float](7, 18, 38, 46)

#==================================================================================================

def test_add_to_rectangle_in_place():
    original_rect = juce.Rectangle[int](10, 20, 30, 40)
    border = juce.BorderSize[int](2, 3, 4, 5)
    border.addTo(original_rect)
    assert original_rect == juce.Rectangle[int](7, 18, 38, 46)

    original_rect = juce.Rectangle[float](10, 20, 30, 40)
    border = juce.BorderSize[float](2, 3, 4, 5)
    border.addTo(original_rect)
    assert original_rect == juce.Rectangle[float](7, 18, 38, 46)

#==================================================================================================

def test_subtract_from_border():
    border1 = juce.BorderSize[int](2, 3, 4, 5)
    border2 = juce.BorderSize[int](1, 2, 3, 4)
    result_border = border1.subtractedFrom(border2)
    assert result_border == juce.BorderSize[int](-1, -1, -1, -1)

    border1 = juce.BorderSize[float](2, 3, 4, 5)
    border2 = juce.BorderSize[float](1, 2, 3, 4)
    result_border = border1.subtractedFrom(border2)
    assert result_border == juce.BorderSize[float](-1, -1, -1, -1)

#==================================================================================================

def test_add_to_border():
    border1 = juce.BorderSize[int](2, 3, 4, 5)
    border2 = juce.BorderSize[int](1, 2, 3, 4)
    result_border = border1.addedTo(border2)
    assert result_border == juce.BorderSize[int](3, 5, 7, 9)

    border1 = juce.BorderSize[float](2, 3, 4, 5)
    border2 = juce.BorderSize[float](1, 2, 3, 4)
    result_border = border1.addedTo(border2)
    assert result_border == juce.BorderSize[float](3, 5, 7, 9)

#==================================================================================================

def test_multiply_by_scalar():
    border = juce.BorderSize[int](2, 3, 4, 5)
    result_border = border.multipliedBy(2)
    assert result_border == juce.BorderSize[int](4, 6, 8, 10)

    border = juce.BorderSize[float](2, 3, 4, 5)
    result_border = border.multipliedBy(2.0)
    assert result_border == juce.BorderSize[float](4, 6, 8, 10)

#==================================================================================================

def test_equality():
    border1 = juce.BorderSize[int](2, 3, 4, 5)
    border2 = juce.BorderSize[int](2, 3, 4, 5)
    assert border1 == border2

    border1 = juce.BorderSize[float](2, 3, 4, 5)
    border2 = juce.BorderSize[float](2, 3, 4, 5)
    assert border1 == border2

#==================================================================================================

def test_inequality():
    border1 = juce.BorderSize[int](2, 3, 4, 5)
    border2 = juce.BorderSize[int](1, 2, 3, 4)
    assert border1 != border2

    border1 = juce.BorderSize[float](2, 3, 4, 5)
    border2 = juce.BorderSize[float](1, 2, 3, 4)
    assert border1 != border2

#==================================================================================================

def test_repr():
    b = juce.BorderSize[int](2, 3, 4, 5)
    assert repr(b) == "popsicle.BorderSize[int](2, 3, 4, 5)"
