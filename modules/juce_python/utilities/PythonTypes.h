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

#include <cstdint>
#include <functional>

namespace popsicle {

// =================================================================================================

template <class T>
class GenericInteger
{
public:
    using underlying_type = T;

    constexpr GenericInteger() = default;

    constexpr GenericInteger (T value) noexcept
        : value (value)
    {
    }

    constexpr GenericInteger (const GenericInteger& other) = default;
    constexpr GenericInteger (GenericInteger&& other) = default;
    constexpr GenericInteger& operator= (const GenericInteger& other) = default;
    constexpr GenericInteger& operator= (GenericInteger&& other) = default;

    constexpr operator T() const noexcept
    {
        return value;
    }

    constexpr T get() const noexcept
    {
        return value;
    }

private:
    T value{};
};

// =================================================================================================

template <class T>
struct underlying_type
{
    using type = T;
};

template <class T>
struct underlying_type<GenericInteger<T>>
{
    using type = typename GenericInteger<T>::underlying_type;
};


template <class T>
using underlying_type_t = typename underlying_type<T>::type;

} // namespace popsicle
