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

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include "ScriptBindings.h"
#include "ScriptException.h"
#include "ScriptUtilities.h"

#include <functional>
#include <string_view>
#include <tuple>

namespace popsicle::Bindings {

//=================================================================================================

ComponentTypeMap& getComponentTypeMap()
{
    static ComponentTypeMap typeMap;
    return typeMap;
}

void registerComponentType (juce::StringRef className, ComponentTypeCaster classCaster)
{
    auto& map = getComponentTypeMap();

    auto lock = juce::CriticalSection::ScopedLockType (map.mutex);
    map.typeMap [className] = std::move (classCaster);
}

void clearComponentTypes()
{
    auto& map = getComponentTypeMap();

    auto lock = juce::CriticalSection::ScopedLockType (map.mutex);
    map.typeMap.clear();
}

} // namespace popsicle::Bindings

#endif
