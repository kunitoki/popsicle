import os
import setuptools
import glob
import shutil

from setuptools.dist import Distribution

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

    def has_ext_modules(self):
        return True

data_folder = "popsicle/data"

shutil.rmtree(data_folder, ignore_errors=True)

for module in glob.glob("JUCE/modules/**"):
    module_name = os.path.split(module)[1]
    if module_name.startswith("juce_"):
        for file_path in glob.glob(os.path.join(module, "**", "*.h"), recursive=True):
            new_path = file_path.replace("JUCE/modules/{}/".format(module_name), "")
            new_path = os.path.join(data_folder, module_name, new_path)
            try:
                os.makedirs(os.path.split(new_path)[0])
            except OSError:
                pass
            shutil.copy(file_path, new_path)

for f in glob.glob("cxxbuild/popsicle_artefacts/**/*.*"):
    shutil.copy(f, data_folder)

for f in glob.glob("cxxbuild/popsicle_artefacts/JuceLibraryCode/**/Defs.txt"):
    shutil.copy(f, data_folder)

package_data = glob.glob("{}/**/*".format(data_folder)) + glob.glob("{}/*".format(data_folder))
package_data = [d for d in package_data if os.path.isfile(d)]

with open("VERSION.txt", "r") as f: version = f.read()
with open("README.rst", "r") as f: long_description = f.read()

setuptools.setup(
    name="popsicle",
    version=version,
    author="Lucio 'kRAkEn/gORe' Asnaghi",
    author_email="kunitoki@gmail.com",
    description="popsicle is JUCE Python Bindings on top of cppyy",
    long_description=long_description,
    long_description_content_type="text/x-rst",
    url="https://github.com/kunitoki/popsicle",
    packages=setuptools.find_packages(".", exclude=["examples"]),
    package_data={ "": package_data },
    include_package_data=True,
    zip_safe=False,
    distclass=BinaryDistribution,
    platforms=[ "macosx", "win32", "linux" ],
    python_requires=">=3.6",
    setup_requires=[ "wheel" ],
    install_requires=[ "cppyy>=1.9.1" ],
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

shutil.rmtree(data_folder, ignore_errors=True)
