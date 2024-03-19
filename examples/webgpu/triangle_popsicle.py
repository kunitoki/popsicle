"""
Import the viz from triangle.py and run it using popsicle.

# run_example = false
"""

import sys
import wgpu

from pathlib import Path

from pop import WgpuCanvas, run

sys.path.insert(0, str(Path(__file__).parent))

from triangle import main  # noqa: E402, The function to call to run the visualization


if __name__ == "__main__":
    canvas = WgpuCanvas(size=(640, 480), title="wgpu triangle", max_fps=60)
    run(canvas, main)
