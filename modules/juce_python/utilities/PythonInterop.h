/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#include "PyBind11Includes.h"

namespace popsicle::Helpers {

//=================================================================================================

template <class T>
struct CppToPython;

template <>
struct CppToPython<int>
{
    using type = pybind11::int_;
};

template <>
struct CppToPython<float>
{
    using type = pybind11::float_;
};

} // namespace popsicle::Helpers
