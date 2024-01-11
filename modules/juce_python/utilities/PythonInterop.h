
/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#include "PyBind11Includes.h"

namespace jucepy::Helpers {

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

} // namespace jucepy::Helpers
