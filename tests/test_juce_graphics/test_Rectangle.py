import pytest

import popsicle as juce

#==================================================================================================

def test_rectangle_default_constructor():
    rect = juce.Rectangle[int]()
    assert rect.getWidth() == 0
    assert rect.getHeight() == 0
    assert rect.getX() == 0
    assert rect.getY() == 0

#==================================================================================================

def test_rectangle_copy_constructor():
    original = juce.Rectangle[int](10, 20, 30, 40)
    copy = juce.Rectangle[int](original)
    assert copy.getX() == 10
    assert copy.getY() == 20
    assert copy.getWidth() == 30
    assert copy.getHeight() == 40

#==================================================================================================

def test_rectangle_position_and_size_constructor():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    assert rect.getX() == 10
    assert rect.getY() == 20
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

#==================================================================================================

def test_rectangle_size_constructor():
    rect = juce.Rectangle[int](30, 40)
    assert rect.getX() == 0
    assert rect.getY() == 0
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

#==================================================================================================

def test_rectangle_corners_constructor():
    corner1 = juce.Point[int](10, 20)
    corner2 = juce.Point[int](40, 60)
    rect = juce.Rectangle[int](corner1, corner2)
    assert rect.getX() == 10
    assert rect.getY() == 20
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

#==================================================================================================

def test_rectangle_is_empty():
    rect = juce.Rectangle[int]()
    assert rect.isEmpty()

    rect.setSize(10, 0)
    assert rect.isEmpty()

    rect.setSize(0, 10)
    assert rect.isEmpty()

    rect.setSize(-10, 10)
    assert rect.isEmpty()

    rect.setSize(10, -10)
    assert rect.isEmpty()

    rect.setSize(10, 10)
    assert not rect.isEmpty()

#==================================================================================================

def test_rectangle_get_set_position():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setPosition(50, 60)
    assert rect.getX() == 50
    assert rect.getY() == 60

    newPos = juce.Point[int](70, 80)
    rect.setPosition(newPos)
    assert rect.getPosition() == newPos

#==================================================================================================

def test_rectangle_get_set_size():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setSize(50, 60)
    assert rect.getWidth() == 50
    assert rect.getHeight() == 60

#==================================================================================================

def test_rectangle_get_set_bounds():
    rect = juce.Rectangle[int]()
    rect.setBounds(10, 20, 30, 40)
    assert rect.getX() == 10
    assert rect.getY() == 20
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

#==================================================================================================

def test_rectangle_translate():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.translate(5, -5)
    assert rect.getX() == 15
    assert rect.getY() == 15

#==================================================================================================

def test_rectangle_with_position():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    movedRect = rect.withPosition(50, 60)
    assert movedRect.getX() == 50
    assert movedRect.getY() == 60
    assert movedRect.getWidth() == 30
    assert movedRect.getHeight() == 40

    newPos = juce.Point[int](70, 80)
    movedRect = rect.withPosition(newPos)
    assert movedRect.getPosition() == newPos

#==================================================================================================

def test_rectangle_with_size():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    resizedRect = rect.withSize(50, 60)
    assert resizedRect.getX() == 10
    assert resizedRect.getY() == 20
    assert resizedRect.getWidth() == 50
    assert resizedRect.getHeight() == 60

#==================================================================================================

def test_rectangle_contains_point():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    pointInside = juce.Point[int](20, 30)
    pointOutside = juce.Point[int](5, 50)
    assert rect.contains(pointInside)
    assert not rect.contains(pointOutside)

#==================================================================================================

def test_rectangle_intersection():
    rect1 = juce.Rectangle[int](10, 20, 30, 40)
    rect2 = juce.Rectangle[int](20, 30, 30, 40)
    assert rect1.intersects(rect2)

    rect3 = juce.Rectangle[int](100, 100, 30, 40)
    assert not rect1.intersects(rect3)

#==================================================================================================

def test_rectangle_get_intersection():
    rect1 = juce.Rectangle[int](10, 20, 30, 40)
    rect2 = juce.Rectangle[int](20, 30, 30, 40)
    intersection = rect1.getIntersection(rect2)
    assert intersection.getX() == 20
    assert intersection.getY() == 30
    assert intersection.getWidth() == 20
    assert intersection.getHeight() == 30

    rect3 = juce.Rectangle[int](100, 100, 30, 40)
    intersection = rect1.getIntersection(rect3)
    assert intersection.isEmpty()

#==================================================================================================

def test_rectangle_union():
    rect1 = juce.Rectangle[int](10, 20, 30, 40)
    rect2 = juce.Rectangle[int](40, 50, 30, 40)
    unionRect = rect1.getUnion(rect2)
    assert unionRect.getX() == 10
    assert unionRect.getY() == 20
    assert unionRect.getWidth() == 60
    assert unionRect.getHeight() == 70

#==================================================================================================

def test_rectangle_expand():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.expand(5, 10)
    assert rect.getX() == 5
    assert rect.getY() == 10
    assert rect.getWidth() == 40
    assert rect.getHeight() == 60

#==================================================================================================

def test_rectangle_expanded():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    expandedRect = rect.expanded(5, 10)
    assert expandedRect.getX() == 5
    assert expandedRect.getY() == 10
    assert expandedRect.getWidth() == 40
    assert expandedRect.getHeight() == 60

#==================================================================================================

def test_rectangle_reduce():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.reduce(5, 10)
    assert rect.getX() == 15
    assert rect.getY() == 30
    assert rect.getWidth() == 20
    assert rect.getHeight() == 20

#==================================================================================================

def test_rectangle_reduced():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    reducedRect = rect.reduced(5, 10)
    assert reducedRect.getX() == 15
    assert reducedRect.getY() == 30
    assert reducedRect.getWidth() == 20
    assert reducedRect.getHeight() == 20

#==================================================================================================

def test_rectangle_remove_from_top():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    removedRect = rect.removeFromTop(10)
    assert rect.getY() == 30
    assert rect.getHeight() == 30
    assert removedRect.getY() == 20
    assert removedRect.getHeight() == 10

#==================================================================================================

def test_rectangle_remove_from_left():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    removedRect = rect.removeFromLeft(10)
    assert rect.getX() == 20
    assert rect.getWidth() == 20
    assert removedRect.getX() == 10
    assert removedRect.getWidth() == 10

#==================================================================================================

def test_rectangle_remove_from_right():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    removedRect = rect.removeFromRight(10)
    assert rect.getWidth() == 20
    assert removedRect.getX() == 30
    assert removedRect.getWidth() == 10

#==================================================================================================

def test_rectangle_remove_from_bottom():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    removedRect = rect.removeFromBottom(10)
    assert rect.getHeight() == 30
    assert removedRect.getY() == 50
    assert removedRect.getHeight() == 10

#==================================================================================================

def test_rectangle_get_constrained_point():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    pointInside = juce.Point[int](10, 20)
    pointOutside = juce.Point[int](0, 0)
    constrainedPoint = rect.getConstrainedPoint(pointOutside)
    assert constrainedPoint == pointInside

#==================================================================================================

def test_rectangle_get_relative_point():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    relativePoint = rect.getRelativePoint(0.5, 0.5)  # Center point
    assert relativePoint.getX() == 25
    assert relativePoint.getY() == 40

#==================================================================================================

def test_rectangle_proportion_of_width():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    proportionWidth = rect.proportionOfWidth(0.5)
    assert proportionWidth == 15

#==================================================================================================

def test_rectangle_proportion_of_height():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    proportionHeight = rect.proportionOfHeight(0.5)
    assert proportionHeight == 20

#==================================================================================================

def test_rectangle_get_proportion():
    rect = juce.Rectangle[float](10, 10, 10, 10)
    proportionalRect = rect.getProportion(juce.Rectangle[float](0.25, 0.25, 0.5, 0.5))
    assert proportionalRect.getCentreX() == pytest.approx(15)
    assert proportionalRect.getCentreY() == pytest.approx(15)
    assert proportionalRect.getWidth() == pytest.approx(5)
    assert proportionalRect.getHeight() == pytest.approx(5)

#==================================================================================================

def test_rectangle_contains_rectangle():
    rect1 = juce.Rectangle[int](10, 20, 30, 40)
    rect2 = juce.Rectangle[int](15, 25, 10, 10)
    assert rect1.contains(rect2)

    rect3 = juce.Rectangle[int](0, 0, 100, 100)
    assert not rect1.contains(rect3)

#==================================================================================================

def test_rectangle_operator_equality():
    rect1 = juce.Rectangle[int](10, 20, 30, 40)
    rect2 = juce.Rectangle[int](10, 20, 30, 40)
    assert rect1 == rect2

    rect3 = juce.Rectangle[int](0, 0, 50, 60)
    assert rect1 != rect3

#==================================================================================================

def test_rectangle_operator_addition():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    delta = juce.Point[int](5, 5)
    resultRect = rect + delta
    assert resultRect.getX() == 15
    assert resultRect.getY() == 25

#==================================================================================================

def test_rectangle_operator_subtraction():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    delta = juce.Point[int](5, 5)
    resultRect = rect - delta
    assert resultRect.getX() == 5
    assert resultRect.getY() == 15

#==================================================================================================

def test_rectangle_operator_multiplication():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    scaleFactor = 2
    resultRect = rect * scaleFactor
    # Expecting the rectangle to scale around the origin, not its center.
    assert resultRect.getX() == 20
    assert resultRect.getY() == 40
    assert resultRect.getWidth() == 60
    assert resultRect.getHeight() == 80

#==================================================================================================

def test_rectangle_operator_division():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    scaleFactor = 2
    resultRect = rect / scaleFactor
    # Expecting the rectangle to scale around the origin, not its center.
    assert resultRect.getX() == 5
    assert resultRect.getY() == 10
    assert resultRect.getWidth() == 15
    assert resultRect.getHeight() == 20

#==================================================================================================

def test_rectangle_set_left():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setLeft(5)
    assert rect.getX() == 5
    assert rect.getWidth() == 35  # Adjusted to keep the right edge in place

#==================================================================================================

def test_rectangle_with_left():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    newRect = rect.withLeft(5)
    assert newRect.getX() == 5
    assert newRect.getWidth() == 35  # Adjusted to keep the right edge in place

#==================================================================================================

def test_rectangle_set_top():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setTop(15)
    assert rect.getY() == 15
    assert rect.getHeight() == 45  # Adjusted to keep the bottom edge in place

#==================================================================================================

def test_rectangle_with_top():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    newRect = rect.withTop(15)
    assert newRect.getY() == 15
    assert newRect.getHeight() == 45  # Adjusted to keep the bottom edge in place

#==================================================================================================

def test_rectangle_set_right():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setRight(50)
    assert rect.getWidth() == 40  # Adjusted width to reach the new right edge

#==================================================================================================

def test_rectangle_with_right():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    newRect = rect.withRight(50)
    assert newRect.getWidth() == 40  # Adjusted width to reach the new right edge

#==================================================================================================

def test_rectangle_set_bottom():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setBottom(70)
    assert rect.getHeight() == 50  # Adjusted height to reach the new bottom edge

#==================================================================================================

def test_rectangle_with_bottom():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    newRect = rect.withBottom(70)
    assert newRect.getHeight() == 50  # Adjusted height to reach the new bottom edge

#==================================================================================================

def test_rectangle_with_trimmed_left():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    trimmedRect = rect.withTrimmedLeft(5)
    assert trimmedRect.getX() == 15
    assert trimmedRect.getWidth() == 25

#==================================================================================================

def test_rectangle_with_trimmed_right():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    trimmedRect = rect.withTrimmedRight(5)
    assert trimmedRect.getWidth() == 25

#==================================================================================================

def test_rectangle_with_trimmed_top():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    trimmedRect = rect.withTrimmedTop(5)
    assert trimmedRect.getY() == 25
    assert trimmedRect.getHeight() == 35

#==================================================================================================

def test_rectangle_with_trimmed_bottom():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    trimmedRect = rect.withTrimmedBottom(5)
    assert trimmedRect.getHeight() == 35

#==================================================================================================

def test_rectangle_set_horizontal_range():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setHorizontalRange(juce.Range[int](5, 50))
    assert rect.getX() == 5
    assert rect.getWidth() == 45

#==================================================================================================

def test_rectangle_set_vertical_range():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rect.setVerticalRange(juce.Range[int](15, 70))
    assert rect.getY() == 15
    assert rect.getHeight() == 55

#==================================================================================================

def test_rectangle_enlarge_if_adjacent():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    adjacentRect = juce.Rectangle[int](40, 20, 20, 40)
    assert rect.enlargeIfAdjacent(adjacentRect)
    assert rect.getWidth() == 50

    notAdjacentRect = juce.Rectangle[int](70, 20, 30, 40)
    assert not rect.enlargeIfAdjacent(notAdjacentRect)

#==================================================================================================

def test_rectangle_reduce_if_partly_contained_in():
    rect = juce.Rectangle[int](10, 10, 20, 20)
    largerRect = juce.Rectangle[int](5, 5, 20, 40)
    assert rect.reduceIfPartlyContainedIn(largerRect)
    assert not rect.isEmpty()

    nonOverlappingRect = juce.Rectangle[int](100, 100, 30, 40)
    assert not rect.reduceIfPartlyContainedIn(nonOverlappingRect)

    overlappingButComplexRect = juce.Rectangle[int](5, 5, 10, 10)
    assert not rect.reduceIfPartlyContainedIn(overlappingButComplexRect)

#==================================================================================================

def test_rectangle_constrained_within():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    constrainingRect = juce.Rectangle[int](0, 0, 20, 20)
    constrainedRect = rect.constrainedWithin(constrainingRect)
    assert constrainedRect.getX() == 0
    assert constrainedRect.getY() == 0
    assert constrainedRect.getWidth() == 20
    assert constrainedRect.getHeight() == 20

#==================================================================================================

def test_rectangle_transformed_by():
    # Simplified test, assuming AffineTransform is a similar concept as in other graphic libraries
    rect = juce.Rectangle[int](10, 20, 30, 40)
    # AffineTransform would typically include operations like translate, rotate, scale, etc.
    # This method would then test if the rectangle is correctly transformed by such an AffineTransform,
    # which is challenging without a concrete AffineTransform implementation to test with.
    # Placeholder test code assuming a hypothetical transform that doubles size:
    # transformedRect = rect.transformedBy(doubleSizeTransform)
    # assert transformedRect.getWidth() == 60
    # assert transformedRect.getHeight() == 80
    pass  # Replace pass with actual implementation when AffineTransform is available

#==================================================================================================

def test_rectangle_get_smallest_integer_container():
    rect = juce.Rectangle[float](10.5, 20.5, 30.1, 40.2)
    integerContainer = rect.getSmallestIntegerContainer()
    assert integerContainer.getX() == 10
    assert integerContainer.getY() == 20
    assert integerContainer.getWidth() == 31  # Rounded up to contain the floating point size
    assert integerContainer.getHeight() == 41  # Rounded up

#==================================================================================================

def test_rectangle_to_nearest_int():
    rect = juce.Rectangle[float](10.7, 20.7, 30.1, 40.2)
    intRect = rect.toNearestInt()
    assert intRect.getX() == 11
    assert intRect.getY() == 21
    assert intRect.getWidth() == 30
    assert intRect.getHeight() == 40

#==================================================================================================

def test_rectangle_to_nearest_int_edges():
    rect = juce.Rectangle[float](10.5, 20.5, 30.1, 40.2)
    intEdgesRect = rect.toNearestIntEdges()
    assert intEdgesRect.getX() == 10
    assert intEdgesRect.getY() == 20
    assert intEdgesRect.getWidth() == 31  # Rounded up to nearest edge
    assert intEdgesRect.getHeight() == 41  # Rounded up to nearest edge

#==================================================================================================

def test_rectangle_to_float():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    floatRect = rect.toFloat()
    assert floatRect.getX() == pytest.approx(10.0)
    assert floatRect.getY() == pytest.approx(20.0)
    assert floatRect.getWidth() == pytest.approx(30.0)
    assert floatRect.getHeight() == pytest.approx(40.0)

#==================================================================================================

"""
def test_rectangle_to_type():
    rect = juce.Rectangle[float](10.5, 20.5, 30.5, 40.5)
    intRect = rect.toType[int]()
    assert intRect.getX() == 10
    assert intRect.getY() == 20
    assert intRect.getWidth() == 30
    assert intRect.getHeight() == 40

    # This demonstrates converting a Rectangle of one type to another, like float to int in this case.
"""

#==================================================================================================

def test_rectangle_to_string():
    rect = juce.Rectangle[int](10, 20, 30, 40)
    rectStr = rect.toString()
    expectedStr = "10 20 30 40"
    assert rectStr == expectedStr

    # This method tests converting a rectangle's properties to a string format.

#==================================================================================================

def test_rectangle_static_left_top_right_bottom():
    rect = juce.Rectangle[int].leftTopRightBottom(10, 20, 40, 60)
    assert rect.getX() == 10
    assert rect.getY() == 20
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

    # This static method creates a Rectangle from left, top, right, and bottom values.

#==================================================================================================

"""
def test_rectangle_static_find_area_containing_points():
    points = [juce.Point[int](10, 20), juce.Point[int](30, 40), juce.Point[int](50, 60)]
    boundingRect = juce.Rectangle[int].findAreaContainingPoints(points, len(points))
    assert boundingRect.getX() == 10
    assert boundingRect.getY() == 20
    assert boundingRect.getWidth() == 40
    assert boundingRect.getHeight() == 40

    # This static method finds the smallest Rectangle that can contain a given set of points.
"""

#==================================================================================================

"""
def test_rectangle_static_intersect_rectangles():
    x1, y1, w1, h1 = 10, 20, 30, 40
    x2, y2, w2, h2 = 20, 30, 50, 60
    intersected = juce.Rectangle[int].intersectRectangles(x1, y1, w1, h1, x2, y2, w2, h2)
    assert intersected
    assert x1 == 20
    assert y1 == 30
    assert w1 == 20
    assert h1 == 30

    # This static utility method tests if two sets of rectangle coordinates intersect and adjusts the first set to the intersection area if they do.
"""

#==================================================================================================

def test_rectangle_static_from_string():
    rectStr = "10 20 30 40"
    rect = juce.Rectangle[int].fromString(rectStr)
    assert rect.getX() == 10
    assert rect.getY() == 20
    assert rect.getWidth() == 30
    assert rect.getHeight() == 40

