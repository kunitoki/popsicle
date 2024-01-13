/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

/*
 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 juce_python
  vendor:             kunitoki
  version:            1.0.2
  name:               Python bindings for the JUCE framework
  description:        The python bindings to create and work on JUCE apps.
  website:            http://www.straw.com
  license:            MIT
  minimumCppStandard: 17

  dependencies:       juce_core juce_events juce_graphics juce_gui_basics

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
