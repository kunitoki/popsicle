import popsicle as juce


#==================================================================================================

def test_range_int_constructor():
    a = juce.Range[int]()
    assert a.getStart() == 0
    assert a.getEnd() == 0
    assert a.getLength() == 0
    assert a.isEmpty()

    b = juce.Range[int](-100, 100)
    assert b.getStart() == -100
    assert b.getEnd() == 100
    assert b.getLength() == 200
    assert not b.isEmpty()

    c = juce.Range[int].between(-1, 1)
    assert c.getStart() == -1
    assert c.getEnd() == 1
    assert c.getLength() == 2
    assert not c.isEmpty()

    d = juce.Range[int].withStartAndLength(42, 1)
    assert d.getStart() == 42
    assert d.getEnd() == 43
    assert d.getLength() == 1
    assert not d.isEmpty()

    e = juce.Range[int].emptyRange(111)
    assert e.getStart() == 111
    assert e.getEnd() == 111
    assert e.getLength() == 0
    assert e.isEmpty()

#==================================================================================================

def test_range_set_start_end():
    a = juce.Range[int]()
    a.setStart(-10)
    assert a.getStart() == -10
    assert a.getEnd() == 0
    assert a.getLength() == 10
    assert not a.isEmpty()

    a.setEnd(10)
    assert a.getStart() == -10
    assert a.getEnd() == 10
    assert a.getLength() == 20
    assert not a.isEmpty()

#==================================================================================================

def test_range_with_start_end():
    a = juce.Range[int]()

    b = a.withStart(-10)
    assert a.getStart() == 0
    assert a.getEnd() == 0
    assert a.getLength() == 0
    assert a.isEmpty()

    assert b.getStart() == -10
    assert b.getEnd() == 0
    assert b.getLength() == 10
    assert not b.isEmpty()

    c = b.withEnd(10)
    assert b.getStart() == -10
    assert b.getEnd() == 0
    assert b.getLength() == 10
    assert not b.isEmpty()

    assert c.getStart() == -10
    assert c.getEnd() == 10
    assert c.getLength() == 20
    assert not c.isEmpty()

#==================================================================================================

def test_range_moved_start_end():
    a = juce.Range[int](0, 100)

    b = a.movedToStartAt(10)
    assert b.getStart() == 10
    assert b.getEnd() == 110
    assert b.getLength() == 100
    assert not b.isEmpty()

    c = b.movedToEndAt(90)
    assert c.getStart() == -10
    assert c.getEnd() == 90
    assert c.getLength() == 100
    assert not c.isEmpty()

#==================================================================================================

def test_range_set_length():
    a = juce.Range[int](0, 100)

    a.setLength(20)
    assert a.getStart() == 0
    assert a.getEnd() == 20
    assert not a.isEmpty()

    a.setStart(10)
    a.setLength(0)
    assert a.getStart() == 10
    assert a.getEnd() == 10
    assert a.isEmpty()

#==================================================================================================

def test_range_expanded():
    a = juce.Range[int](0, 100)

    b = a.expanded(100)
    assert b.getStart() == -100
    assert b.getEnd() == 200
    assert b.getLength() == 300
    assert not b.isEmpty()

#==================================================================================================

def test_range_operators():
    a = juce.Range[int](-10, 10)

    a += 1
    assert a.getStart() == -9
    assert a.getEnd() == 11
    assert a.getLength() == 20
    assert not a.isEmpty()

    a -= 1
    assert a.getStart() == -10
    assert a.getEnd() == 10
    assert a.getLength() == 20
    assert not a.isEmpty()

    b = a + 1
    assert b.getStart() == -9
    assert b.getEnd() == 11
    assert b.getLength() == 20
    assert not b.isEmpty()

    c = b - 1
    assert c.getStart() == -10
    assert c.getEnd() == 10
    assert c.getLength() == 20
    assert not c.isEmpty()

#==================================================================================================

def test_range_equality():
    a = juce.Range[int](-10, 10)
    b = juce.Range[int](-10, 10)
    c = juce.Range[int](-11, 10)

    assert a == a
    assert a == b
    assert b == a
    assert a != c
    assert c != a

#==================================================================================================

def test_range_contains():
    a = juce.Range[int](-10, 10)

    assert not a.contains(-11)
    assert a.contains(-10)
    assert a.contains(-1)
    assert a.contains(0)
    assert a.contains(1)
    assert a.contains(9)
    assert not a.contains(10)
    assert not a.contains(11)

    b = juce.Range[int](-15, -5)
    c = juce.Range[int](-15, -10)

    assert not a.contains(b)
    assert not b.contains(a)
    assert b.contains(c)
    assert not c.contains(b)

#==================================================================================================

def test_range_clip_value():
    a = juce.Range[int](-10, 10)

    assert a.clipValue(-11) == -10
    assert a.clipValue(-6) == -6
    assert a.clipValue(6) == 6
    assert a.clipValue(10) == 10

#==================================================================================================

def test_range_intersects():
    a = juce.Range[int](-10, 10)
    b = juce.Range[int](-15, -5)
    c = juce.Range[int](5, 15)

    assert a.intersects(b)
    assert b.intersects(a)
    assert not b.intersects(c)
    assert not c.intersects(b)

    assert a.getIntersectionWith(b) == juce.Range[int](-10, -5)
    assert b.getIntersectionWith(a) == juce.Range[int](-10, -5)
    assert b.getIntersectionWith(c).isEmpty()
    assert c.getIntersectionWith(b).isEmpty()

#==================================================================================================

def test_range_intersects():
    a = juce.Range[int](-10, 10)
    b = juce.Range[int](-15, -5)

    assert a.getUnionWith(100) == juce.Range[int](-10, 100)
    assert b.getUnionWith(-10) == juce.Range[int](-15, -5)
    assert b.getUnionWith(100) == juce.Range[int](-15, 100)

    assert a.getUnionWith(b) == juce.Range[int](-15, 10)
    assert b.getUnionWith(a) == juce.Range[int](-15, 10)

#==================================================================================================

def test_range_constrain_range():
    a = juce.Range[int](-10, 10)
    b = juce.Range[int](-15, -5)

    c = a.constrainRange(b)
    assert c == juce.Range[int](-10, 0)
