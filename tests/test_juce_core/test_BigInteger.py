from ctypes import c_uint32

import popsicle as juce

#==================================================================================================

def get_big_random(r: juce.Random) -> juce.BigInteger:
    b = juce.BigInteger()

    while (b < 2):
        r.fillBitsRandomly(b, 0, r.nextInt (150) + 1)

    return b

#==================================================================================================

def test_construct_default():
    big_int = juce.BigInteger()
    assert big_int.isZero()

#==================================================================================================

def test_construct_with_uint32():
    big_int = juce.BigInteger(1234)
    assert big_int.toInteger() == 1234

#==================================================================================================

def test_construct_with_int32():
    big_int = juce.BigInteger(-1234)
    assert big_int.toInteger() == -1234
    assert big_int.isNegative()

#==================================================================================================

def test_construct_with_int64():
    big_int = juce.BigInteger(1234567890123)
    assert big_int.toInt64() == 1234567890123

#==================================================================================================

def test_copy_constructor():
    original = juce.BigInteger(12345)
    copy = juce.BigInteger(original)
    assert copy.toInteger() == 12345

#==================================================================================================

def test_move_constructor():
    original = juce.BigInteger(12345)
    moved = juce.BigInteger(original)
    assert moved.toInteger() == 12345

#==================================================================================================

def test_assignment_operator():
    original = juce.BigInteger(12345)
    assigned = juce.BigInteger()
    assigned = original
    assert assigned.toInteger() == 12345

#==================================================================================================

def test_swap_with():
    a = juce.BigInteger(1234)
    b = juce.BigInteger(5678)
    a.swapWith(b)
    assert a.toInteger() == 5678
    assert b.toInteger() == 1234

#==================================================================================================

def test_is_zero():
    big_int = juce.BigInteger()
    assert big_int.isZero()

#==================================================================================================

def test_is_one():
    big_int = juce.BigInteger(1)
    assert big_int.isOne()

#==================================================================================================

def test_to_integer():
    big_int = juce.BigInteger(1234)
    assert big_int.toInteger() == 1234

#==================================================================================================

def test_to_int64():
    big_int = juce.BigInteger(1234567890123)
    assert big_int.toInt64() == 1234567890123

#==================================================================================================

def test_clear():
    big_int = juce.BigInteger(1234)
    big_int.clear()
    assert big_int.isZero()

#==================================================================================================

def test_clear_bit():
    big_int = juce.BigInteger(3)  # binary 11
    big_int.clearBit(0)
    assert big_int.toInteger() == 2  # binary 10

#==================================================================================================

def test_set_bit():
    big_int = juce.BigInteger()
    big_int.setBit(1)
    assert big_int.toInteger() == 2

#==================================================================================================

def test_set_bit_value():
    big_int = juce.BigInteger()
    big_int.setBit(1, True)
    assert big_int.toInteger() == 2
    big_int.setBit(1, False)
    assert big_int.isZero()

#==================================================================================================

def test_set_range():
    big_int = juce.BigInteger()
    big_int.setRange(0, 4, True)  # 1111 in binary
    assert big_int.toInteger() == 15

#==================================================================================================

def test_insert_bit():
    big_int = juce.BigInteger(2)  # binary 10
    big_int.insertBit(1, True)    # should result in binary 110
    assert big_int.toInteger() == 6

#==================================================================================================

def test_get_bit_range():
    big_int = juce.BigInteger(15)  # binary 1111
    sub_int = big_int.getBitRange(1, 2)  # should get binary 11
    assert sub_int.toInteger() == 3

#==================================================================================================

def test_get_bit_range_as_int():
    big_int = juce.BigInteger(15)  # 1111 in binary
    result = big_int.getBitRangeAsInt(0, 4)
    assert result == 15

#==================================================================================================

def test_set_bit_range_as_int():
    big_int = juce.BigInteger()
    big_int.setBitRangeAsInt(0, 4, 15)  # 1111 in binary
    assert big_int.toInteger() == 15

#==================================================================================================

def test_shift_bits():
    big_int = juce.BigInteger(1)  # binary 1
    big_int.shiftBits(2, 0)       # should result in binary 100
    assert big_int.toInteger() == 4

#==================================================================================================

def test_count_number_of_set_bits():
    big_int = juce.BigInteger(15)  # 1111 in binary
    assert big_int.countNumberOfSetBits() == 4

#==================================================================================================

def test_find_next_set_bit():
    big_int = juce.BigInteger(10)  # binary 1010
    assert big_int.findNextSetBit(0) == 1

#==================================================================================================

def test_find_next_clear_bit():
    big_int = juce.BigInteger(9)  # binary 1001
    assert big_int.findNextClearBit(0) == 1

#==================================================================================================

def test_get_highest_bit():
    big_int = juce.BigInteger(8)  # binary 1000
    assert big_int.getHighestBit() == 3

#==================================================================================================

def test_is_negative():
    big_int = juce.BigInteger(-1)
    assert big_int.isNegative()

#==================================================================================================

def test_set_negative():
    big_int = juce.BigInteger(1)
    big_int.setNegative(True)
    assert big_int.isNegative()

#==================================================================================================

def test_negate():
    big_int = juce.BigInteger(1)
    big_int.negate()
    assert big_int.isNegative()
    big_int.negate()
    assert not big_int.isNegative()

#==================================================================================================

def test_addition():
    a = juce.BigInteger(1)
    b = juce.BigInteger(2)
    a += b
    assert a.toInteger() == 3

#==================================================================================================

def test_subtraction():
    a = juce.BigInteger(3)
    b = juce.BigInteger(2)
    a -= b
    assert a.toInteger() == 1

#==================================================================================================

def test_multiplication():
    a = juce.BigInteger(3)
    b = juce.BigInteger(2)
    a *= b
    assert a.toInteger() == 6

#==================================================================================================

def test_division():
    a = juce.BigInteger(6)
    b = juce.BigInteger(2)
    a /= b
    assert a.toInteger() == 3

#==================================================================================================

def test_modulus():
    a = juce.BigInteger(7)
    b = juce.BigInteger(3)
    a %= b
    assert a.toInteger() == 1

#==================================================================================================

def test_bitwise_and():
    a = juce.BigInteger(6)  # binary 110
    b = juce.BigInteger(3)  # binary 011
    a &= b
    assert a.toInteger() == 2  # binary 010

#==================================================================================================

def test_bitwise_or():
    a = juce.BigInteger(6)  # binary 110
    b = juce.BigInteger(3)  # binary 011
    a |= b
    assert a.toInteger() == 7  # binary 111

#==================================================================================================

def test_bitwise_xor():
    a = juce.BigInteger(6)  # binary 110
    b = juce.BigInteger(3)  # binary 011
    a ^= b
    assert a.toInteger() == 5  # binary 101

#==================================================================================================

def test_left_shift():
    a = juce.BigInteger(1)
    a <<= 2
    assert a.toInteger() == 4

#==================================================================================================

def test_right_shift():
    a = juce.BigInteger(4)
    a >>= 2
    assert a.toInteger() == 1

#==================================================================================================

def test_equality():
    a = juce.BigInteger(123)
    b = juce.BigInteger(123)
    assert a == b

#==================================================================================================

def test_inequality():
    a = juce.BigInteger(123)
    b = juce.BigInteger(456)
    assert a != b

#==================================================================================================

def test_less_than():
    a = juce.BigInteger(123)
    b = juce.BigInteger(456)
    assert a < b

#==================================================================================================

def test_less_than_or_equal():
    a = juce.BigInteger(123)
    b = juce.BigInteger(123)
    assert a <= b

#==================================================================================================

def test_greater_than():
    a = juce.BigInteger(456)
    b = juce.BigInteger(123)
    assert a > b

#==================================================================================================

def test_greater_than_or_equal():
    a = juce.BigInteger(456)
    b = juce.BigInteger(456)
    assert a >= b

#==================================================================================================

def test_compare():
    a = juce.BigInteger(123)
    b = juce.BigInteger(456)
    assert a.compare(b) < 0
    assert b.compare(a) > 0

#==================================================================================================

def test_compare_absolute():
    a = juce.BigInteger(-123)
    b = juce.BigInteger(123)
    assert a.compareAbsolute(b) == 0

#==================================================================================================

def test_divide_by():
    divisor = juce.BigInteger(3)
    remainder = juce.BigInteger()
    a = juce.BigInteger(10)
    a.divideBy(divisor, remainder)
    assert a.toInteger() == 3
    assert remainder.toInteger() == 1

#==================================================================================================

def test_find_greatest_common_divisor():
    a = juce.BigInteger(54)
    b = juce.BigInteger(24)
    assert a.findGreatestCommonDivisor(b).toInteger() == 6

#==================================================================================================

def test_exponent_modulo():
    a = juce.BigInteger(2)
    exponent = juce.BigInteger(3)
    modulus = juce.BigInteger(5)
    a.exponentModulo(exponent, modulus)
    assert a.toInteger() == 3

#==================================================================================================

def test_inverse_modulo():
    a = juce.BigInteger(3)
    modulus = juce.BigInteger(11)
    a.inverseModulo(modulus)
    assert a.toInteger() == 4

#==================================================================================================

def test_montgomery_multiplication():
    a = juce.BigInteger(10001)
    other = juce.BigInteger(3)
    modulus = juce.BigInteger(5)
    modulusp = juce.BigInteger(2)

    a.montgomeryMultiplication(other, modulus, modulusp, 1)

    result = juce.BigInteger()
    result.parseString('3a94', 16)
    assert a == result

#==================================================================================================

def test_extended_euclidean():
    a = juce.BigInteger(240)
    b = juce.BigInteger(46)

    xOut = juce.BigInteger()
    yOut = juce.BigInteger()
    gcd = juce.BigInteger(a)

    gcd.extendedEuclidean(a, b, xOut, yOut)

    assert gcd == juce.BigInteger(2)

#==================================================================================================

def test_comparisons():
    r = juce.Random.getSystemRandom()

    assert juce.BigInteger().isZero()
    assert juce.BigInteger(1).isOne()

    for _ in range(1000, 0, -1):
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

    for _ in range(1000, 0, -1):
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
