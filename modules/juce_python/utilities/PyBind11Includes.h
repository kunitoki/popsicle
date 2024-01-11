
/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#if JUCE_PYTHON_USE_EXTERNAL_PYBIND11
#include <pybind11/embed.h>

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS)
#include <pybind11/operators.h>
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_STL)
#include <pybind11/stl.h>
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL)
#include <pybind11/functional.h>
#endif

#else // JUCE_PYTHON_USE_EXTERNAL_PYBIND11
#include "../pybind11/embed.h"

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS)
#include "../pybind11/operators.h"
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_STL)
#include "../pybind11/stl.h"
#endif

#if defined (JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL)
#include "../pybind11/functional.h"
#endif

#endif
