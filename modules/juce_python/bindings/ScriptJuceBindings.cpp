/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
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
