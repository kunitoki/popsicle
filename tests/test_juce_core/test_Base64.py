import popsicle as juce

#==================================================================================================

def test_to_base64():
    a = juce.Base64.toBase64(b'\xe6\x1dy\x98\x8cM\xd0\xa9\x12@\xedwri\x0c^')
    assert a == "5h15mIxN0KkSQO13cmkMXg=="

    b = juce.Base64.toBase64("The quick brown fox jumps over the lazy dog")
    assert b == "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw=="

#==================================================================================================

def test_convert_to_base64():
    mos = juce.MemoryOutputStream()
    juce.Base64.convertToBase64(mos, b"The quick brown fox jumps over the lazy dog")
    assert mos.toString() == "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw=="

#==================================================================================================

def test_convert_from_base64():
    mos = juce.MemoryOutputStream()
    juce.Base64.convertFromBase64(mos, "5h15mIxN0KkSQO13cmkMXg==")
    assert mos.getMemoryBlock().getData() == b'\xe6\x1dy\x98\x8cM\xd0\xa9\x12@\xedwri\x0c^'
