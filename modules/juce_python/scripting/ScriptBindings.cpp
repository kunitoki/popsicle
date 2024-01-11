/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include "ScriptBindings.h"
#include "ScriptException.h"
#include "ScriptUtilities.h"

#include <functional>
#include <string_view>
#include <tuple>

namespace jucepy::Bindings {

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

} // namespace jucepy::Bindings

#endif
