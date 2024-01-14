import io
from contextlib import redirect_stdout

import juce_init
import popsicle as juce

with io.StringIO() as buf, redirect_stdout(buf):
    help(juce)
    docs = buf.getvalue()

print(docs)
