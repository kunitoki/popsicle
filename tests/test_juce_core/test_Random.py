import sys

import popsicle as juce


#==================================================================================================

def test_system_random():
    r = juce.Random.getSystemRandom()

    for _ in range(1000):
        a = r.nextBool()
        assert a or not a

    for _ in range(1000):
        a = r.nextInt(100)
        assert a >= 0 and a <= 100

    for _ in range(1000):
        a = r.nextInt(juce.Range[int](0, 100))
        assert a >= 0 and a <= 100

    for _ in range(1000):
        a = r.nextInt() / float(sys.maxsize)
        assert a >= -1.0 and a <= 1.0

    for _ in range(1000):
        a = r.nextInt64() / float(sys.maxsize)
        assert a >= -1.0 and a <= 1.0

    for _ in range(1000):
        a = r.nextFloat() / sys.float_info.max
        assert a >= 0.0 and a <= 1.0

    for _ in range(1000):
        a = r.nextDouble() / sys.float_info.max
        assert a >= 0.0 and a <= 1.0

#==================================================================================================

def test_seeded_random():
    r = juce.Random(10)
    a1 = r.nextInt()
    a2 = r.nextInt()
    r.setSeed(10)
    b1 = r.nextInt()
    b2 = r.nextInt()

    assert a1 == b1
    assert a2 == b2

    r = juce.Random(10)
    c1 = r.nextInt()
    c2 = r.nextInt()

    assert a1 == c1
    assert a2 == c2
