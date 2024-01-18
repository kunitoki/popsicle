from .. import common
import popsicle as juce


def test_construct_empty():
    a = juce.Uuid()
    assert a != ""


def test_constuct_string():
    a = juce.Uuid("974cc888-0d9a-4d47-8930-913f682430cb")
    assert a.getRawData() == b"\227L\310\210\r\232MG\2110\221?h$0\313"


def test_constuct_bytes():
    a = juce.Uuid(b"\227L\310\210\r\232MG\2110\221?h$0\313")
    assert a.toString() == "974cc8880d9a4d478930913f682430cb"
    assert a == "974cc8880d9a4d478930913f682430cb"

    b = juce.Uuid(b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f')
    assert b.toDashedString() == "00010203-0405-0607-0809-0a0b0c0d0e0f"
    assert b == "000102030405060708090a0b0c0d0e0f"
