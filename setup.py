import os
import sys
import re
import pathlib
import platform
import glob
import shutil
import setuptools

from distutils import log
from distutils import sysconfig
from setuptools import Extension
from setuptools.command.build_ext import build_ext


project_name = "popsicle"


class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])


class BuildExtension(build_ext):
    def build_extension(self, ext):
        log.info("entering build extension: cmake")

        cwd = pathlib.Path().absolute()

        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        extdir = pathlib.Path(self.get_ext_fullpath(ext.name)).absolute()
        extdir.mkdir(parents=True, exist_ok=True)

        output_path = extdir.parent
        output_path.mkdir(parents=True, exist_ok=True)

        config = "Debug" if self.debug else "Release"
        cmake_args = [
            f"-DCMAKE_BUILD_TYPE={config}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_path}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{config.upper()}={output_path}",
            f"-DPython_INCLUDE_DIRS={self.get_includes_path()}",
            f"-DPython_LIBRARY_DIRS={self.get_lib_path()}"
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

            for extension in [".pyd"] if sys.platform in ["win32", "cygwin"] else [".so"]:
                for f in glob.iglob(f"{project_name}_artefacts/**/*{extension}", recursive=True):
                    shutil.copy(f, output_path / f"{project_name}{extension}")

        finally:
            os.chdir(str(cwd))

    def get_python_path(self):
        vars = sysconfig.get_config_vars()
        log.info(f"vars={vars}")

        if 'LIBPL' in vars and 'LIBRARY' in vars:
            path = os.path.join(vars['LIBPL'], vars['LIBRARY'])
            if os.path.exists(path):
                return path

        if 'LIBDEST' in vars:
            path = vars['LIBDEST']
            if sys.platform not in ["win32", "cygwin"]:
                path = os.path.split(path)[0]
                path = os.path.split(path)[0]

            path = self.glob_python_library(path)
            if path and os.path.exists(path):
                return path

        srcdir = None
        if 'SCRIPTDIR' in vars:
            srcdir = vars['SCRIPTDIR']
        elif 'srcdir' in vars:
            srcdir = vars['srcdir']

        if srcdir:
            path = self.glob_python_library(srcdir)
            if path and os.path.exists(path):
                return path

        log.error("cannot find static library to be linked")
        exit(-1)

    def glob_python_library(self, path):
        log.info(path)

        for extension in [".a", ".lib", ".so", ".dylib", ".dll", ".pyd"]:
            for m in glob.iglob(f"{path}/**/*python*{extension}", recursive=True):
                log.info(m)
                if "site-packages" not in m:
                    return m
        return None

    def get_includes_path(self):
        include_dir = sysconfig.get_config_var('CONFINCLUDEPY')
        if include_dir and os.path.exists(include_dir):
            return include_dir
        return sysconfig.get_config_var('INCLUDEPY')

    def get_lib_path(self):
        return os.path.dirname(self.get_python_path())


with open("modules/juce_python/juce_python.h", "r") as f:
    version = re.findall(r"version\:\s+(\d+\.\d+\.\d+)", f.read())[0]

with open("README.rst", "r") as f:
    long_description = f.read()


setuptools.setup(
    name=project_name,
    version=version,
    author="Lucio 'kRAkEn/gORe' Asnaghi",
    author_email="kunitoki@gmail.com",
    description=f"{project_name} is JUCE Python Bindings on top of pybind11",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/kunitoki/popsicle",
    packages=setuptools.find_packages(".", exclude=["demo", "examples", "JUCE", "modules", "scripts"]),
    include_package_data=True,
    cmdclass={"build_ext": BuildExtension},
    ext_modules=[CMakeExtension(project_name)],
    zip_safe=False,
    platforms=["macosx", "win32", "linux"],
    python_requires=">=3.8",
    setup_requires=["wheel"],
    license="MIT",
    classifiers=[
        "Intended Audience :: Developers",
        "Development Status :: 3 - Alpha",
        "Topic :: Software Development :: Libraries :: Application Frameworks",
        "License :: OSI Approved",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux"
    ]
)
