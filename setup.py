import os
import sys
import re
import shutil
import pathlib
import platform
import glob
import setuptools

from distutils import log
from distutils import sysconfig
from setuptools import Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.install_scripts import install_scripts


project_name = "popsicle"
root_dir = os.path.dirname(os.path.abspath(__file__))


def glob_python_library(path):
    for extension in [".a", ".lib", ".so", ".dylib", ".dll", ".pyd"]:
        for m in glob.iglob(f"{path}/**/*python*{extension}", recursive=True):
            if "site-packages" not in m:
                return m
    return None


def get_python_path():
    vars = sysconfig.get_config_vars()

    if 'LIBPL' in vars and 'LIBRARY' in vars:
        path = os.path.join(vars['LIBPL'], vars['LIBRARY'])
        if os.path.exists(path):
            return path

    if 'SCRIPTDIR' in vars:
        srcdir = vars['SCRIPTDIR']
    elif 'srcdir' in vars:
        srcdir = vars['srcdir']
    else:
        srcdir = None

    if srcdir:
        path = glob_python_library(srcdir)
        if path and os.path.exists(path):
            return path

    if 'LIBDEST' in vars:
        path = vars['LIBDEST']
        if sys.platform in ["win32", "cygwin"]:
            path = os.path.split(os.path.split(path)[0])[0]

        path = glob_python_library(path)
        if path and os.path.exists(path):
            return path

    log.error("cannot find static library to be linked")
    exit(-1)


def get_python_includes_path():
    include_dir = sysconfig.get_config_var('CONFINCLUDEPY')
    if include_dir and os.path.exists(include_dir):
        return include_dir
    return sysconfig.get_config_var('INCLUDEPY')


def get_python_lib_path():
    return os.path.dirname(get_python_path())


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class CMakeBuildExtension(build_ext):
    build_for_coverage = os.environ.get("POPSICLE_COVERAGE", None) is not None

    def build_extension(self, ext):
        log.info("building with cmake")

        cwd = pathlib.Path().absolute()

        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        extdir = pathlib.Path(self.get_ext_fullpath(ext.name)).absolute()
        extdir.mkdir(parents=True, exist_ok=True)

        output_path = extdir.parent
        output_path.mkdir(parents=True, exist_ok=True)

        config = "Debug" if self.debug or self.build_for_coverage else "Release"
        cmake_args = [
            f"-DCMAKE_BUILD_TYPE={config}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_path}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{config.upper()}={output_path}",
            f"-DPython_INCLUDE_DIRS={get_python_includes_path()}",
            f"-DPython_LIBRARY_DIRS={get_python_lib_path()}"
        ]

        if self.build_for_coverage:
            cmake_args += [
                "-DENABLE_COVERAGE:BOOL=ON"
            ]

        if platform.system() == 'Darwin':
            cmake_args += [
                "-DCMAKE_OSX_ARCHITECTURES:STRING=arm64;x86_64",
                "-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=10.15"
            ]

        try:
            os.chdir(str(build_temp))
            make_command = ["cmake", str(cwd)] + cmake_args
            self.spawn(make_command)

            if getattr(self, "dry_run"): return

            build_command = ["cmake", "--build", ".", "--config", config]
            if sys.platform not in ["win32", "cygwin"]:
                build_command += ["--", f"-j{os.cpu_count()}"]
            self.spawn(build_command)

            if self.build_for_coverage:
                self.generate_coverage(cwd)

        finally:
            os.chdir(str(cwd))

        self.generate_pyi(cwd)

    def generate_coverage(self, cwd):
        log.info("generating coverage files")

        self.spawn(["cmake", "--build", ".", "--target", f"{project_name}_coverage"])

        if os.path.isdir("/host"): # We are running in cibuildwheel container
            os.makedirs("/output", exist_ok=True)

            for m in glob.iglob(f"{cwd}/**/*.info", recursive=True):
                log.info(f"found {m} coverage info file")

                self.spawn(["sed", "-i", "'s:/project::g'", m])
                shutil.copyfile(m, f"/output/lcov.info")
                break

    def generate_pyi(self, cwd):
        log.info("generating pyi files")

        library = None
        for extension in [".so", ".pyd"]:
            for m in glob.iglob(f"{cwd}/**/{project_name}{extension}", recursive=True):
                library = m
                break

        if library is None:
            return

        library_dir = os.path.dirname(library)
        temp_pyi_dir = os.path.join(library_dir, project_name)
        final_pyi_dir = os.path.join(root_dir, project_name)
        final_pyi_file = os.path.join(root_dir, f"{project_name}.pyi")

        try:
            os.chdir(library_dir)

            shutil.rmtree(temp_pyi_dir, ignore_errors=True)
            shutil.rmtree(final_pyi_dir, ignore_errors=True)

            self.spawn(["stubgen", "--output", library_dir, "-p", project_name])

            if os.path.isdir(project_name):
                shutil.copytree(project_name, final_pyi_dir)

            if os.path.isfile(f"{project_name}.pyi"):
                shutil.copyfile(f"{project_name}.pyi", final_pyi_file)

        finally:
            os.chdir(str(cwd))


class CustomInstallScripts(install_scripts):
    def run(self):
        install_scripts.run(self)

        log.info("cleaning up pyi files")

        final_pyi_dir = os.path.join(root_dir, project_name)
        if os.path.isdir(final_pyi_dir):
            shutil.rmtree(final_pyi_dir, ignore_errors=True)

        final_pyi_file = os.path.join(root_dir, f"{project_name}.pyi")
        if os.path.isfile(final_pyi_file):
            os.remove(final_pyi_file)


with open("modules/juce_python/juce_python.h", mode="r", encoding="utf-8") as f:
    version = re.findall(r"version\:\s+(\d+\.\d+\.\d+)", f.read())[0]

with open("README.rst", mode="r", encoding="utf-8") as f:
    long_description = f.read()

if platform.system() == 'Darwin':
    os.environ["_PYTHON_HOST_PLATFORM"] = "macosx-10.15-universal2"


setuptools.setup(
    name=project_name,
    version=version,
    author="kunitoki",
    author_email="kunitoki@gmail.com",
    description=f"{project_name} is JUCE Python Bindings on top of pybind11",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/kunitoki/popsicle",
    packages=setuptools.find_packages(".", exclude=["*demo*", "*examples*", "*JUCE*", "*scripts*", "*tests*"]),
    include_package_data=True,
    cmdclass={"build_ext": CMakeBuildExtension, "install_scripts": CustomInstallScripts},
    ext_modules=[CMakeExtension(project_name)],
    zip_safe=False,
    platforms=["macosx", "win32", "linux"],
    python_requires=">=3.10",
    license="GPLv3",
    classifiers=[
        "Intended Audience :: Developers",
        "Development Status :: 2 - Beta",
        "Topic :: Software Development :: Libraries :: Application Frameworks",
        "License :: OSI Approved",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux"
    ]
)
