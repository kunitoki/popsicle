/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
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
