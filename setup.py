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
from wheel.bdist_wheel import bdist_wheel, get_platform


project_name = "popsicle"


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

        extdir = pathlib.Path(self.get_ext_fullpath(ext.name)).absolute()
        extdir.mkdir(parents=True, exist_ok=True)

        output_path = extdir.parent
        config = "Debug" if self.debug else "Release"
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_path}",
            f"-DCMAKE_BUILD_TYPE={config}"
        ]

        binary_dest = output_path / project_name / "data"
        os.makedirs(binary_dest, exist_ok=True)

        try:
            os.chdir(str(build_temp))
            self.spawn(["cmake", str(cwd)] + cmake_args)

            if getattr(self, "dry_run"): return

            build_command = ["cmake", "--build", ".", "--config", config]
            if sys.platform not in ["win32", "cygwin"]:
                build_command += ["--", f"-j{os.cpu_count()}"]
            self.spawn(build_command)

            for f in glob.iglob("popsicle_artefacts/**/*.*"):
                shutil.copy(f, binary_dest)

            for f in glob.iglob("popsicle_artefacts/JuceLibraryCode/**/*.*"):
                shutil.copy(f, binary_dest)

            modules_base_dir = "JUCE/modules"

            os.chdir(str(cwd))
            for module in glob.iglob(f"{modules_base_dir}/**"):
                module_name = os.path.split(module)[1]
                if not module_name.startswith("juce_"):
                    continue

                for file_path in glob.iglob(os.path.join(module, "**", "*.h"), recursive=True):
                    path = pathlib.Path(file_path)

                    new_path = binary_dest / path.relative_to(f"{modules_base_dir}/")

                    new_path.parent.mkdir(parents=True, exist_ok=True)

                    shutil.copy(path.absolute(), new_path)

        finally:
            os.chdir(str(cwd))


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True


class BinaryDistWheel(bdist_wheel):
    def finalize_options(self):
        # For some reason without this it keeps failing to generate a wheel on my local osx
        #if sys.platform == "darwin":
        #    self.plat_name = "macosx_10_6_intel"
        #else:
        #self.plat_name = get_platform(self.bdist_dir)
        #self.universal = False
        bdist_wheel.finalize_options(self)
        #self.root_is_pure = True


class InstallPlatformLibrary(install):
    def finalize_options(self):
        install.finalize_options(self)
        if self.distribution.has_ext_modules():
            self.install_lib = self.install_platlib


with open("VERSION.txt", "r") as f: version = f.read()
with open("README.rst", "r") as f: long_description = f.read()


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
    distclass=BinaryDistribution,
    cmdclass={
        "install": InstallPlatformLibrary,
        "build_ext": BuildExtension,
    },
    ext_modules=[CMakeExtension("popsicle")],
    zip_safe=False,
    platforms=[ "macosx", "win32", "linux" ],
    python_requires=">=3.7",
    setup_requires=[ "wheel" ],
    install_requires=[ "cppyy>=2.3.1" ],
    license="GPLv3",
    classifiers=[
        "Intended Audience :: Developers",
        "Development Status :: 3 - Alpha",
        "Topic :: Software Development :: Libraries :: Application Frameworks",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux"
    ]
)
