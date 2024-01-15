/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
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
    PYBIND11_TYPE_CASTER (juce::StringRef, const_name ("StringRef"));

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
    PYBIND11_TYPE_CASTER (juce::String, const_name ("String"));

    bool load (handle src, bool convert);

    static handle cast (const juce::String& src, return_value_policy policy, handle parent);

private:
    bool load_raw (handle src);
};

//=================================================================================================

template <>
struct type_caster<juce::NewLine>
{
public:
    PYBIND11_TYPE_CASTER (juce::NewLine, const_name ("NewLine"));

    static handle cast (const juce::NewLine& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::Identifier>
{
public:
    PYBIND11_TYPE_CASTER (juce::Identifier, const_name ("Identifier"));

    bool load (handle src, bool convert);

    static handle cast (const juce::Identifier& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::var>
{
public:
    PYBIND11_TYPE_CASTER (juce::var, const_name ("var"));

    bool load (handle src, bool convert);

    static handle cast (const juce::var& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::StringArray>
{
public:
    PYBIND11_TYPE_CASTER (juce::StringArray, const_name ("StringArray"));

    bool load (handle src, bool convert);

    static handle cast (const juce::StringArray& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <>
struct type_caster<juce::NamedValueSet>
{
public:
    PYBIND11_TYPE_CASTER (juce::NamedValueSet, const_name ("NamedValueSet[Identifier, var]"));

    bool load (handle src, bool convert);

    static handle cast (const juce::NamedValueSet& src, return_value_policy policy, handle parent);
};

//=================================================================================================

template <class T>
struct type_caster<juce::Array<T>>
{
    using value_conv = make_caster<T>;

public:
    PYBIND11_TYPE_CASTER (juce::Array<T>, const_name("Array[") + value_conv::name + const_name("]"));

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
