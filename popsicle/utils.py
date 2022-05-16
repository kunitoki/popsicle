import sys
import os
import re
import glob
import cppyy
from functools import lru_cache
from itertools import zip_longest


__all__ = [ "juce_bootstrap" ]

__target_paths = [ "RelWithDebInfo", "Release", "Debug", None ]


@lru_cache(maxsize=1024)
def __get_juce_library() -> str:
    if sys.platform in ["win32", "cygwin"]:
        return "popsicle.dll"

    elif sys.platform in ["darwin"]:
        return "libpopsicle.dylib"

    return "libpopsicle.so"


@lru_cache(maxsize=1024)
def __get_juce_library_path(root_path: str) -> str:
    library_path = os.path.join(root_path, "data", __get_juce_library())
    if os.path.exists(library_path):
        return library_path

    for target in __target_paths:
        base_path = glob.glob(os.path.join(root_path, "..", "build", "**", "popsicle_artefacts"))
        if not base_path:
            continue

        current_base_path = base_path[0]
        if target: current_base_path = os.path.join(current_base_path, target)

        library_path = os.path.abspath(os.path.join(current_base_path, __get_juce_library()))
        if os.path.exists(library_path):
            return library_path

    raise ImportError("Unable to locate juce library path")


@lru_cache(maxsize=1024)
def __get_juce_include_path(root_path: str) -> str:
    include_path = os.path.abspath(os.path.join(root_path, "data"))
    if os.path.isdir(include_path):
        return include_path

    include_path = os.path.abspath(os.path.join(root_path, "..", "JUCE", "modules"))

    if os.path.isdir(include_path):
        return include_path

    raise ImportError("Unable to locate juce include path")


@lru_cache(maxsize=1024)
def __get_juce_defs_path(root_path: str) -> str:
    defs_path = os.path.join(root_path, "data", "Defs.txt")
    if os.path.exists(defs_path):
        return defs_path

    for target in __target_paths:
        base_path = glob.glob(os.path.join(root_path, "..", "build", "**", "popsicle_artefacts", "JuceLibraryCode"))
        if not base_path:
            continue

        current_base_path = base_path[0]
        if target: current_base_path = os.path.join(current_base_path, target)

        defs_path = os.path.abspath(os.path.join(current_base_path, "Defs.txt"))
        if os.path.exists(defs_path):
            return defs_path

    raise ImportError("Unable to locate juce definitions text file path")


@lru_cache(maxsize=1024)
def __get_juce_defs(root_path: str) -> str:
    defs_path = __get_juce_defs_path(root_path)

    with open(defs_path, "r") as f:
        defs = [iter(re.split("\x1f|\x1e|\\s", f.read()))] * 2
        defs = zip_longest(*defs, fillvalue=None)
        defs = list(filter(lambda v: v[0] == "MODULE_DEFINITIONS", defs))[0]
        defs = list(map(lambda d: "#define {} {}".format(*d.split("=")), defs[1].split(";")))

    return "\n".join(defs)


@lru_cache(maxsize=1024)
def juce_bootstrap():
    root_path = os.path.abspath(os.path.join(os.path.dirname(__file__)))

    include_path = __get_juce_include_path(root_path)
    library_path = __get_juce_library_path(root_path)
    defines = __get_juce_defs(root_path)

    cppyy.add_include_path(include_path)
    cppyy.load_library(library_path)
    cppyy.cppdef(defines)

    if sys.platform in ["darwin"]:
        cppyy.cppdef("namespace juce { extern void initialiseNSApplication(); }")
