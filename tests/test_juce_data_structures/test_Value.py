from ..fixtures import juce_app
import popsicle as juce


#==================================================================================================

def test_var_constructor():
    a = juce.Value()
    assert isinstance(a.getValue(), type(None))
    assert a.getValue() is None

    a = juce.Value(True)
    assert isinstance(a.getValue(), bool)
    assert a.getValue() == True

    a = juce.Value(1)
    assert isinstance(a.getValue(), int)
    assert a.getValue() == 1

    a = juce.Value(1.0)
    assert isinstance(a.getValue(), float)
    assert a.getValue() == 1.0

    a = juce.Value("abcdefg")
    assert isinstance(a.getValue(), str)
    assert a.getValue() == "abcdefg"

    a = juce.Value(b"abcdefg")
    assert isinstance(a.getValue(), bytes)
    assert a.getValue() == b"abcdefg"

    a = juce.Value((1, 2, 3))
    assert isinstance(a.getValue(), list)
    assert a.getValue() == [1, 2, 3]

    a = juce.Value([1, "2", 3, False, None])
    assert isinstance(a.getValue(), list)
    assert a.getValue() == [1, "2", 3, False, None]

    a = juce.Value({"a": 1, "b": 2, "c": None})
    assert isinstance(a.getValue(), dict)
    assert a.getValue() == {"a": 1, "b": 2, "c": None}

    a = juce.Value({"a": {"x": 10, "y": 20, "z": [1, 2]}, "b": 2, "c": 3})
    assert isinstance(a.getValue(), dict)
    assert a.getValue() == {"a": {"x": 10, "y": 20, "z": [1, 2]}, "b": 2, "c": 3}

    a = juce.Value(juce.MemoryBlock(b'\x01\x01\x01\x01'))
    assert isinstance(a.getValue(), bytes)
    assert a.getValue() == b'\x01\x01\x01\x01'

#==================================================================================================

class ValueListener(juce.Value.Listener):
    value = None

    def valueChanged(self, value):
        self.value = value.getValue()

#==================================================================================================

def test_value_listener(juce_app):
    listener = ValueListener()

    a = juce.Value(1.0)

    a.addListener(listener)
    assert listener.value == None

    a.setValue(2.0)
    assert a.getValue() == 2.0
    assert listener.value == None
    juce_app.processEvents()
    assert listener.value == a.getValue()

    a.removeListener(listener)

    a.setValue(3.0)
    assert a.getValue() == 3.0
    assert listener.value == 2.0
    juce_app.processEvents()
    assert listener.value == 2.0

#==================================================================================================

class ValueSource(juce.Value.ValueSource):
    value = 1

    def setValue(self, value):
        self.value = value * 2
        self.sendChangeMessage(False)

    def getValue(self):
        return self.value

#==================================================================================================

def test_value_source(juce_app):
    source = ValueSource()
    assert source.value == 1

    a = juce.Value(source)
    assert a.getValue() == 1
    assert a.getValueSource() == source
    b = juce.Value()
    b.referTo(a)

    assert b.refersToSameSourceAs(a)
    assert b.getValueSource() == source
    assert b.getValue() == 1

    a.setValue(2)
    assert a.getValue() == 4
    assert b.getValue() == 4
    assert a.toString() == "4"
    assert source.value == 4

    a.setValue(3)
    assert a.getValue() == 6
    assert b.getValue() == 6
    assert b.toString() == "6"
    assert source.value == 6

    listener = ValueListener()
    a.addListener(listener)

    a.setValue(4)
    assert a.getValue() == 8
    assert b.getValue() == 8

