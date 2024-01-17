/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2022 - kunitoki <kunitoki@gmail.com>
 *
 * popsicle is an open source library subject to commercial or open-source licensing.
 *
 * By using popsicle, you agree to the terms of the popsicle License Agreement, which can
 * be found at https://raw.githubusercontent.com/kunitoki/popsicle/master/LICENSE
 *
 * Or: You may also use this code under the terms of the GPL v3 (see www.gnu.org/licenses).
 *
 * POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER EXPRESSED
 * OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE DISCLAIMED.
 */

#if JUCE_PYTHON_EMBEDDED_INTERPRETER

#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include <iostream>
#include <string>

PYBIND11_EMBEDDED_MODULE(__popsicle__, m)
{
    namespace py = pybind11;

    struct CustomOutputStream
    {
        CustomOutputStream() = default;
        CustomOutputStream (const CustomOutputStream&) = default;
        CustomOutputStream (CustomOutputStream&&) = default;
    };

    py::class_<CustomOutputStream> classCustomOutputStream (m, "__stdout__");
    classCustomOutputStream.def_static ("write", [](py::object buffer) { std::cout << buffer.cast<std::string>(); });
    classCustomOutputStream.def_static ("flush", [] { std::cout << std::flush; });

    struct CustomErrorStream
    {
        CustomErrorStream() = default;
        CustomErrorStream (const CustomErrorStream&) = default;
        CustomErrorStream (CustomErrorStream&&) = default;
    };

    py::class_<CustomErrorStream> classCustomErrorStream (m, "__stderr__");
    classCustomErrorStream.def_static ("write", [](py::object buffer) { std::cerr << buffer.cast<std::string>(); });
    classCustomErrorStream.def_static ("flush", [] { std::cerr << std::flush; });

    m.def ("__redirect__", []
    {
        auto sys = py::module::import ("sys");
        auto popsicleSys = py::module::import ("__popsicle__");

        popsicleSys.attr ("__saved_stdout__") = sys.attr ("stdout");
        popsicleSys.attr ("__saved_stderr__") = sys.attr ("stderr");
        sys.attr ("stdout") = popsicleSys.attr ("__stdout__");
        sys.attr ("stderr") = popsicleSys.attr ("__stderr__");
    });

    m.def ("__restore__", []
    {
        auto sys = py::module::import ("sys");
        auto popsicleSys = py::module::import ("__popsicle__");

        sys.attr ("stdout") = popsicleSys.attr ("__saved_stdout__");
        sys.attr ("stderr") = popsicleSys.attr ("__saved_stderr__");
    });
}

#endif
