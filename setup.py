import os
import sys
import setuptools
import glob
import shutil

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

for f in glob.glob("cxxbuild/popsicle_artefacts/*.*"):
    shutil.copy(f, data_folder)

package_data = glob.glob("{}/**/*".format(data_folder)) + glob.glob("{}/*".format(data_folder))
package_data = [d for d in package_data if os.path.isfile(d)]

setuptools.setup(
    name="popsicle",
    version="0.0.1",
    author="Lucio 'kRAkEn/gORe' Asnaghi",
    author_email="kunitoki@gmail.com",
    description="popsicle is JUCE Python Bindings on top of cppyy",
    long_description="popsicle is JUCE Python Bindings on top of cppyy",
    long_description_content_type="text/plain",
    url="https://github.com/kunitoki/popsicle",
    packages=setuptools.find_packages(".", exclude=["examples"]),
    package_data={"": package_data},
    include_package_data=True,
    zip_safe=False,
    platforms=["macosx", "win32", "linux"],
    python_requires='>=3.6',
    install_requires=[
        "cppyy>=1.8.0"
    ],
    license='GPLv3',
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
    ],
)

shutil.rmtree(data_folder, ignore_errors=True)
