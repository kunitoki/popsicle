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

#pragma once

#include <juce_core/juce_core.h>

#include "PyBind11Includes.h"

#include <functional>

namespace popsicle::Helpers {

//=================================================================================================

template <class T>
struct NoopDeleter
{
    inline void operator()(T*) const noexcept
    {
    }
};

//=================================================================================================

template <class T, class F>
auto makeVoidPointerAndSizeCallable (F&& func)
{
    namespace py = pybind11;

    if constexpr (std::is_invocable_v<F, T&, void*, size_t>)
    {
        return [func](T* self, py::buffer data)
        {
            auto info = data.request (true);

            using return_value = std::invoke_result_t<F, T&, void*, size_t>;

            if constexpr (std::is_void_v<return_value>)
                std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
            else
                return std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
        };
    }
    else if constexpr (std::is_invocable_v<F, const T&, void*, size_t>)
    {
        return [func](const T* self, py::buffer data)
        {
            auto info = data.request (true);

            using return_value = std::invoke_result_t<F, T&, void*, size_t>;

            if constexpr (std::is_void_v<return_value>)
                std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
            else
                return std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
        };
    }
    else if constexpr (std::is_invocable_v<F, T&, const void*, size_t>)
    {
        return [func](T* self, py::buffer data)
        {
            const auto info = data.request();

            using return_value = std::invoke_result_t<F, T&, const void*, size_t>;

            if constexpr (std::is_void_v<return_value>)
                std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
            else
                return std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
        };
    }
    else if constexpr (std::is_invocable_v<F, const T&, const void*, size_t>)
    {
        return [func](const T* self, py::buffer data)
        {
            const auto info = data.request();

            using return_value = std::invoke_result_t<F, const T&, const void*, size_t>;

            if constexpr (std::is_void_v<return_value>)
                std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
            else
                return std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
        };
    }
    else
    {
        return func;
    }
}

} // namespace popsicle::Helpers
