import os
import sys
import glob
from pathlib import Path

try:
	import popsicle as juce

except ImportError:
    folder = Path(__file__).parent.parent / "build"
    for f in glob.iglob(os.path.join(str(folder.resolve()), "**/*.so")):
        if os.path.isfile(f):
            sys.path.append(str(Path(f).parent.resolve()))
            break

    import popsicle as juce
