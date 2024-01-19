import popsicle as juce


def test_empty_constructor():
    a = juce.Identifier()
    assert a.isNull()
    assert not a.isValid()

    b = juce.Identifier()
    assert a == b

    c = juce.Identifier("123")
    assert a != c
    assert b != c
    assert c != a
    assert c != b


def test_constructor_python_str():
    a = juce.Identifier("abc")
    assert not a.isNull()
    assert a.isValid()

    b = juce.Identifier("abc")
    assert a == b


def test_constructor_copy():
    a = juce.Identifier("abc")
    b = juce.Identifier(a)
    assert a.isNull() == b.isNull()
    assert a.isValid() == b.isValid()
    assert a.toString() == b.toString()


def test_constructor_juce_String():
    a = juce.Identifier("abc")
    assert not a.isNull()
    assert a.isValid()

    b = juce.Identifier("abc")
    assert a == b


def test_comparisons():
    a = juce.Identifier("abc")
    b = juce.Identifier("abc")
    c = juce.Identifier("abcd")
    assert a == b
    assert a <= b
    assert a >= b
    assert not (a < b)
    assert not (a > b)

    assert a != c
    assert a <= c
    assert not (a >= c)
    assert a < c
    assert not (a > c)


def test_toString():
    a = juce.Identifier("abc")
    b = juce.Identifier("abc")
    assert a.toString() == b.toString()

    c = juce.Identifier("123")
    assert a.toString() != c.toString()


def test_isValidIdentifier():
    assert not juce.Identifier.isValidIdentifier("")
    assert not juce.Identifier.isValidIdentifier(" ")
    assert juce.Identifier.isValidIdentifier("abcf123")
