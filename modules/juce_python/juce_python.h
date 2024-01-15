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

/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_python
  vendor:             kunitoki
  version:            1.0.3
  name:               Python bindings for the JUCE framework
  description:        The python bindings to create and work on JUCE apps.
  website:            http://www.straw.com
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

    Enable or disable embedding the interpreter. This should be disabled when
*/
#ifndef JUCE_PYTHON_EMBEDDED_INTERPRETER
 #define JUCE_PYTHON_EMBEDDED_INTERPRETER 1
#endif

//==============================================================================
/** Config: JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION

    Enable or disable catching script exceptions.
*/
#ifndef JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION
 #define JUCE_PYTHON_SCRIPT_CATCH_EXCEPTION 0
#endif

#include "scripting/ScriptException.h"
#include "scripting/ScriptEngine.h"
#include "scripting/ScriptBindings.h"
#include "scripting/ScriptUtilities.h"
#include "bindings/ScriptJuceCoreBindings.h"
#include "bindings/ScriptJuceEventsBindings.h"
#include "bindings/ScriptJuceGraphicsBindings.h"
#include "bindings/ScriptJuceGuiBasicsBindings.h"
#include "utilities/ClassDemangling.h"
#include "utilities/PythonInterop.h"
