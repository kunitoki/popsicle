import pytest
from math import isclose

import popsicle as juce

#==================================================================================================

def test_empty_constructor():
    p = juce.Parallelogram[int]()
    assert p.topLeft.x == 0
    assert p.topLeft.y == 0
    assert p.topRight.x == 0
    assert p.topRight.y == 0
    assert p.bottomLeft.x == 0
    assert p.bottomLeft.y == 0
    # assert p.isEmpty()
    assert p.isFinite()
    assert p.getWidth() == 0
    assert p.getHeight() == 0
    assert p == p

    p = juce.Parallelogram[float]()
    assert isclose(p.topLeft.x, 0.0)
    assert isclose(p.topLeft.y, 0.0)
    assert isclose(p.topRight.x, 0.0)
    assert isclose(p.topRight.y, 0.0)
    assert isclose(p.bottomLeft.x, 0.0)
    assert isclose(p.bottomLeft.y, 0.0)
    # assert p.isEmpty()
    assert p.isFinite()
    assert isclose(p.getWidth(), 0.0)
    assert isclose(p.getHeight(), 0.0)
    assert p == p

#==================================================================================================

def test_rectangle_constructor():
    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    assert p.getTopLeft().x == 10
    assert p.getTopLeft().y == 10
    assert p.getTopRight().x == 30
    assert p.getTopRight().y == 10
    assert p.getBottomLeft().x == 10
    assert p.getBottomLeft().y == 30
    assert p.isFinite()
    assert p.getWidth() == 20
    assert p.getHeight() == 20
    assert p == p
    assert p != juce.Parallelogram[int]()

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    assert isclose(p.getTopLeft().x, 10.0)
    assert isclose(p.getTopLeft().y, 10.0)
    assert isclose(p.getTopRight().x, 30.0)
    assert isclose(p.getTopRight().y, 10.0)
    assert isclose(p.getBottomLeft().x, 10.0)
    assert isclose(p.getBottomLeft().y, 30.0)
    assert p.isFinite()
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)
    assert p == p
    assert p != juce.Parallelogram[float]()

#==================================================================================================

@pytest.mark.skip(reason="This is a JUCE bug")
def test_is_empty():
    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    assert not p.isEmpty()

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    assert not p.isEmpty()

#==================================================================================================

def test_operator_add_assign():
    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p += juce.Point[int](1, 2)
    assert p.getTopLeft().x == 11
    assert p.getTopLeft().y == 12
    assert p.getTopRight().x == 31
    assert p.getTopRight().y == 12
    assert p.getBottomLeft().x == 11
    assert p.getBottomLeft().y == 32
    assert p.getWidth() == 20
    assert p.getHeight() == 20

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p += juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 11.0)
    assert isclose(p.getTopLeft().y, 12.0)
    assert isclose(p.getTopRight().x, 31.0)
    assert isclose(p.getTopRight().y, 12.0)
    assert isclose(p.getBottomLeft().x, 11.0)
    assert isclose(p.getBottomLeft().y, 32.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)

#==================================================================================================

def test_operator_add():
    a = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p = a + juce.Point[int](1, 2)
    assert p.getTopLeft().x == 11
    assert p.getTopLeft().y == 12
    assert p.getTopRight().x == 31
    assert p.getTopRight().y == 12
    assert p.getBottomLeft().x == 11
    assert p.getBottomLeft().y == 32
    assert p.getWidth() == 20
    assert p.getHeight() == 20

    b = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p = b + juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 11.0)
    assert isclose(p.getTopLeft().y, 12.0)
    assert isclose(p.getTopRight().x, 31.0)
    assert isclose(p.getTopRight().y, 12.0)
    assert isclose(p.getBottomLeft().x, 11.0)
    assert isclose(p.getBottomLeft().y, 32.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)

#==================================================================================================

def test_operator_sub_assign():
    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p -= juce.Point[int](1, 2)
    assert p.getTopLeft().x == 9
    assert p.getTopLeft().y == 8
    assert p.getTopRight().x == 29
    assert p.getTopRight().y == 8
    assert p.getBottomLeft().x == 9
    assert p.getBottomLeft().y == 28
    assert p.getWidth() == 20
    assert p.getHeight() == 20

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p -= juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 9.0)
    assert isclose(p.getTopLeft().y, 8.0)
    assert isclose(p.getTopRight().x, 29.0)
    assert isclose(p.getTopRight().y, 8.0)
    assert isclose(p.getBottomLeft().x, 9.0)
    assert isclose(p.getBottomLeft().y, 28.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)

#==================================================================================================

def test_operator_sub():
    a = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p = a - juce.Point[int](1, 2)
    assert p.getTopLeft().x == 9
    assert p.getTopLeft().y == 8
    assert p.getTopRight().x == 29
    assert p.getTopRight().y == 8
    assert p.getBottomLeft().x == 9
    assert p.getBottomLeft().y == 28
    assert p.getWidth() == 20
    assert p.getHeight() == 20

    b = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p = b - juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 9.0)
    assert isclose(p.getTopLeft().y, 8.0)
    assert isclose(p.getTopRight().x, 29.0)
    assert isclose(p.getTopRight().y, 8.0)
    assert isclose(p.getBottomLeft().x, 9.0)
    assert isclose(p.getBottomLeft().y, 28.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)

#==================================================================================================

def test_operator_mul_assign():
    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p *= juce.Point[float](1, 2)
    assert p.getTopLeft().x == 10
    assert p.getTopLeft().y == 20
    assert p.getTopRight().x == 30
    assert p.getTopRight().y == 20
    assert p.getBottomLeft().x == 10
    assert p.getBottomLeft().y == 60
    assert p.getWidth() == 20
    assert p.getHeight() == 40

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p *= juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 10.0)
    assert isclose(p.getTopLeft().y, 20.0)
    assert isclose(p.getTopRight().x, 30.0)
    assert isclose(p.getTopRight().y, 20.0)
    assert isclose(p.getBottomLeft().x, 10.0)
    assert isclose(p.getBottomLeft().y, 60.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 40.0)

#==================================================================================================

def test_operator_mul():
    a = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p = a * juce.Point[float](1, 2)
    assert p.getTopLeft().x == 10
    assert p.getTopLeft().y == 20
    assert p.getTopRight().x == 30
    assert p.getTopRight().y == 20
    assert p.getBottomLeft().x == 10
    assert p.getBottomLeft().y == 60
    assert p.getWidth() == 20
    assert p.getHeight() == 40

    b = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p = b * juce.Point[float](1, 2)
    assert isclose(p.getTopLeft().x, 10.0)
    assert isclose(p.getTopLeft().y, 20.0)
    assert isclose(p.getTopRight().x, 30.0)
    assert isclose(p.getTopRight().y, 20.0)
    assert isclose(p.getBottomLeft().x, 10.0)
    assert isclose(p.getBottomLeft().y, 60.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 40.0)

#==================================================================================================

def test_transformed_by():
    t = juce.AffineTransform.translation(10, 10)

    p = juce.Parallelogram[int](juce.Rectangle[int](10, 10, 20, 20))
    p = p.transformedBy(t)
    assert p.getTopLeft().x == 20
    assert p.getTopLeft().y == 20
    assert p.getTopRight().x == 40
    assert p.getTopRight().y == 20
    assert p.getBottomLeft().x == 20
    assert p.getBottomLeft().y == 40
    assert p.getWidth() == 20
    assert p.getHeight() == 20

    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))
    p = p.transformedBy(t)
    assert isclose(p.getTopLeft().x, 20.0)
    assert isclose(p.getTopLeft().y, 20.0)
    assert isclose(p.getTopRight().x, 40.0)
    assert isclose(p.getTopRight().y, 20.0)
    assert isclose(p.getBottomLeft().x, 20.0)
    assert isclose(p.getBottomLeft().y, 40.0)
    assert isclose(p.getWidth(), 20.0)
    assert isclose(p.getHeight(), 20.0)

#==================================================================================================

def test_relative_point():
    p = juce.Parallelogram[float](juce.Rectangle[float](10, 10, 20, 20))

    relative = p.getRelativePoint(juce.Point[float](0.0, 0.5))
    assert isclose(relative.x, 10.0)
    assert isclose(relative.y, 20.0)

    relative = p.getRelativePoint(juce.Point[float](0.5, 1.0))
    assert isclose(relative.x, 20.0)
    assert isclose(relative.y, 30.0)

#==================================================================================================

def test_bounding_box():
    def make_point(type, x, y):
        return juce.Point[type](x, y)

    p = juce.Parallelogram[int](make_point(int, 10, 10), make_point(int, 20, 10), make_point(int, 15, 20))
    b = p.getBoundingBox()
    assert b.getTopLeft().x == 10
    assert b.getTopLeft().y == 10
    assert b.getBottomRight().x == 25
    assert b.getBottomRight().y == 20
    assert b.getWidth() == 15
    assert b.getHeight() == 10

    p = juce.Parallelogram[float](make_point(float, 10, 10), make_point(float, 20, 10), make_point(float, 15, 20))
    b = p.getBoundingBox()
    assert isclose(b.getTopLeft().x, 10.0)
    assert isclose(b.getTopLeft().y, 10.0)
    assert isclose(b.getBottomRight().x, 25.0)
    assert isclose(b.getBottomRight().y, 20.0)
    assert isclose(b.getWidth(), 15.0)
    assert isclose(b.getHeight(), 10.0)

