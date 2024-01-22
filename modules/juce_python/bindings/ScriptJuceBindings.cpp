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

#if JUCE_MODULE_AVAILABLE_juce_events
#include "ScriptJuceEventsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_data_structures
#include "ScriptJuceDataStructuresBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_graphics
#include "ScriptJuceGraphicsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_basics
#include "ScriptJuceGuiBasicsBindings.h"
#endif

//=================================================================================================
#if JUCE_PYTHON_EMBEDDED_INTERPRETER
PYBIND11_EMBEDDED_MODULE (popsicle, m)
#else
PYBIND11_MODULE (popsicle, m)
#endif
{
    // Register juce_core bindings
    popsicle::Bindings::registerJuceCoreBindings (m);

#if JUCE_MODULE_AVAILABLE_juce_events
    // Register juce_events bindings
    popsicle::Bindings::registerJuceEventsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_data_structures
    // Register juce_data_structures bindings
    popsicle::Bindings::registerJuceDataStructuresBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_graphics
    // Register juce_graphics bindings
    popsicle::Bindings::registerJuceGraphicsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_basics
    // Register juce_gui_basics bindings
    popsicle::Bindings::registerJuceGuiBasicsBindings (m);
#endif
}
