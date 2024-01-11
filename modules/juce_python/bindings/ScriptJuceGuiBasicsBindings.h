/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include <juce_gui_basics/juce_gui_basics.h>

#include "../utilities/PyBind11Includes.h"

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m);

} // namespace jucepy::Bindings

#endif
