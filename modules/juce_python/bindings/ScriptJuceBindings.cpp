/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2024 - kunitoki <kunitoki@gmail.com>
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
#include "ScriptJuceGuiEntryPointsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_extra
#include "ScriptJuceGuiExtraBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_basics
#include "ScriptJuceAudioBasicsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_devices
#include "ScriptJuceAudioDevicesBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
#include "ScriptJuceAudioProcessorsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_formats
#include "ScriptJuceAudioFormatsBindings.h"
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_utils
#include "ScriptJuceAudioUtilsBindings.h"
#endif

// =================================================================================================
#if JUCE_PYTHON_EMBEDDED_INTERPRETER
PYBIND11_EMBEDDED_MODULE (JUCE_PYTHON_MODULE_NAME, m)
#else
PYBIND11_MODULE (JUCE_PYTHON_MODULE_NAME, m)
#endif
{
    popsicle::Bindings::registerJuceCoreBindings (m);

#if JUCE_MODULE_AVAILABLE_juce_events
    popsicle::Bindings::registerJuceEventsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_data_structures
    popsicle::Bindings::registerJuceDataStructuresBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_graphics
    popsicle::Bindings::registerJuceGraphicsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_basics
    popsicle::Bindings::registerJuceGuiBasicsBindings (m);
    popsicle::Bindings::registerJuceGuiEntryPointsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_gui_extra
    popsicle::Bindings::registerJuceGuiExtraBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_basics
    popsicle::Bindings::registerJuceAudioBasicsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_devices
    popsicle::Bindings::registerJuceAudioDevicesBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_processors
    popsicle::Bindings::registerJuceAudioProcessorsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_formats
    popsicle::Bindings::registerJuceAudioFormatsBindings (m);
#endif

#if JUCE_MODULE_AVAILABLE_juce_audio_utils
    popsicle::Bindings::registerJuceAudioUtilsBindings (m);
#endif
}
