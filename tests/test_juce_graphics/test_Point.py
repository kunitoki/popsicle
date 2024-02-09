import pytest
from math import isclose

import popsicle as juce

#==================================================================================================

def test_empty_constructor():
    p = juce.Point[int]()
    assert p.x == 0
    assert p.y == 0
    assert p.getX() == 0
    assert p.getY() == 0
    assert p.isOrigin()
    assert p.isFinite()

    p = juce.Point[float]()
    assert isclose(p.x, 0.0)
    assert isclose(p.y, 0.0)
    assert isclose(p.getX(), 0.0)
    assert isclose(p.getY(), 0.0)
    assert p.isOrigin()
    assert p.isFinite()

#==================================================================================================

def test_value_constructor():
    p = juce.Point[int](10, 20)
    assert p.x == 10
    assert p.y == 20
    assert not p.isOrigin()
    assert p.isFinite()

    p = juce.Point[float](10.5, 20.5)
    assert isclose(p.x, 10.5)
    assert isclose(p.y, 20.5)
    assert not p.isOrigin()
    assert p.isFinite()

#==================================================================================================

def test_infinite_constructor():
    p = juce.Point[float](float('inf'), float('-inf'))
    assert not p.isFinite()

#==================================================================================================

def test_set_xy():
    p = juce.Point[int]()

    p.x = 100
    p.y = 100
    assert p.x == 100
    assert p.y == 100
    assert p.getX() == 100
    assert p.getY() == 100

    p.setX(10)
    p.setY(10)
    assert p.x == 10
    assert p.y == 10
    assert p.getX() == 10
    assert p.getY() == 10

    p = juce.Point[float]()

    p.x = 100.5
    p.y = 100.5
    assert isclose(p.x, 100.5)
    assert isclose(p.y, 100.5)
    assert isclose(p.getX(), 100.5)
    assert isclose(p.getY(), 100.5)

    p.setX(10.5)
    p.setY(10.5)
    assert isclose(p.x, 10.5)
    assert isclose(p.y, 10.5)
    assert isclose(p.getX(), 10.5)
    assert isclose(p.getY(), 10.5)

#==================================================================================================

def test_with_xy():
    p = juce.Point[int](1, 2)
    assert p.withX(10) == juce.Point[int](10, 2)
    assert p.withY(20) == juce.Point[int](1, 20)

    p = juce.Point[float](1.0, 2.0)
    assert p.withX(10.0) == juce.Point[float](10.0, 2.0)
    assert p.withY(20.0) == juce.Point[float](1.0, 20.0)

#==================================================================================================

def test_add_xy():
    p = juce.Point[int](1, 2)
    p.addXY(10, 10)
    assert p.getX() == 11
    assert p.getY() == 12

    p = juce.Point[float](1.0, 2.0)
    p.addXY(10.0, 10.0)
    assert isclose(p.getX(), 11.0)
    assert isclose(p.getY(), 12.0)

#==================================================================================================

def test_translated():
    p = juce.Point[int](1, 2)
    p = p.translated(10, 10)
    assert p.getX() == 11
    assert p.getY() == 12

    p = juce.Point[float](1.0, 2.0)
    p = p.translated(10.0, 10.0)
    assert isclose(p.getX(), 11.0)
    assert isclose(p.getY(), 12.0)

#==================================================================================================

def test_operator_add():
    a = juce.Point[int](1, 2)
    b = juce.Point[int](4, 8)

    p = a + b
    assert p.getX() == 5
    assert p.getY() == 10
    p += a
    assert p.getX() == 6
    assert p.getY() == 12

    a = juce.Point[float](1.0, 2.0)
    b = juce.Point[float](4.0, 8.0)

    p = a + b
    assert isclose(p.getX(), 5.0)
    assert isclose(p.getY(), 10.0)
    p += a
    assert isclose(p.getX(), 6.0)
    assert isclose(p.getY(), 12.0)

#==================================================================================================

def test_operator_subtract():
    a = juce.Point[int](1, 2)
    b = juce.Point[int](4, 8)

    p = b - a
    assert p.getX() == 3
    assert p.getY() == 6
    p -= a
    assert p.getX() == 2
    assert p.getY() == 4

    a = juce.Point[float](1.0, 2.0)
    b = juce.Point[float](4.0, 8.0)

    p = b - a
    assert isclose(p.getX(), 3.0)
    assert isclose(p.getY(), 6.0)
    p -= a
    assert isclose(p.getX(), 2.0)
    assert isclose(p.getY(), 4.0)

#==================================================================================================

def test_operator_multiply():
    a = juce.Point[int](1, 2)
    b = juce.Point[int](4, 8)

    p = a * b
    assert p.getX() == 4
    assert p.getY() == 16
    p *= a
    assert p.getX() == 4
    assert p.getY() == 32

    a = juce.Point[float](1.0, 2.0)
    b = juce.Point[float](4.0, 8.0)

    p = a * b
    assert isclose(p.getX(), 4.0)
    assert isclose(p.getY(), 16.0)
    p *= a
    assert isclose(p.getX(), 4.0)
    assert isclose(p.getY(), 32.0)

#==================================================================================================

def test_operator_multiply_scalar():
    a = juce.Point[int](1, 2)

    p = a * 2
    assert p.getX() == 2
    assert p.getY() == 4
    p *= 2
    assert p.getX() == 4
    assert p.getY() == 8

    a = juce.Point[float](1.0, 2.0)

    p = a * 2.0
    assert isclose(p.getX(), 2.0)
    assert isclose(p.getY(), 4.0)
    p *= 2.0
    assert isclose(p.getX(), 4.0)
    assert isclose(p.getY(), 8.0)


#==================================================================================================

def test_operator_divide():
    a = juce.Point[int](1, 2)
    b = juce.Point[int](4, 8)

    p = b / a
    assert p.getX() == 4
    assert p.getY() == 4
    p /= b
    assert p.getX() == 1
    assert p.getY() == 0

    a = juce.Point[float](1.0, 2.0)
    b = juce.Point[float](4.0, 8.0)

    p = b / a
    assert isclose(p.getX(), 4.0)
    assert isclose(p.getY(), 4.0)
    p /= b
    assert isclose(p.getX(), 1.0)
    assert isclose(p.getY(), 0.5)

#==================================================================================================

def test_operator_divide_scalar():
    a = juce.Point[int](4, 8)

    p = a / 2.0
    assert p.getX() == 2
    assert p.getY() == 4
    p /= 2
    assert p.getX() == 1
    assert p.getY() == 2

    a = juce.Point[float](4.0, 8.0)

    p = a / 3.0
    assert isclose(p.getX(), 1.3333333730697632)
    assert isclose(p.getY(), 2.6666667461395264)
    p /= 3.0
    assert isclose(p.getX(), 0.4444444477558136)
    assert isclose(p.getY(), 0.8888888955116272)

#==================================================================================================

def test_operator_unary_minus():
    a = juce.Point[int](1, 2)

    a = -a
    assert a.getX() == -1
    assert a.getY() == -2

    a = juce.Point[float](1.0, 2.0)

    a = -a
    assert isclose(a.getX(), -1.0)
    assert isclose(a.getY(), -2.0)

#==================================================================================================

def test_distance_from_origin():
    a = juce.Point[int](0, 0)
    assert isclose(a.getDistanceFromOrigin(), 0)

    a = juce.Point[int](2, 3)
    assert isclose(a.getDistanceFromOrigin(), 3)

    a = juce.Point[int](-2, 3)
    assert isclose(a.getDistanceFromOrigin(), 3)

    a = juce.Point[float](0, 0)
    assert isclose(a.getDistanceFromOrigin(), 0)

    a = juce.Point[float](2, 3)
    assert isclose(a.getDistanceFromOrigin(), 3.605551242828369)

    a = juce.Point[float](2, -3)
    assert isclose(a.getDistanceFromOrigin(), 3.605551242828369)

#==================================================================================================

def test_distance_squared_from_origin():
    a = juce.Point[int](0, 0)
    assert isclose(a.getDistanceSquaredFromOrigin(), 0)

    a = juce.Point[int](2, 3)
    assert isclose(a.getDistanceSquaredFromOrigin(), 13)

    a = juce.Point[int](-2, 3)
    assert isclose(a.getDistanceSquaredFromOrigin(), 13)

    a = juce.Point[float](0, 0)
    assert isclose(a.getDistanceSquaredFromOrigin(), 0)

    a = juce.Point[float](2, 3)
    assert isclose(a.getDistanceSquaredFromOrigin(), 13.0)

    a = juce.Point[float](2, -3)
    assert isclose(a.getDistanceSquaredFromOrigin(), 13.0)

#==================================================================================================

def test_distance_from():
    a = juce.Point[int](0, 0)
    assert isclose(a.getDistanceFrom(juce.Point[int]()), 0)

    a = juce.Point[int](2, 3)
    assert isclose(a.getDistanceFrom(juce.Point[int](12, 13)), 14)

    a = juce.Point[int](-2, 3)
    assert isclose(a.getDistanceFrom(juce.Point[int](2, 3)), 4)

    a = juce.Point[float](0, 0)
    assert isclose(a.getDistanceFrom(juce.Point[float]()), 0)

    a = juce.Point[float](2, 3)
    assert isclose(a.getDistanceFrom(juce.Point[float](12, 13)), 14.142135620117188)

    a = juce.Point[float](2, -3)
    assert isclose(a.getDistanceFrom(juce.Point[float](2, 3)), 6.0)

#==================================================================================================

def test_distance_squared_from():
    a = juce.Point[int](0, 0)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[int]()), 0)

    a = juce.Point[int](2, 3)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[int](12, 13)), 200)

    a = juce.Point[int](-2, 3)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[int](2, 3)), 16)

    a = juce.Point[float](0, 0)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[float]()), 0)

    a = juce.Point[float](2, 3)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[float](12, 13)), 200.0)

    a = juce.Point[float](2, -3)
    assert isclose(a.getDistanceSquaredFrom(juce.Point[float](2, 3)), 36.0)

#==================================================================================================

def test_angle_to_point():
    a = juce.Point[int](0, 0)
    b = juce.Point[int](10, 0)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 90, rel_tol=1e-6)

    a = juce.Point[int](0, 0)
    b = juce.Point[int](0, 10)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 180, rel_tol=1e-6)

    a = juce.Point[int](0, 0)
    b = juce.Point[int](10, 10)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 135, rel_tol=1e-6)

    a = juce.Point[float](0, 0)
    b = juce.Point[float](10, 0)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 90, rel_tol=1e-6)

    a = juce.Point[float](0, 0)
    b = juce.Point[float](0, 10)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 180, rel_tol=1e-6)

    a = juce.Point[float](0, 0)
    b = juce.Point[float](10, 10)
    assert isclose(juce.radiansToDegrees(a.getAngleToPoint(b)), 135, rel_tol=1e-6)

#==================================================================================================

@pytest.mark.skip(reason="The integer version of this is actually broken in JUCE")
def test_rotated_about_origin_int():
    a = juce.Point[int](10, 0)
    assert a.rotatedAboutOrigin(int(juce.degreesToRadians(90))) == juce.Point[int](10, 0)

#==================================================================================================

def test_rotated_about_origin_float():
    a = juce.Point[float](10.5, 0.956)

    r = a.rotatedAboutOrigin(juce.degreesToRadians(90))
    assert isclose(r.x, -0.956, rel_tol=1e-06)
    assert isclose(r.y, 10.5, rel_tol=1e-06)

#==================================================================================================

def test_point_on_circumference_int():
    a = juce.Point[int](0, 0)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(0))
    assert isclose(r.x, 0.0, rel_tol=1e-06)
    assert isclose(r.y, -1.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(90))
    assert isclose(r.x, 1.0, rel_tol=1e-06)
    # assert isclose(r.y, 0.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(180))
    # assert isclose(r.x, 0.0, rel_tol=1e-06)
    assert isclose(r.y, 1.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(270))
    assert isclose(r.x, -1.0, rel_tol=1e-06)
    #assert isclose(r.y, 0.0, rel_tol=1e-06)

#==================================================================================================

def test_point_on_circumference_float():
    a = juce.Point[float](0, 0)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(0))
    assert isclose(r.x, 0.0, rel_tol=1e-06)
    assert isclose(r.y, -1.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(90))
    assert isclose(r.x, 1.0, rel_tol=1e-06)
    # assert isclose(r.y, 0.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(180))
    # assert isclose(r.x, 0.0, rel_tol=1e-06)
    assert isclose(r.y, 1.0, rel_tol=1e-06)

    r = a.getPointOnCircumference(1.0, juce.degreesToRadians(270))
    assert isclose(r.x, -1.0, rel_tol=1e-06)
    #assert isclose(r.y, 0.0, rel_tol=1e-06)

#==================================================================================================

def test_dot_product():
    a = juce.Point[int](2, 3)
    b = juce.Point[int](7, 9)

    assert isclose(a.getDotProduct(b), 14 + 27)

    a = juce.Point[float](2, 3)
    b = juce.Point[float](7, 9)

    assert isclose(a.getDotProduct(b), 14 + 27)

#==================================================================================================

def test_apply_transform():
    a = juce.Point[int](2, 3)

    a.applyTransform(juce.AffineTransform.translation(10.0, 20.0))
    assert a.x == 12
    assert a.y == 23

    a = juce.Point[float](2, 3)

    a.applyTransform(juce.AffineTransform.translation(10.0, 20.0))
    assert isclose(a.x, 12)
    assert isclose(a.y, 23)

#==================================================================================================

def test_transformed_by():
    a = juce.Point[int](2, 3)

    p = a.transformedBy(juce.AffineTransform.scale(0.5, 2.0))
    assert p.x == 1
    assert p.y == 6

    a = juce.Point[float](2, 3)

    p = a.transformedBy(juce.AffineTransform.scale(0.25, 2.5))
    assert isclose(p.x, 0.5)
    assert isclose(p.y, 7.5)

#==================================================================================================

def test_round_to_int():
    a = juce.Point[float](1.99, 2.4).toInt()
    assert isinstance(a.x, int)
    assert isinstance(a.y, int)
    assert a.x == 1
    assert a.y == 2

#==================================================================================================

def test_to_float():
    a = juce.Point[int](1, 2).toFloat()
    assert isinstance(a.x, float)
    assert isinstance(a.y, float)
    assert isclose(a.x, 1.0)
    assert isclose(a.y, 2.0)

#==================================================================================================

def test_to_int():
    a = juce.Point[float](1.1, 2.8).roundToInt()
    assert isinstance(a.x, int)
    assert isinstance(a.y, int)

#==================================================================================================

def test_to_string():
    a = juce.Point[float](1.1, 2.8)
    assert a.toString() == "1.1, 2.8"
    assert repr(a) == "popsicle.Point[float](1.1, 2.8)"
    assert str(a) == "1.1, 2.8"
