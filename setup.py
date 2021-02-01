import os
import sys
import pathlib
import glob
import shutil
import setuptools

from setuptools import Extension
from setuptools.dist import Distribution
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from wheel.bdist_wheel import bdist_wheel

project_name = "popsicle"

with open("VERSION.txt", "r") as f: version = f.read()
with open("README.rst", "r") as f: long_description = f.read()

def is_manylinux():
    try:
        with open("/etc/redhat-release", "r") as f:
            for line in f.readlines():
                if "CentOS release 6.10 (Final)" in line:
                    return True
    except (OSError, IOError):
        pass

    return False

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class BuildExtension(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)

        extdir = pathlib.Path(self.get_ext_fullpath(ext.name))
        extdir.mkdir(parents=True, exist_ok=True)

        output_dir = str(extdir.parent.absolute())

        config = "Debug" if self.debug else "Release"

        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_dir}",
            f"-DCMAKE_BUILD_TYPE={config}"
        ]

        if is_manylinux():
            cmake_args.append("-DPOPSICLE_MANYLINUX=1")

        os.chdir(str(build_temp))

        binary_dest = os.path.join(output_dir, project_name, "data")
        os.makedirs(binary_dest, exist_ok=True)

        try:
            self.spawn(["cmake", str(cwd)] + cmake_args)

            if self.dry_run: return

            build_command = ["cmake", "--build", ".", "--config", config]

            if sys.platform not in ["win32", "cygwin"]:
                build_command += ["--", f"-j{os.cpu_count()}"]

            self.spawn(build_command)

            for f in glob.glob("popsicle_artefacts/**/*.*"):
                shutil.copy(f, binary_dest)

            for f in glob.glob("popsicle_artefacts/JuceLibraryCode/**/*.*"):
                shutil.copy(f, binary_dest)

            os.chdir(str(cwd))
            for module in glob.iglob("JUCE/modules/**"):
                module_name = os.path.split(module)[1]
                if module_name.startswith("juce_"):
                    for file_path in glob.glob(os.path.join(module, "**", "*.h"), recursive=True):
                        new_path = file_path.replace("JUCE/modules/{}/".format(module_name), "")
                        new_path = os.path.join(binary_dest, module_name, new_path)
                        try:
                            os.makedirs(os.path.split(new_path)[0])
                        except OSError:
                            pass
                        shutil.copy(file_path, new_path)

        finally:
            os.chdir(str(cwd))

class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True

class BinaryDistWheel(bdist_wheel):
    def finalize_options(self):
        from distutils.util import get_platform
        self.plat_name = get_platform()
        self.universal = True
        bdist_wheel.finalize_options(self)
        self.root_is_pure = True

class InstallPlatformLibrary(install):
    def finalize_options(self):
        install.finalize_options(self)
        if self.distribution.has_ext_modules():
            self.install_lib = self.install_platlib

setuptools.setup(
    name=project_name,
    version=version,
    author="Lucio 'kRAkEn/gORe' Asnaghi",
    author_email="kunitoki@gmail.com",
    description="popsicle is JUCE Python Bindings on top of cppyy",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/kunitoki/popsicle",
    packages=setuptools.find_packages(".", exclude=["examples"]),
    include_package_data=True,
    cmdclass={
        "install": InstallPlatformLibrary,
        "bdist_wheel": BinaryDistWheel,
        "build_ext": BuildExtension,
    },
    ext_modules=[CMakeExtension("popsicle")],
    zip_safe=False,
    distclass=BinaryDistribution,
    platforms=[ "macosx", "win32", "linux" ],
    python_requires=">=3.6",
    setup_requires=[ "wheel" ],
    install_requires=[ "cppyy>=1.9.2" ],
    license="GPLv3",
    classifiers=[
        "Intended Audience :: Developers",
        "Topic :: Software Development :: Libraries :: Application Frameworks",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Development Status :: 3 - Alpha",
        "Programming Language :: Python :: 3 :: Only",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux"
    ]
)
