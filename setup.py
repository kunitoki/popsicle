import os
import sys
import pathlib
import platform
import glob
import shutil
import setuptools

from distutils import sysconfig
from setuptools import Extension
from setuptools.dist import Distribution
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from wheel.bdist_wheel import bdist_wheel, get_platform


project_name = "juce"


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
        output_path.mkdir(parents=True, exist_ok=True)

        config = "Debug" if self.debug else "Release"
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_path}",
            f"-DCMAKE_BUILD_TYPE={config}",
            f"-DPython_INCLUDE_DIRS={self.get_includes_path()}",
            f"-DPython_LIBRARY_DIRS={self.get_lib_path()}"
        ]

        try:
            os.chdir(str(build_temp))
            self.spawn(["cmake", str(cwd)] + cmake_args)

            if getattr(self, "dry_run"): return

            build_command = ["cmake", "--build", ".", "--config", config]
            if sys.platform not in ["win32", "cygwin"]:
                build_command += ["--", f"-j{os.cpu_count()}"]
            self.spawn(build_command)

            for f in glob.iglob(f"{project_name}_artefacts/**/*.so"):
                shutil.copy(f, output_path / f"{project_name}.so")

        finally:
            os.chdir(str(cwd))

    def get_python_path(self):
        vars = sysconfig.get_config_vars()
        if 'LIBPL' in vars and 'LIBRARY' in vars:
            path = os.path.join(vars['LIBPL'], vars['LIBRARY'])
            if os.path.exists(path):
                return path

        if 'SCRIPTDIR' in vars:
            srcdir = vars['SCRIPTDIR']
        elif 'srcdir' in vars:
            srcdir = vars['srcdir']

        extensions = [".a", ".dylib", ".lib", ".so"]
        for extension in extensions:
            matches = list(glob.glob(f"{srcdir}/**/*python*{extension}", recursive=True))
            for match in matches:
                if "site-packages" not in match:
                    return match

        print("cannot find static library to be linked")
        exit(-1)

    def get_includes_path(self):
        include_dir = sysconfig.get_config_var('CONFINCLUDEPY')
        if include_dir and os.path.exists(include_dir):
            return include_dir
        return sysconfig.get_config_var('INCLUDEPY')

    def get_lib_path(self):
        return os.path.dirname(self.get_python_path())


class BinaryDistribution(Distribution):
    def has_ext_modules(self):
        return True


#class BinaryDistWheel(bdist_wheel):
#    def finalize_options(self):
#        bdist_wheel.finalize_options(self)
#        self.universal = True
#
#    def get_tag(self):
#        python, abi, plat = bdist_wheel.get_tag(self)
#        plat = 'macosx_10_15_universal2'
#        return python, abi, plat


class InstallPlatformLibrary(install):
    def finalize_options(self):
        install.finalize_options(self)
        if self.distribution.has_ext_modules():
            self.install_lib = self.install_platlib


with open("VERSION", "r") as f: version = f.read()
with open("README.rst", "r") as f: long_description = f.read()

cmdclass = {
    "install": InstallPlatformLibrary,
    "build_ext": BuildExtension
}

if platform.system() == 'Darwin':
    os.environ["_PYTHON_HOST_PLATFORM"] = "macosx-10.15-universal2"

#if platform.system() == 'Darwin':
#    cmdclass['bdist_wheel'] = BinaryDistWheel

setuptools.setup(
    name=project_name,
    version=version,
    author="Lucio 'kRAkEn/gORe' Asnaghi",
    author_email="kunitoki@gmail.com",
    description=f"{project_name} is JUCE Python Bindings on top of pybind11",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/kunitoki/juce_python",
    packages=setuptools.find_packages(".", exclude=["examples", "demo", "scripts"]),
    include_package_data=True,
    distclass=BinaryDistribution,
    cmdclass=cmdclass,
    ext_modules=[CMakeExtension(project_name)],
    zip_safe=False,
    platforms=["macosx", "win32", "linux"],
    python_requires=">=3.7",
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
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux"
    ]
)
