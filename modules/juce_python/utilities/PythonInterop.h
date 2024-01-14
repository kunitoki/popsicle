/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#include <juce_core/juce_core.h>

#include "PyBind11Includes.h"

namespace popsicle::Helpers {

//=================================================================================================

template <class T>
struct CppToPython;

template <>
struct CppToPython<bool>
{
    using type = pybind11::bool_;
};

template <>
struct CppToPython<int>
{
    using type = pybind11::int_;
};

template <>
struct CppToPython<juce::int64>
{
    using type = pybind11::int_;
};

template <>
struct CppToPython<float>
{
    using type = pybind11::float_;
};

template <>
struct CppToPython<double>
{
    using type = pybind11::float_;
};

template <>
struct CppToPython<juce::String>
{
    using type = pybind11::str;
};

} // namespace popsicle::Helpers
