import os
import sys
import glob
from pathlib import Path

try:
	import popsicle as juce

except ImportError:
    folder = (Path(__file__).parent.parent / "build")
    for ext in ["*.so", "*.pyd"]:
        path_to_search = folder / "**" / ext
        for f in glob.iglob(str(path_to_search), recursive=True):
            if os.path.isfile(f):
                sys.path.append(str(Path(f).parent))
                break

    import popsicle as juce
