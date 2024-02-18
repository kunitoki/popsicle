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

#include <juce_python/juce_python.h>

#include "PyBind11Includes.h"
#include "ClassDemangling.h"

#include <functional>

// =================================================================================================

PYBIND11_DECLARE_HOLDER_TYPE(T, juce::ReferenceCountedObjectPtr<T>, true)

namespace popsicle::Helpers {

// =================================================================================================

inline void printPythonException (const pybind11::error_already_set& e)
{
    pybind11::print ("Traceback (most recent call last):");

    pybind11::module_::import ("traceback").attr ("print_tb") (e.trace());

    pybind11::print (e.what());
}

// =================================================================================================

template <class T, class F>
auto makeRepr (F&& func)
{
    static_assert (std::is_invocable_v<F, const T&>);

    return [func](const T& instance)
    {
        juce::String result;

        result
            << pythonizeCompoundClassName (PythonModuleName, typeid (instance).name())
            << "('" << std::invoke (func, instance) << "')";

        return result;
    };
}

// =================================================================================================

template <class E>
pybind11::enum_<E> makeArithmeticEnum (pybind11::object& parent, const char* name)
{
    jassert (name != nullptr);

    using T = std::underlying_type_t<E>;

    pybind11::enum_<E> classEnum (parent, name);

    classEnum
        .def ("__and__", [](E lhs, E rhs) { return (static_cast<T> (lhs) & static_cast<T> (rhs)); })
        .def ("__eq__", [](E lhs, E rhs) { return (static_cast<T> (lhs) == static_cast<T> (rhs)); })
        .def ("__eq__", [](E lhs, T rhs) { return (static_cast<T> (lhs) == rhs); })
        .def ("__ge__", [](E lhs, E rhs) { return (static_cast<T> (lhs) >= static_cast<T> (rhs)); })
        .def ("__ge__", [](E lhs, T rhs) { return (static_cast<T> (lhs) >= rhs); })
        .def ("__gt__", [](E lhs, E rhs) { return (static_cast<T> (lhs) > static_cast<T> (rhs)); })
        .def ("__gt__", [](E lhs, T rhs) { return (static_cast<T> (lhs) > rhs); })
        .def ("__hash__", [](E lhs) { return static_cast<T> (lhs); })
        .def ("__int__", [](E lhs) { return static_cast<T> (lhs); })
        .def ("__invert__", [](E lhs) { return ~static_cast<T> (lhs); })
        .def ("__le__", [](E lhs, E rhs) { return (static_cast<T> (lhs) <= static_cast<T> (rhs)); })
        .def ("__le__", [](E lhs, T rhs) { return (static_cast<T> (lhs) <= rhs); })
        .def ("__lt__", [](E lhs, E rhs) { return (static_cast<T> (lhs) < static_cast<T> (rhs)); })
        .def ("__lt__", [](E lhs, T rhs) { return (static_cast<T> (lhs) < rhs); })
        .def ("__ne__", [](E lhs, E rhs) { return (static_cast<T> (lhs) != static_cast<T> (rhs)); })
        .def ("__ne__", [](E lhs, T rhs) { return (static_cast<T> (lhs) != rhs); })
        .def ("__or__", [](E lhs, E rhs) { return (static_cast<T> (lhs) | static_cast<T> (rhs)); })
        .def ("__xor__", [](E lhs, E rhs) { return (static_cast<T> (lhs) ^ static_cast<T> (rhs)); })
    ;

    return classEnum;
}

// =================================================================================================

template <class T, class F>
auto makeVoidPointerAndSizeCallable (F&& func)
{
    if constexpr (std::is_invocable_v<F, T&, const void*, size_t>)
    {
        return [func](T* self, pybind11::buffer data)
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
        return [func](const T* self, pybind11::buffer data)
        {
            const auto info = data.request();

            using return_value = std::invoke_result_t<F, const T&, const void*, size_t>;

            if constexpr (std::is_void_v<return_value>)
                std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
            else
                return std::invoke (func, self, info.ptr, static_cast<size_t> (info.size));
        };
    }
    else if constexpr (std::is_invocable_v<F, T&, void*, size_t>)
    {
        return [func](T* self, pybind11::buffer data)
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
        return [func](const T* self, pybind11::buffer data)
        {
            auto info = data.request (true);

            using return_value = std::invoke_result_t<F, T&, void*, size_t>;

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
