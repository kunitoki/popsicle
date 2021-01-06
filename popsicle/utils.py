import sys
import os
import re
import cppyy
from functools import lru_cache
from itertools import zip_longest


__all__ = [ "juce_bootstrap", "juce_bind" ]


target_paths = [ "RelWithDebInfo", "Release", "Debug", None ]


@lru_cache(maxsize=1024)
def __get_juce_library():
    if sys.platform in ["win32", "cygwin"]:
        return "libpopsicle.dll"

    elif sys.platform in ["darwin"]:
        return "libpopsicle.dylib"

    return "libpopsicle.so"


@lru_cache(maxsize=1024)
def __get_juce_library_path(root_path):
    library_path = os.path.join(root_path, "data", __get_juce_library())
    if os.path.exists(library_path):
        return library_path

    for target in target_paths:
        base_path = os.path.join(root_path, "..", "cxxbuild", "popsicle_artefacts")
        if target:
            base_path = os.path.join(base_path, target)

        library_path = os.path.abspath(os.path.join(base_path, __get_juce_library()))
        if os.path.exists(library_path):
            return library_path

    raise ImportError("Unable to locate juce library path")


@lru_cache(maxsize=1024)
def __get_juce_include_path(root_path):
    include_path = os.path.abspath(os.path.join(root_path, "data"))
    if os.path.isdir(include_path):
        return include_path

    include_path = os.path.abspath(os.path.join(root_path,
        "..", "JUCE", "modules"))

    if os.path.isdir(include_path):
        return include_path

    raise ImportError("Unable to locate juce include path")


@lru_cache(maxsize=1024)
def __get_juce_defs_path(root_path):
    defs_path = os.path.join(root_path, "data", "Defs.txt")
    if os.path.exists(defs_path):
        return defs_path

    for target in target_paths:
        base_path = os.path.join(root_path, "..", "cxxbuild", "popsicle_artefacts", "JuceLibraryCode")
        if target:
            base_path = os.path.join(base_path, target)

        defs_path = os.path.abspath(os.path.join(base_path, "Defs.txt"))
        if os.path.exists(defs_path):
            return defs_path

    raise ImportError("Unable to locate juce definitions text file path")


@lru_cache(maxsize=1024)
def __get_juce_defs(root_path):
    defs_path = __get_juce_defs_path(root_path)

    with open(defs_path, "r") as f:
        defs = [iter(re.split("\x1f|\x1e|\\s", f.read()))] * 2
        defs = zip_longest(fillvalue=None, *defs)
        defs = list(filter(lambda v: v[0] == "MODULE_DEFINITIONS", defs))[0]
        defs = map(lambda d: "#define {} {}".format(*d.split("=")), defs[1].split(";"))
        defs = "\n".join(defs)

    return defs


@lru_cache(maxsize=1024)
def juce_bootstrap():
    root_path = os.path.abspath(os.path.join(os.path.dirname(__file__)))

    cppyy.add_include_path(__get_juce_include_path(root_path))

    cppyy.load_library(__get_juce_library_path(root_path))

    cppyy.cppdef(__get_juce_defs(root_path))

    if sys.platform in ["darwin"]:
        cppyy.cppdef("namespace juce { extern void initialiseNSApplication(); }")


def juce_bind(obj, method):
    setattr(obj, "__{}".format(method.__name__), method)
    return method
