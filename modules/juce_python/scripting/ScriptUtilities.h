/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
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

#pragma once

#include "../utilities/PyBind11Includes.h"

#include <optional>

namespace popsicle {

/**
 * @brief
 */
template <class T>
std::optional<T> python_cast (const pybind11::object& value)
{
    try
    {
        return value.cast<T>();
    }
    catch (const pybind11::cast_error& e)
    {
        return std::nullopt;
    }
}

/**
 * @brief
 */
struct ScriptStreamRedirection
{
    ScriptStreamRedirection() noexcept;
    ~ScriptStreamRedirection() noexcept;

private:
    pybind11::object sys;
};

} // namespace popsicle
