/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include <juce_gui_basics/juce_gui_basics.h>

#include "../utilities/PyBind11Includes.h"

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m);

} // namespace popsicle::Bindings

#endif
