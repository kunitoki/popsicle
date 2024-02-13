import os
import sys
import shutil
from pathlib import Path

if __name__ == "__main__":
    base_folder = Path(sys.argv[1])
    output_folder = Path(sys.argv[2])
    version_major = sys.argv[3]
    version_minor = sys.argv[4]

    version = f"{version_major}.{version_minor}"
    version_nodot = f"{version_major}{version_minor}"

    final_location = output_folder / "python"
    site_packages = final_location / "site-packages"
    base_python = base_folder / "lib" / f"python{version}"

    ignored_files = shutil.ignore_patterns(
        "*.pyc",
        "__pycache__",
        "__phello__",
        "*config-3*",
        "*tcl*",
        "*tdbc*",
        "*tk*",
        "Tk*",
        "_tk*",
        "_test*",
        "libpython*",
        "pkgconfig",
        "idlelib",
        "site-packages",
        "test",
        "turtledemo",
    )

    print("cleaning up...")
    shutil.rmtree(str(final_location))

    print("copying library...")
    shutil.copytree(str(base_python), str(final_location), ignore=ignored_files, dirs_exist_ok=True)

    print("creating site-packages...")
    os.makedirs(str(site_packages), exist_ok=True)

    print("making archive...")
    shutil.make_archive(str(output_folder / f"python{version_nodot}"), "zip", str(final_location))
