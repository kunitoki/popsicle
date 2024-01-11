import os
import sys
import glob
from pathlib import Path

try:
	import juce

except ImportError:
    folder = Path(__file__).parent.parent / "build"
    print(folder)
    for f in glob.iglob(os.path.join(str(folder.resolve()), "**/*.so")):
        print(str(Path(f).parent.resolve()))
        sys.path.append(str(Path(f).parent.resolve()))
        break

    import juce
