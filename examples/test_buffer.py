import juce_init
import popsicle as juce


mb = juce.MemoryBlock()

mb.append(bytearray(b'123456'))

print(mb.toString())
