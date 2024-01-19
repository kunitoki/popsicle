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

#define JUCE_PYTHON_INCLUDE_PYBIND11_STL
#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <utility>

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceCoreBindings (pybind11::module_& m);

} // namespace popsicle::Bindings

//=================================================================================================

namespace PYBIND11_NAMESPACE {
namespace detail {

//=================================================================================================

template <>
struct type_caster<juce::StringRef>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringRef, const_name (PYBIND11_STRING_NAME));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringRef& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

//=================================================================================================

template <>
struct type_caster<juce::String>
{
public:
    PYBIND11_TYPE_CASTER (juce::String, const_name (PYBIND11_STRING_NAME));

    bool load (handle src, bool convert);

    static handle cast (const juce::String& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

//=================================================================================================

template <>
struct type_caster<juce::Identifier> : public type_caster_base<juce::Identifier>
{
    using base_type = type_caster_base<juce::Identifier>;

public:
    PYBIND11_TYPE_CASTER (juce::Identifier, const_name ("popsicle.Identifier"));

    bool load (handle src, bool convert);

    static handle cast (const juce::Identifier& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

//=================================================================================================

template <>
struct type_caster<juce::var>
{
public:
    PYBIND11_TYPE_CASTER (juce::var, const_name ("popsicle.var"));

    bool load (handle src, bool convert);

    static handle cast (const juce::var& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::StringArray>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringArray, const_name ("popsicle.StringArray"));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringArray& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::NamedValueSet>
{
public:
    PYBIND11_TYPE_CASTER (juce::NamedValueSet, const_name ("popsicle.NamedValueSet"));

    bool load (handle src, bool convert);

    static handle cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <class T>
struct type_caster<juce::Array<T>>
{
    using value_conv = make_caster<T>;

public:
    PYBIND11_TYPE_CASTER (juce::Array<T>, const_name("popsicle.Array[") + value_conv::name + const_name("]"));

    bool load (handle src, bool convert)
    {
        if (! isinstance<list> (src))
            return false;

        value.clear();

        auto l = reinterpret_borrow<list> (src);
        for (auto it : l)
        {
            make_caster<T> conv;

            if (! conv.load (it.ptr(), convert))
                return false;

            value.add (cast_op<T&&> (std::move (conv)));
        }

        return true;
    }

    template <class U>
    static handle cast(U&& src, return_value_policy policy, handle parent)
    {
        if (! std::is_lvalue_reference_v<U>)
            policy = return_value_policy_override<T>::policy(policy);

        list l (src.size());
        ssize_t index = 0;

        for (auto&& value : src)
        {
            auto value_object = reinterpret_steal<object>(
                value_conv::cast (forward_like<U> (value), policy, parent));

            if (! value_object)
                return handle();

            PyList_SET_ITEM(l.ptr(), index++, value_object.release().ptr()); // steals a reference
        }

        return l.release();
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE
