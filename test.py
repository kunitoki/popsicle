import os
import sys
import shutil
import pytest

if __name__ == "__main__":
    root_dir = os.path.dirname(__file__)
    package_name = os.path.split(root_dir)[1]
    src_module = os.path.join(root_dir, package_name)
    dst_module = os.path.join(root_dir, f"__{package_name}")

    try:
        if os.path.isdir(src_module):
            shutil.move(src_module, dst_module)

        sys.exit(pytest.main(sys.argv[1:]))

    finally:
        if os.path.isdir(dst_module):
            shutil.move(dst_module, src_module)
