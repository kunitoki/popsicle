from ctypes import c_uint32

import popsicle as juce


#==================================================================================================

def get_big_random(r: juce.Random) -> juce.BigInteger:
    b = juce.BigInteger()

    while (b < 2):
        r.fillBitsRandomly(b, 0, r.nextInt (150) + 1)

    return b

#==================================================================================================

def test_comparisons():
    r = juce.Random.getSystemRandom()

    assert juce.BigInteger().isZero()
    assert juce.BigInteger(1).isOne()

    for _ in range(10000, 0, -1):
        b1 = juce.BigInteger(get_big_random(r))
        b2 = juce.BigInteger(get_big_random(r))

        b3: juce.BigInteger = b1 + b2
        assert b3 > b1 and b3 > b2
        assert b3 - b1 == b2
        assert b3 - b2 == b1

        b4: juce.BigInteger = b1 * b2
        assert b4 > b1 and b4 > b2
        assert b4 / b1 == b2
        assert b4 / b2 == b1
        assert ((b4 << 1) >> 1) == b4
        assert ((b4 << 10) >> 10) == b4
        assert ((b4 << 100) >> 100) == b4

        b5 = juce.BigInteger()
        b5.loadFromMemoryBlock(b3.toMemoryBlock())
        assert b3 == b5

#==================================================================================================

def test_bit_setting():
    r = juce.Random.getSystemRandom()

    test = bytearray(2048)

    for _ in range(10000, 0, -1):
        offset = r.nextInt (200) + 10
        num = r.nextInt (32) + 1
        value = r.nextInt()

        if num < 32:
            value &= ((1 << num) - 1)

        value = c_uint32(value)

        old1 = juce.readLittleEndianBitsInBuffer (test, offset - 6, 6)
        old2 = juce.readLittleEndianBitsInBuffer (test, offset + num, 6)
        juce.writeLittleEndianBitsInBuffer (test, offset, num, value.value)
        result = juce.readLittleEndianBitsInBuffer (test, offset, num)

        assert c_uint32(result).value == value.value
        assert old1 == juce.readLittleEndianBitsInBuffer (test, offset - 6, 6)
        assert old2 == juce.readLittleEndianBitsInBuffer (test, offset + num, 6)
