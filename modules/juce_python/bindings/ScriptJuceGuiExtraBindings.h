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

#pragma once

#if !JUCE_MODULE_AVAILABLE_juce_gui_extra
 #error This binding file requires adding the juce_gui_extra module in the project
#else
 #include <juce_gui_extra/juce_gui_extra.h>
#endif

#include "ScriptJuceGuiBasicsBindings.h"

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceGuiExtraBindings (pybind11::module_& m);

// =================================================================================================

template <class Base = juce::AnimatedAppComponent>
struct PyAnimatedAppComponent : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    void update() override
    {
        PYBIND11_OVERRIDE_PURE(void, Base, update);
    }
};

} // namespace popsicle::Bindings
