import sys as __sys
import os as __os
import re as __re
import glob as __glob
import cppyy as __cppyy
from functools import lru_cache as __lru_cache
from itertools import zip_longest as __zip_longest


__all__ = [ "juce_bootstrap" ]

__target_paths = [ "RelWithDebInfo", "Release", "Debug", None ]


@__lru_cache(maxsize=1024)
def __get_juce_library():
    if __sys.platform in ["win32", "cygwin"]:
        return "popsicle.dll"

    elif __sys.platform in ["darwin"]:
        return "libpopsicle.dylib"

    return "libpopsicle.so"


@__lru_cache(maxsize=1024)
def __get_juce_library_path(root_path):
    library_path = __os.path.join(root_path, "data", __get_juce_library())
    if __os.path.exists(library_path):
        return library_path

    for target in __target_paths:
        base_path = __glob.glob(__os.path.join(root_path, "..", "build", "**", "popsicle_artefacts"))
        if base_path: base_path = base_path[0]
        if target: base_path = __os.path.join(base_path, target)

        library_path = __os.path.abspath(__os.path.join(base_path, __get_juce_library()))
        if __os.path.exists(library_path):
            return library_path

    raise ImportError("Unable to locate juce library path")


@__lru_cache(maxsize=1024)
def __get_juce_include_path(root_path):
    include_path = __os.path.abspath(__os.path.join(root_path, "data"))
    if __os.path.isdir(include_path):
        return include_path

    include_path = __os.path.abspath(__os.path.join(root_path, "..", "JUCE", "modules"))

    if __os.path.isdir(include_path):
        return include_path

    raise ImportError("Unable to locate juce include path")


@__lru_cache(maxsize=1024)
def __get_juce_defs_path(root_path):
    defs_path = __os.path.join(root_path, "data", "Defs.txt")
    if __os.path.exists(defs_path):
        return defs_path

    for target in __target_paths:
        base_path = __glob.glob(__os.path.join(root_path, "..", "build", "**", "popsicle_artefacts", "JuceLibraryCode"))
        if base_path: base_path = base_path[0]
        if target: base_path = __os.path.join(base_path, target)

        defs_path = __os.path.abspath(__os.path.join(base_path, "Defs.txt"))
        if __os.path.exists(defs_path):
            return defs_path

    raise ImportError("Unable to locate juce definitions text file path")


@__lru_cache(maxsize=1024)
def __get_juce_defs(root_path):
    defs_path = __get_juce_defs_path(root_path)

    with open(defs_path, "r") as f:
        defs = [iter(__re.split("\x1f|\x1e|\\s", f.read()))] * 2
        defs = __zip_longest(fillvalue=None, *defs)
        defs = list(filter(lambda v: v[0] == "MODULE_DEFINITIONS", defs))[0]
        defs = map(lambda d: "#define {} {}".format(*d.split("=")), defs[1].split(";"))
        defs = "\n".join(defs)

    return defs


@__lru_cache(maxsize=1024)
def juce_bootstrap():
    root_path = __os.path.abspath(__os.path.join(__os.path.dirname(__file__)))

    __cppyy.add_include_path(__get_juce_include_path(root_path))

    __cppyy.load_library(__get_juce_library_path(root_path))

    __cppyy.cppdef(__get_juce_defs(root_path))

    if __sys.platform in ["darwin"]:
        __cppyy.cppdef("namespace juce { extern void initialiseNSApplication(); }")
