/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#if JUCE_PYTHON_USE_EXTERNAL_PYBIND11
#include <pybind11/embed.h>

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
