/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#pragma once

#if __has_include(<juce_graphics/juce_graphics.h>)

#include <juce_graphics/juce_graphics.h>

#include "../utilities/PyBind11Includes.h"

//=================================================================================================

namespace jucepy::Bindings {

void registerJuceGraphicsBindings (pybind11::module_& m);

} // namespace jucepy::Bindings

#endif
