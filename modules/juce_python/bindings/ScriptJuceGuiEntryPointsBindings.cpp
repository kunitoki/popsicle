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

#include "ScriptJuceGuiEntryPointsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM
#include "../utilities/PyBind11Includes.h"

#if JUCE_WINDOWS
#include "../utilities/WindowsIncludes.h"
#endif

#include "ScriptJuceOptionsBindings.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

// =================================================================================================

namespace juce {

#if ! JUCE_WINDOWS
 extern const char* const* juce_argv;
 extern int juce_argc;
#endif

} // namespace juce

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

namespace {

// ============================================================================================

void runApplication (JUCEApplicationBase* application, int milliseconds)
{
    {
        py::gil_scoped_release release;

        if (! application->initialiseApp())
            return;
    }

    while (! MessageManager::getInstance()->hasStopMessageBeenSent())
    {
        if (globalOptions().catchExceptionsAndContinue)
        {
            try
            {
                py::gil_scoped_release release;

                MessageManager::getInstance()->runDispatchLoopUntil (milliseconds);
            }
            catch (const py::error_already_set& e)
            {
                Helpers::printPythonException (e);
            }
        }
        else
        {
            py::gil_scoped_release release;

            MessageManager::getInstance()->runDispatchLoopUntil (milliseconds);
        }

        if (globalOptions().caughtKeyboardInterrupt)
            break;

        if (PyErr_CheckSignals() != 0)
            throw py::error_already_set();
    }
}

} // namespace

void registerJuceGuiEntryPointsBindings (py::module_& m)
{
#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER

    // =================================================================================================

    m.def ("START_JUCE_APPLICATION", [](py::handle applicationType, bool catchExceptionsAndContinue)
    {
        globalOptions().catchExceptionsAndContinue = catchExceptionsAndContinue;
        globalOptions().caughtKeyboardInterrupt = false;

        py::scoped_ostream_redirect output;

        if (! applicationType)
            throw py::value_error("Argument must be a JUCEApplication subclass");

        JUCEApplicationBase* application = nullptr;

        auto sys = py::module_::import ("sys");
        auto systemExit = [sys, &application]
        {
            const int returnValue = application != nullptr ? application->shutdownApp() : 255;

            sys.attr ("exit") (returnValue);
        };

#if ! JUCE_WINDOWS
        StringArray arguments;
        for (auto arg : sys.attr ("argv"))
            arguments.add (arg.cast<String>());

        Array<const char*> argv;
        for (const auto& arg : arguments)
            argv.add (arg.toRawUTF8());

        juce_argv = argv.getRawDataPointer();
        juce_argc = argv.size();
#endif

        auto pyApplication = applicationType(); // TODO - error checking (python)

        application = pyApplication.cast<JUCEApplication*>();
        if (application == nullptr)
        {
            systemExit();
            return;
        }

        try
        {
            runApplication (application, globalOptions().messageManagerGranularityMilliseconds);
        }
        catch (const py::error_already_set& e)
        {
            Helpers::printPythonException (e);
        }

        systemExit();
    }, "applicationType"_a, "catchExceptionsAndContinue"_a = false);

#endif

    // =================================================================================================

    struct PyTestableApplication
    {
        struct Scope
        {
            Scope (py::handle applicationType)
            {
                if (! applicationType)
                    throw py::value_error("Argument must be a JUCEApplication subclass");

                JUCEApplicationBase* application = nullptr;

#if ! JUCE_WINDOWS
                for (auto arg : py::module_::import ("sys").attr ("argv"))
                    arguments.add (arg.cast<String>());

                for (const auto& arg : arguments)
                    argv.add (arg.toRawUTF8());

                juce_argv = argv.getRawDataPointer();
                juce_argc = argv.size();
#endif

                auto pyApplication = applicationType();

                application = pyApplication.cast<JUCEApplication*>();
                if (application == nullptr)
                    return;

                if (! application->initialiseApp())
                    return;
            }

            ~Scope()
            {
            }

        private:
#if ! JUCE_WINDOWS
            StringArray arguments;
            Array<const char*> argv;
#endif
        };

        PyTestableApplication (py::handle applicationType)
            : applicationType (applicationType)
        {
        }

        void processEvents(int milliseconds = 20)
        {
            try
            {
                JUCE_TRY
                {
                    py::gil_scoped_release release;

                    if (MessageManager::getInstance()->hasStopMessageBeenSent())
                        return;

                    MessageManager::getInstance()->runDispatchLoopUntil (milliseconds);
                }
                JUCE_CATCH_EXCEPTION

                bool isErrorSignalInFlight = PyErr_CheckSignals() != 0;
                if (isErrorSignalInFlight)
                    throw py::error_already_set();
            }
            catch (const py::error_already_set& e)
            {
                py::print (e.what());
            }
            catch (...)
            {
                py::print ("unhandled runtime error");
            }
        }

        py::handle applicationType;
        std::unique_ptr<Scope> applicationScope;
    };

    py::class_<PyTestableApplication> classTestableApplication (m, "TestApplication");

    classTestableApplication
        .def (py::init<py::handle>())
        .def ("processEvents", &PyTestableApplication::processEvents, "milliseconds"_a = 20)
        .def ("__enter__", [](PyTestableApplication& self)
        {
            self.applicationScope = std::make_unique<PyTestableApplication::Scope> (self.applicationType);
            return std::addressof (self);
        }, py::return_value_policy::reference)
        .def ("__exit__", [](PyTestableApplication& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.applicationScope.reset();
        })
        .def ("__next__", [](PyTestableApplication& self)
        {
            self.processEvents();
            return std::addressof (self);
        }, py::return_value_policy::reference)
    ;
}

} // namespace popsicle::Bindings

// =================================================================================================

#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER && JUCE_WINDOWS
BOOL APIENTRY DllMain(HANDLE instance, DWORD reason, LPVOID reserved)
{
    juce::ignoreUnused (reserved);

    if (reason == DLL_PROCESS_ATTACH)
        juce::Process::setCurrentModuleInstanceHandle (instance);

    return true;
}
#endif
