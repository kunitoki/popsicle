/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#if __has_include(<juce_graphics/juce_graphics.h>)

#include <juce_graphics/juce_graphics.h>

#include "../utilities/PyBind11Includes.h"

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceGraphicsBindings (pybind11::module_& m);

} // namespace popsicle::Bindings

#endif
