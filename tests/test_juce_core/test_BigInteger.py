from .. import common
import popsicle as juce


def get_big_random(r: juce.Random) -> juce.BigInteger:
    b = juce.BigInteger()

    while (b < 2):
        r.fillBitsRandomly(b, 0, r.nextInt (150) + 1)

    return b


def test_comparisons():
    r = juce.Random.getSystemRandom()

    assert juce.BigInteger().isZero()
    assert juce.BigInteger(1).isOne()

    for j in range(10000, 0, -1):
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

"""
    beginTest ("Bit setting");

    Random r = getRandom();
    static uint8 test[2048];

    for (int j = 100000; --j >= 0;)
    {
        uint32 offset = static_cast<uint32> (r.nextInt (200) + 10);
        uint32 num = static_cast<uint32> (r.nextInt (32) + 1);
        uint32 value = static_cast<uint32> (r.nextInt());

        if (num < 32)
            value &= ((1u << num) - 1);

        auto old1 = readLittleEndianBitsInBuffer (test, offset - 6, 6);
        auto old2 = readLittleEndianBitsInBuffer (test, offset + num, 6);
        writeLittleEndianBitsInBuffer (test, offset, num, value);
        auto result = readLittleEndianBitsInBuffer (test, offset, num);

        expect (result == value);
        expect (old1 == readLittleEndianBitsInBuffer (test, offset - 6, 6));
        expect (old2 == readLittleEndianBitsInBuffer (test, offset + num, 6));
    }
"""
