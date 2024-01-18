from .. import common
import uuid
import popsicle as juce


def test_construct_empty():
    a = juce.Uuid()
    assert a != ""
    assert not a.isNull()


def test_construct_null():
    a = juce.Uuid.null()
    assert a == ""
    assert a.toString() == "00000000000000000000000000000000"
    assert a.toDashedString() == "00000000-0000-0000-0000-000000000000"
    assert a.isNull()


def test_constuct_string():
    a = juce.Uuid("974cc888-0d9a-4d47-8930-913f682430cb")
    assert a.getRawData() == b"\227L\310\210\r\232MG\2110\221?h$0\313"


def test_constuct_bytes():
    a = juce.Uuid(b"\227L\310\210\r\232MG\2110\221?h$0\313")
    assert a.toString() == "974cc8880d9a4d478930913f682430cb"
    assert a == "974cc8880d9a4d478930913f682430cb"
    assert a.getTimeLow() == 2538391688
    assert a.getTimeMid() == 3482
    assert a.getTimeHighAndVersion() == 19783
    assert a.getClockSeqAndReserved() == 137
    assert a.getClockSeqLow() == 48
    assert a.getNode() == 159701516169419
    assert a.hash() == 17627754469088036242

    b = juce.Uuid(b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f')
    assert b.toDashedString() == "00010203-0405-0607-0809-0a0b0c0d0e0f"
    assert b == "000102030405060708090a0b0c0d0e0f"


def test_constuct_comparisons():
    a = juce.Uuid(b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f')
    b = juce.Uuid(b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x1f')
    assert a == a
    assert a != b
    assert a < b
    assert b > a
    assert a <= b
    assert b >= a


def test_constuct_uuid():
    a = uuid.UUID('{00010203-0405-0607-0809-0a0b0c0d0e0f}')
    b = juce.Uuid(a)
    assert a.bytes == b.getRawData()
