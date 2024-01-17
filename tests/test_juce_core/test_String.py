from .. import common
import popsicle as juce

def test_empty_constructor():
    a = juce.String()
    assert a.isEmpty()
    assert not a.isNotEmpty()
    assert a.length() == 0

def test_python_str_constructor():
    a = juce.String("abc")
    assert not a.isEmpty()
    assert a.isNotEmpty()
    assert a.length() == 3

    a = juce.String("abc", 2)
    assert not a.isEmpty()
    assert a.isNotEmpty()
    assert a.length() == 2

def test_copy_constructor():
    a = juce.String("abc")
    b = juce.String(a)
    assert a == b
    assert a.length() == b.length()

def test_number_constructor():
    a = juce.String(1)
    assert not a.isEmpty()
    assert a.isNotEmpty()
    assert str(a) == "1"
    assert a.length() == 1

    b = juce.String(-1)
    assert not b.isEmpty()
    assert b.isNotEmpty()
    assert str(b) == "-1"
    assert b.length() == 2

    c = juce.String(1.2345)
    assert not c.isEmpty()
    assert c.isNotEmpty()
    assert str(c) == "1.2345"
    assert c.length() == 6

    d = juce.String(-1.2345)
    assert not d.isEmpty()
    assert d.isNotEmpty()
    assert str(d) == "-1.2345"
    assert d.length() == 7
