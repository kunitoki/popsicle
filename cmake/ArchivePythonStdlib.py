import os
import fnmatch
import sys
import stat
import shutil
import hashlib
import zipfile
from pathlib import Path
from argparse import ArgumentParser


def uniquify(seq):
    seen = set()
    return [x for x in seq if x and x not in seen and not seen.add(x)]


def file_hash(file):
    h = hashlib.md5()

    with open(file, "rb") as f:
        h.update(f.read())

    return h.hexdigest()


def cleanup_files(directory, file_patterns = None, folder_patterns = None):
    for dirname, folders, files in os.walk(directory):
        if folder_patterns:
            for foldername in folders:
                for pattern in folder_patterns:
                    if fnmatch.fnmatch(foldername, pattern):
                        shutil.rmtree(os.path.join(dirname, foldername))
                        break

        if file_patterns:
            for filename in files:
                for pattern in file_patterns:
                    if fnmatch.fnmatch(filename, pattern):
                        os.remove(os.path.join(dirname, filename))
                        break


def append_files(*args):
    archived_files = []

    for directory_or_file in args:
        if os.path.isdir(directory_or_file):
            for dirname, _, files in os.walk(directory_or_file):
                for filename in files:
                    path = os.path.join(dirname, filename)
                    archived_files.append((path, os.path.relpath(path, directory_or_file)))
        else:
            assert os.path.isabs(directory_or_file)
            archived_files.append((directory_or_file, os.path.split(directory_or_file)[1]))

    return archived_files


def make_archive(file, directory_or_file, *args):
    archived_files = append_files(directory_or_file, *args)

    with zipfile.ZipFile(file, "w") as zf:
        for path, archive_path in sorted(archived_files):
            permission = 0o555 if os.access(path, os.X_OK) else 0o444

            zip_info = zipfile.ZipInfo.from_file(path, archive_path)
            zip_info.date_time = (1999, 1, 1, 0, 0, 0)
            zip_info.external_attr = (stat.S_IFREG | permission) << 16

            with open(path, "rb") as fp:
                zf.writestr(zip_info, fp.read(), compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("-b", "--base-folder", type=Path, help="Path to the base folder.")
    parser.add_argument("-o", "--output-folder", type=Path, help="Path to the output folder.")
    parser.add_argument("-M", "--version-major", type=int, help="Major version number (integer).")
    parser.add_argument("-m", "--version-minor", type=int, help="Minor version number (integer).")
    parser.add_argument("-x", "--exclude-patterns", type=str, default=None, help="Excluded patterns (semicolon separated list).")
    parser.add_argument("-p", "--pip-packages", type=str, default=None, help="Pip packages to install (semicolon separated list).")
    parser.add_argument("-z", "--zip-python-executable", action="store_true", help="Store the python executable in an archive.")

    args = parser.parse_args()

    version = f"{args.version_major}.{args.version_minor}"
    version_nodot = f"{args.version_major}{args.version_minor}"

    base_python: Path = args.base_folder / "lib" / f"python{version}"
    final_location: Path = args.output_folder / "python"
    site_packages: Path = final_location / "site-packages"

    final_archive: Path = args.output_folder / f"python{version_nodot}.zip"
    temp_archive: Path = args.output_folder / f"temp{version_nodot}.zip"
    final_python_archive: Path = args.output_folder / f"python{version_nodot}_executable.zip"
    temp_python_archive: Path = args.output_folder / f"temp{version_nodot}_executable.zip"

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
        "_xxtestfuzz*",
        "doctest*",
        "idlelib",
        "lib2to3",
        "libpython*",
        "pkgconfig",
        "pydoc*",
        "site-packages",
        "test",
        "turtle*",
        "LICENSE.txt",
    ]

    if args.exclude_patterns:
        base_patterns += [x.strip() for x in args.exclude_patterns.split(";")]

    ignored_files = shutil.ignore_patterns(*uniquify(base_patterns))

    #==============================================================================================

    print("cleaning up...")
    if final_location.exists():
        shutil.rmtree(final_location)

    #==============================================================================================

    print("copying library...")
    shutil.copytree(
        base_python,
        final_location,
        ignore=ignored_files,
        ignore_dangling_symlinks=True,
        dirs_exist_ok=True)
    os.makedirs(site_packages, exist_ok=True)

    #==============================================================================================

    if args.pip_packages:
        print("installing packages...")
        packages = uniquify([x.strip() for x in args.pip_packages.split(";")])
        os.system(f"{sys.executable} -m pip install --no-compile --target={site_packages} {' '.join(packages)}")

        print("cleaning up packages...")
        cleanup_files(site_packages, ["*.pyi"], ["tests", "bin"])

    #==============================================================================================

    print("making packages archive...")
    #if sys.platform == "darwin":
    #    os.system(f"xattr -cr {final_location}")

    if os.path.exists(final_archive):
        make_archive(temp_archive, final_location)
        if file_hash(temp_archive) != file_hash(final_archive):
            shutil.copy(temp_archive, final_archive)
    else:
        make_archive(final_archive, final_location)

    #==============================================================================================

    if args.zip_python_executable:
        print("making python executable archive...")
        python_executable = os.path.realpath(sys.executable)
        print(python_executable)

        # TODO (darwin)
        # 1. otool -L python_executable
        # 2. if this contains a Python shared library in the Frameworks/Python.framework whatever, copy that as well
        # 3. fix install_name_tool on python_executable to use the local

        if os.path.exists(final_python_archive):
            make_archive(temp_python_archive, python_executable)
            if file_hash(temp_python_archive) != file_hash(final_python_archive):
                shutil.copy(temp_python_archive, final_python_archive)
        else:
            make_archive(final_python_archive, python_executable)
