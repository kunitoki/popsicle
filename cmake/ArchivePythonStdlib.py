import os
import sys
import shutil
from pathlib import Path
from argparse import ArgumentParser

if __name__ == "__main__":
    parser = ArgumentParser()

    parser.add_argument("-b", "--base-folder", type=Path, help="Path to the base folder.")
    parser.add_argument("-o", "--output-folder", type=Path, help="Path to the output folder.")
    parser.add_argument("-M", "--version-major", type=int, help="Major version number (integer).")
    parser.add_argument("-m", "--version-minor", type=int, help="Minor version number (integer).")
    parser.add_argument("-i", "--ignore-patterns", type=str, default=None, help="Ignored patterns (semicolon separated list).")

    args = parser.parse_args()

    version = f"{args.version_major}.{args.version_minor}"
    version_nodot = f"{args.version_major}{args.version_minor}"

    final_location = args.output_folder / "python"
    site_packages = final_location / "site-packages"
    base_python = args.base_folder / "lib" / f"python{version}"

    base_patterns = [
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
        "LICENSE.txt",
    ]

    if args.ignore_patterns:
        custom_patterns = [x.strip() for x in args.ignore_patterns.split(";")]
        base_patterns += custom_patterns

    ignored_files = shutil.ignore_patterns(*base_patterns)

    print("cleaning up...")
    shutil.rmtree(str(final_location))

    print("copying library...")
    shutil.copytree(str(base_python), str(final_location), ignore=ignored_files, dirs_exist_ok=True)

    print("creating site-packages...")
    os.makedirs(str(site_packages), exist_ok=True)

    print("making archive...")
    shutil.make_archive(str(args.output_folder / f"python{version_nodot}"), "zip", str(final_location))
