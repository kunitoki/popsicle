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

#include <juce_core/juce_core.h>

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wcast-align")
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4180)

#if JUCE_PYTHON_USE_EXTERNAL_PYBIND11
#include <pybind11/embed.h>

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_CAST)
#include <pybind11/cast.h>
#undef JUCE_PYTHON_INCLUDE_PYBIND11_CAST
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS)
#include <pybind11/operators.h>
#undef JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_STL)
#include <pybind11/stl.h>
#undef JUCE_PYTHON_INCLUDE_PYBIND11_STL
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL)
#include <pybind11/functional.h>
#undef JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM)
#include <pybind11/iostream.h>
#undef JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM
#endif

#else // JUCE_PYTHON_USE_EXTERNAL_PYBIND11
#include "../pybind11/embed.h"

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_CAST)
#include "../pybind11/cast.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_CAST
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS)
#include "../pybind11/operators.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_STL)
#include "../pybind11/stl.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_STL
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL)
#include "../pybind11/functional.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM)
#include "../pybind11/iostream.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM
#endif

#endif

JUCE_END_IGNORE_WARNINGS_GCC_LIKE
JUCE_END_IGNORE_WARNINGS_MSVC
