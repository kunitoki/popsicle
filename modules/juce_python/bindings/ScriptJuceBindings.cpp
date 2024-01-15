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

#include "../utilities/PyBind11Includes.h"

#include "ScriptJuceCoreBindings.h"
#include "ScriptJuceEventsBindings.h"
#include "ScriptJuceDataStructuresBindings.h"
#include "ScriptJuceGraphicsBindings.h"
#include "ScriptJuceGuiBasicsBindings.h"

//=================================================================================================
#if JUCE_PYTHON_EMBEDDED_INTERPRETER
PYBIND11_EMBEDDED_MODULE(popsicle, m)
#else
PYBIND11_MODULE(popsicle, m)
#endif
{
    // Register juce_core bindings
    popsicle::Bindings::registerJuceCoreBindings (m);

#if __has_include(<juce_events/juce_events.h>)
    // Register juce_events bindings
    popsicle::Bindings::registerJuceEventsBindings (m);
#endif

#if __has_include(<juce_data_structures/juce_data_structures.h>)
    // Register juce_data_structures bindings
    popsicle::Bindings::registerJuceDataStructuresBindings (m);
#endif

#if __has_include(<juce_graphics/juce_graphics.h>)
    // Register juce_graphics bindings
    popsicle::Bindings::registerJuceGraphicsBindings (m);
#endif

    // Register juce_gui_basics bindings
#if __has_include(<juce_gui_basics/juce_gui_basics.h>)
    popsicle::Bindings::registerJuceGuiBasicsBindings (m);
#endif
}
