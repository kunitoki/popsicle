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

#if JUCE_MODULE_AVAILABLE_juce_gui_basics

#include <juce_gui_basics/juce_gui_basics.h>

#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <typeinfo>
#include <unordered_map>

namespace popsicle {

//=================================================================================================

/**
 * @brief Define a type alias for a function that casts a JUCE Component to a pointer of a specified type.
 *
 * This type alias defines a function pointer named `ComponentTypeCaster`. This function is designed to take a JUCE Component
 * and a reference to a `std::type_info` object, and it returns a constant pointer to void. The purpose of this function is to enable
 * dynamic casting of a JUCE Component to a specific derived class type. It can be used for tasks like type-safe downcasting of
 * components in a straw application when obtaining Components in scripts and it's used by pybind11.
 *
 * @warning Be cautious when using this function as it deals with low-level type information and dynamic casting.
 *
 * @param component A pointer to a JUCE Component that you want to cast to a specific type.
 * @param typeInfo  A reference to a `std::type_info` object representing the target type.
 *
 * @return A constant pointer to void that can be interpreted as a pointer to an instance of the target type
 *         if the cast is successful. If the cast fails, it may return a null pointer.
 */
using ComponentTypeCaster = std::function<const void* (const juce::Component*, const std::type_info*&)>;

//=================================================================================================

namespace Bindings {

/**
 * @brief A structure for managing component type mappings.
 *
 * This structure is used to store mappings between class names and ComponentTypeCaster functions, allowing for dynamic casting of
 * Component objects to their derived types.
 */
struct ComponentTypeMap
{
    juce::CriticalSection mutex;
    std::unordered_map<juce::String, ComponentTypeCaster> typeMap;
};

/**
 * @brief Get the global ComponentTypeMap instance.
 *
 * @return A reference to the global ComponentTypeMap instance.
 */
ComponentTypeMap& getComponentTypeMap();

/**
 * @brief Register a component type caster for a specific class.
 *
 * This function registers a component type caster for the specified class name.
 *
 * @param className The name of the class to register the caster for.
 * @param classCaster The component type caster function for the class.
 */
void registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster);

/**
 * @brief Clear all registered component types.
 *
 * This function clears all the registered component type mappings in the ComponentTypeMap.
 */
void clearComponentTypes();

} // namespace Bindings

//=================================================================================================

/**
 * @brief Template function for casting a Component to a derived type.
 *
 * This template function is used to cast a Component to a derived type. It performs a `dynamic_cast` and returns a pointer to the
 * derived type if the cast is successful, or nullptr otherwise.
 *
 * @tparam T The derived type to cast to.
 *
 * @param src The source Component to cast.
 * @param type A pointer to a std::type_info object that will be set to the type of the derived class if the cast is successful.

 * @return A pointer to the derived type if the cast is successful, or nullptr otherwise.
 */
template <class T>
const void* ComponentType (const juce::Component* src, const std::type_info*& type)
{
    static_assert (std::is_base_of_v<juce::Component, T>, "Invalid unrelated polymorphism between classes");

    if (auto result = dynamic_cast<const T*> (src))
    {
        type = &typeid(T);
        return result;
    }

    return nullptr;
}

} // namespace popsicle

#endif
