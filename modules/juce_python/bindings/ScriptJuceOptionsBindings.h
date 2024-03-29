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

#include <juce_core/juce_core.h>

#include "../utilities/PyBind11Includes.h"

#include <atomic>

namespace popsicle::Bindings {

// =================================================================================================

struct Options
{
    std::atomic_bool catchExceptionsAndContinue = false;
    std::atomic_bool caughtKeyboardInterrupt = false;
    std::atomic_int messageManagerGranularityMilliseconds = 200;
};

Options& globalOptions() noexcept;

// =================================================================================================

void registerJuceOptionsBindings (pybind11::module_& m);

} // namespace popsicle::Bindings
