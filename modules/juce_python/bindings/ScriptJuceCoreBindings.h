/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#include <juce_core/juce_core.h>

#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <typeinfo>

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

    handle cast (const juce::Array<T>& src, return_value_policy policy, handle parent)
    {
        list l;

        for (const auto& arrayItem : src)
        {
            auto item = reinterpret_steal<object> (make_caster<T>::cast (arrayItem, policy, parent));

            if (! item)
                return handle();

            l.append (std::move (item));
        }

        return l.release();
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE
