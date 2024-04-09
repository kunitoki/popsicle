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

/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_python
  vendor:             kunitoki
  version:            0.9.7
  name:               Python bindings for the JUCE framework
  description:        The python bindings to create and work on JUCE apps.
  website:            https://github.com/kunitoki/popsicle
  license:            DUAL
  minimumCppStandard: 17

  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION
*/

#pragma once

//==============================================================================
/** Config: JUCE_PYTHON_USE_EXTERNAL_PYBIND11

    Enable externally provided pybind11 installation.
*/
#ifndef JUCE_PYTHON_USE_EXTERNAL_PYBIND11
 #define JUCE_PYTHON_USE_EXTERNAL_PYBIND11 0
#endif

//==============================================================================
/** Config: JUCE_PYTHON_EMBEDDED_INTERPRETER

    Enable or disable embedding the interpreter. This should be disabled when building standalone wheels.
*/
#ifndef JUCE_PYTHON_EMBEDDED_INTERPRETER
 #define JUCE_PYTHON_EMBEDDED_INTERPRETER 1
#endif

//==============================================================================
/** Config: JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION

    Enable or disable catching script exceptions.
*/
#ifndef JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
 #define JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION 1
#endif

//==============================================================================
/** Config: JUCE_PYTHON_THREAD_CATCH_EXCEPTION

    Enable or disable catching juce::Thread exceptions raised from python.
*/
#ifndef JUCE_PYTHON_THREAD_CATCH_EXCEPTION
 #define JUCE_PYTHON_THREAD_CATCH_EXCEPTION 1
#endif

//==============================================================================

#include "utilities/MacroHelpers.h"

/**
 * @brief Custom module name, it's possible to change but beware to update your `import` statements !
 */
#ifndef JUCE_PYTHON_MODULE_NAME
 #define JUCE_PYTHON_MODULE_NAME popsicle
#endif

/**
 * @brief Custom logs module name, it's possible to change but beware to update your `import` statements !
 */
#ifndef JUCE_PYTHON_LOGS_MODULE_NAME
 #define JUCE_PYTHON_LOGS_MODULE_NAME __popsicle__
#endif

/**
 * @brief Custom python module name as string.
 */
namespace popsicle {

static inline constexpr const char* const PythonModuleName = JUCE_PYTHON_STRINGIFY (JUCE_PYTHON_MODULE_NAME);
static inline constexpr const char* const PythonLogsModuleName = JUCE_PYTHON_STRINGIFY (JUCE_PYTHON_LOGS_MODULE_NAME);

} // namespace popsicle

//==============================================================================
/**
 * @brief Modal loops are required for juce python to work when built as a wheel.
 */
#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER && ! JUCE_MODAL_LOOPS_PERMITTED
 #error When building juce_python with JUCE_PYTHON_EMBEDDED_INTERPRETER=0 it is mandatory to also set JUCE_MODAL_LOOPS_PERMITTED=1
#endif

//==============================================================================
/**
 * @brief Use `JUCE_PYTHON_DEFINE_EMBEDDED_MODULES` when defining a plugin.
 *
 * Because JUCE plugins are compiled with a shared static library, the embedded modules defined in the library are discarded by the linker
 * when building the final plugin shared library.
 *
 * @see https://github.com/pybind/pybind11/discussions/4619 for moe information.
 */
#if JUCE_MODULE_AVAILABLE_juce_audio_plugin_client
 #define JUCE_PYTHON_DEFINE_EMBEDDED_MODULES                                                                 \
    PYBIND11_EMBEDDED_MODULE (JUCE_PYTHON_MODULE_NAME, m) { popsicle::registerMainModule (m); }              \
    PYBIND11_EMBEDDED_MODULE (JUCE_PYTHON_LOGS_MODULE_NAME, m) { popsicle::registerLoggingModule (m); }
#else
 #define JUCE_PYTHON_DEFINE_EMBEDDED_MODULES
#endif

//==============================================================================

#include "scripting/ScriptException.h"
#include "scripting/ScriptEngine.h"
#include "scripting/ScriptBindings.h"
#include "scripting/ScriptUtilities.h"
#include "utilities/ClassDemangling.h"
#include "utilities/CrashHandling.h"
#include "utilities/PythonInterop.h"

#include "bindings/ScriptJuceBindings.h"
#include "bindings/ScriptJuceCoreBindings.h"

#include "modules/ScriptPopsicleLogsModule.h"
