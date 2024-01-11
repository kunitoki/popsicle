/**
 * juce python - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
 */

#include "../utilities/PyBind11Includes.h"

#include "ScriptJuceCoreBindings.h"
#include "ScriptJuceEventsBindings.h"
#include "ScriptJuceGraphicsBindings.h"
#include "ScriptJuceGuiBasicsBindings.h"

//=================================================================================================
#if JUCE_PYTHON_EMBEDDED_INTERPRETER
PYBIND11_EMBEDDED_MODULE(juce, m)
#else
PYBIND11_MODULE(juce, m)
#endif
{
    // Register juce_core bindings
    jucepy::Bindings::registerJuceCoreBindings (m);

#if __has_include(<juce_events/juce_events.h>)
    // Register juce_events bindings
    jucepy::Bindings::registerJuceEventsBindings (m);
#endif

#if __has_include(<juce_graphics/juce_graphics.h>)
    // Register juce_graphics bindings
    jucepy::Bindings::registerJuceGraphicsBindings (m);
#endif

    // Register juce_gui_basics bindings
#if __has_include(<juce_gui_basics/juce_gui_basics.h>)
    jucepy::Bindings::registerJuceGuiBasicsBindings (m);
#endif
}
