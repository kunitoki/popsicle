import glob
import subprocess
import re
import os
import sys

from itertools import zip_longest

def generate_pyi(where):
    try:
        import CppHeaderParser
    except ImportError:
        return

    command = [
        "pcpp",
        "--passthru-unfound-includes",
        "-o",
        "build/juce_module.h",
        "-D", "JUCE_API=", "-D", "JUCE_STRING_UTF_TYPE=8"
    ]

    if sys.platform in ["win32", "cygwin"]:
        command += [ "-D", "WIN32=1", "-D", "_MSC_VER=1" ]

    elif sys.platform in ["darwin"]:
        command += [ "-D", "APPLE=1", "-D", "__APPLE_CPP__=1", "-D", "__clang__=1", "-D", "MAC_OS_X_VERSION_10_11=1"]

    else:
        command += [ "-D", "LINUX=1", "-D", "__linux__=1" ]

    defs = []
    for file in glob.glob("build/**/popsicle_artefacts/JuceLibraryCode/**/Defs.txt"):
        with open(file, "r") as f:
            module_defs = [iter(re.split("\x1f|\x1e|\\s", f.read()))] * 2
            module_defs = zip_longest(fillvalue=None, *module_defs)
            module_defs = list(filter(lambda v: v[0] == "MODULE_DEFINITIONS", module_defs))[0]
            for md in module_defs[1].split(";"):
                defs += [ "-D", md ]
        break

    command = command + defs
    lines = []
    classes_added = []

    for header in glob.iglob("JUCE/modules/*/*/*.h"):
        if "/native/" in header \
                or "/juce_audio_plugin_client/" in header \
                or "/juce_audio_processors/format_types/" in header \
                or "juce_PlatformDefs.h" in header:
            continue

        try:
            shutil.rmtree("build/juce_module.h", ignore_errors=True)

            subprocess.check_call(command + [header])

            with open("build/juce_module.h", "r") as f: text = f.read()
            with open("build/juce_module.h", "w") as f:
                text = re.sub(r"JUCE_DEPRECATED_STATIC\s*\(.*\)", "", text)
                text = re.sub(r"JUCE_DECLARE_DEPRECATED_STATIC\s*\(.*\)", "", text)
                text = re.sub(r"JUCE_BEGIN_IGNORE_WARNINGS_MSVC\s*\(.*\)", "", text)
                text = re.sub(r"JUCE_DECLARE_SINGLETON\s*\(.*\)", "", text)
                text = re.sub(r"JUCE_DECLARE_SINGLETON_SINGLETHREADED_MINIMAL\s*\(.*\)", "", text)
                text = re.sub(r"JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR\s*\(.*\)", "", text)
                f.write(text)

            header_object = CppHeaderParser.CppHeader("build/juce_module.h")

            for _, klass in header_object.classes.items():
                classname = klass["name"]
                namespace = klass["namespace"]

                # Skip non juce namespace
                if namespace != "juce":
                    continue

                # Cleanup classname
                classname = re.sub(r"(\w*)<.*>", r"\1", classname)
                if classname in classes_added:
                    continue

                # Inheritance
                inherits = []
                for inherit_from in klass["inherits"]:
                    inherits.append(inherit_from["decl_name"].replace("::", "."))
                if not inherits:
                    inherits = "object"
                else:
                    inherits = ", ".join(inherits)

                methods_count = 0
                has_constructor = False

                classes_added.append(classname)

                lines.append(f"class {classname}({inherits}):")

                # Process methods
                for meth in klass["methods"]["public"]:
                    methodname = meth["name"]
                    methodname = methodname.replace("*", "")

                    if methodname.startswith("operator") \
                            or methodname.startswith("JUCE_DEPRECATED") \
                            or methodname.startswith("function<") \
                            or methodname[0].isupper():
                        continue

                    if methodname == classname:
                        has_constructor = True
                        continue

                    args_count = 0

                    args = [ "self" ]
                    for arg in meth["parameters"]:
                        argname = arg["name"].replace("&", "").strip()
                        argname = argname.replace(")", "").strip()
                        if not argname or argname == "uint8": argname = f"arg{args_count}"
                        args.append(argname)
                        args_count += 1

                    args = ", ".join(args)

                    lines.append(f"    def {methodname}({args}): ...")
                    methods_count += 1

                if has_constructor:
                    lines.append("    def __init__(self, *args): ...")
                    methods_count += 1

                if methods_count == 0:
                    lines.append("    pass")

                lines.append("")

        except Exception as e:
            print(e)

    with open(os.path.join(where, "juce.pyi"), "w") as f:
        f.writelines("\n".join(lines))
