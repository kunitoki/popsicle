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

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

#include "ScriptJuceGuiBasicsBindings.h"
#include "../scripting/ScriptBindings.h"
#include "../utilities/ClassDemangling.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM
#include "../utilities/PyBind11Includes.h"

#if JUCE_WINDOWS
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#include <windows.h>
#undef VC_EXTRALEAN
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#undef NOGDI
#endif

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace PYBIND11_NAMESPACE {

template <>
struct polymorphic_type_hook<juce::Component>
{
    static const void* get (const juce::Component* src, const std::type_info*& type)
    {
        if (src == nullptr)
            return src;

        auto& map = popsicle::Bindings::getComponentTypeMap();
        auto demangledName = popsicle::Helpers::demangleClassName (typeid(*src).name());

        auto it = map.typeMap.find (demangledName);
        if (it != map.typeMap.end())
            return it->second (src, type);

        return src;
    }
};

} // namespace PYBIND11_NAMESPACE

//=================================================================================================

namespace juce {

#if ! JUCE_WINDOWS
 extern const char* const* juce_argv;
 extern int juce_argc;
#endif

} // namespace juce

//=================================================================================================

#if JUCE_WINDOWS
BOOL APIENTRY DllMain(HANDLE instance, DWORD reason, LPVOID reserved)
{
    juce::ignoreUnused (reserved);

    if (reason == DLL_PROCESS_ATTACH)
        juce::Process::setCurrentModuleInstanceHandle (instance);

    return true;
}
#endif

namespace popsicle::Bindings {

void registerJuceGuiBasicsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;
    using namespace py::literals;

#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER

    // ============================================================================================ START_JUCE_APPLICATION

    m.def ("START_JUCE_APPLICATION", [](py::handle applicationType)
    {
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
            if (! application->initialiseApp()) // TODO - error checking (python)
            {
                systemExit();
                return;
            }

            bool isErrorSignalInFlight = false;

            JUCE_TRY
            {
                do
                {
                    {
                        py::gil_scoped_release release;

                        if (MessageManager::getInstance()->hasStopMessageBeenSent())
                            break;

                        MessageManager::getInstance()->runDispatchLoopUntil (200);
                    }

                    isErrorSignalInFlight = PyErr_CheckSignals() != 0;
                    if (isErrorSignalInFlight)
                        break;
                }
                while (true);
            }
            JUCE_CATCH_EXCEPTION

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

        systemExit();
    });

#endif

    // ============================================================================================ PyTestableApplication

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
    ;

    // ============================================================================================ juce::JUCEApplication

    struct PyJUCEApplication : public JUCEApplication
    {
        const String getApplicationName() override
        {
            PYBIND11_OVERRIDE_PURE (const String, JUCEApplication, getApplicationName);
        }

        const String getApplicationVersion() override
        {
            PYBIND11_OVERRIDE_PURE (const String, JUCEApplication, getApplicationVersion);
        }

        bool moreThanOneInstanceAllowed() override
        {
            PYBIND11_OVERRIDE (bool, JUCEApplication, moreThanOneInstanceAllowed);
        }

        void initialise (const String& commandLineParameters) override
        {
            PYBIND11_OVERRIDE_PURE (void, JUCEApplication, initialise, commandLineParameters);
        }

        void shutdown() override
        {
            PYBIND11_OVERRIDE_PURE (void, JUCEApplication, shutdown);
        }

        void anotherInstanceStarted (const String& commandLine) override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, anotherInstanceStarted, commandLine);
        }

        void systemRequestedQuit() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, systemRequestedQuit);
        }

        void suspended() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, suspended);
        }

        void resumed() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, resumed);
        }

        void unhandledException (const std::exception* ex, const String& sourceFilename, int lineNumber) override
        {
            //PYBIND11_OVERRIDE (void, JUCEApplication, unhandledException, ex, sourceFilename, lineNumber);

            ignoreUnused (ex);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<JUCEApplication*> (this), "unhandledException"); override_)
                {
                    //String code;
                    //code << "RuntimeError(\"\"\"" << (ex != nullptr ? ex->what() : "unhandled exception") << "\"\"\")";
                    //auto pyex = py::eval (code.toRawUTF8());

                    //auto pyex = exceptionClass (ex != nullptr ? ex->what() : "unhandled exception");

                    auto pyex = py::none();

                    override_ (pyex, sourceFilename, lineNumber);

                    return;
                }
            }

            std::terminate();
        }

        void memoryWarningReceived() override
        {
            PYBIND11_OVERRIDE (void, JUCEApplication, memoryWarningReceived);
        }

        bool backButtonPressed() override
        {
            PYBIND11_OVERRIDE (bool, JUCEApplication, backButtonPressed);
        }
    };

    py::class_<JUCEApplication, PyJUCEApplication> (m, "JUCEApplication")
        .def (py::init<>())
        .def_static ("getInstance", &JUCEApplication::getInstance, py::return_value_policy::reference)
        .def ("getApplicationName", &JUCEApplication::getApplicationName)
        .def ("getApplicationVersion", &JUCEApplication::getApplicationVersion)
        .def ("moreThanOneInstanceAllowed", &JUCEApplication::moreThanOneInstanceAllowed)
        .def ("initialise", &JUCEApplication::initialise, "commandLineParameters"_a)
        .def ("shutdown", &JUCEApplication::shutdown)
        .def ("anotherInstanceStarted", &JUCEApplication::anotherInstanceStarted)
        .def ("systemRequestedQuit", &JUCEApplication::systemRequestedQuit)
        .def ("suspended", &JUCEApplication::suspended)
        .def ("resumed", &JUCEApplication::resumed)
        .def ("unhandledException", &JUCEApplication::unhandledException)
        .def ("memoryWarningReceived", &JUCEApplication::memoryWarningReceived)
        .def_static ("quit", &JUCEApplication::quit)
        .def_static ("getCommandLineParameterArray", &JUCEApplication::getCommandLineParameterArray)
        .def_static ("getCommandLineParameters", &JUCEApplication::getCommandLineParameters)
        .def ("setApplicationReturnValue", &JUCEApplication::setApplicationReturnValue)
        .def ("getApplicationReturnValue", &JUCEApplication::getApplicationReturnValue)
        .def_static ("isStandaloneApp", &JUCEApplication::isStandaloneApp)
        .def ("isInitialising", &JUCEApplication::isInitialising)
    ;

    // ============================================================================================ juce::MouseInputSource

    py::class_<ModifierKeys> classModifierKeys (m, "ModifierKeys");

    py::enum_<ModifierKeys::Flags> (classModifierKeys, "Flags", py::arithmetic())
        .value("noModifiers", ModifierKeys::Flags::noModifiers)
        .value("shiftModifier", ModifierKeys::Flags::shiftModifier)
        .value("ctrlModifier", ModifierKeys::Flags::ctrlModifier)
        .value("altModifier", ModifierKeys::Flags::altModifier)
        .value("leftButtonModifier", ModifierKeys::Flags::leftButtonModifier)
        .value("rightButtonModifier", ModifierKeys::Flags::rightButtonModifier)
        .value("middleButtonModifier", ModifierKeys::Flags::middleButtonModifier)
        .value("commandModifier", ModifierKeys::Flags::commandModifier)
        .value("popupMenuClickModifier", ModifierKeys::Flags::popupMenuClickModifier)
        .value("allKeyboardModifiers", ModifierKeys::Flags::allKeyboardModifiers)
        .value("allMouseButtonModifiers", ModifierKeys::Flags::allMouseButtonModifiers)
        .value("ctrlAltCommandModifiers", ModifierKeys::Flags::ctrlAltCommandModifiers)
        .export_values();

    classModifierKeys
        .def (py::init<>())
        .def (py::init<int>())
        .def ("isCommandDown", &ModifierKeys::isCommandDown)
        .def ("isPopupMenu", &ModifierKeys::isPopupMenu)
        .def ("isLeftButtonDown", &ModifierKeys::isLeftButtonDown)
        .def ("isRightButtonDown", &ModifierKeys::isRightButtonDown)
        .def ("isMiddleButtonDown", &ModifierKeys::isMiddleButtonDown)
        .def ("isAnyMouseButtonDown", &ModifierKeys::isAnyMouseButtonDown)
        .def ("isAnyModifierKeyDown", &ModifierKeys::isAnyModifierKeyDown)
        .def ("isShiftDown", &ModifierKeys::isShiftDown)
        .def ("isCtrlDown", &ModifierKeys::isCtrlDown)
        .def ("isAltDown", &ModifierKeys::isAltDown)
        .def ("withOnlyMouseButtons", &ModifierKeys::withOnlyMouseButtons)
        .def ("withoutMouseButtons", &ModifierKeys::withoutMouseButtons)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("getRawFlags", &ModifierKeys::getRawFlags)
        .def ("withoutFlags", &ModifierKeys::withoutFlags)
        .def ("withFlags", &ModifierKeys::withFlags)
        .def ("testFlags", &ModifierKeys::testFlags)
        .def ("getNumMouseButtonsDown", &ModifierKeys::getNumMouseButtonsDown)
        .def_readonly_static ("currentModifiers", &ModifierKeys::currentModifiers, py::return_value_policy::copy)
        .def_static ("getCurrentModifiers", &ModifierKeys::getCurrentModifiers)
        .def_static ("getCurrentModifiersRealtime", &ModifierKeys::getCurrentModifiersRealtime)
    ;

    // ============================================================================================ juce::MouseInputSource

    py::class_<MouseInputSource> classMouseInputSource (m, "MouseInputSource");

    py::enum_<MouseInputSource::InputSourceType> (classMouseInputSource, "InputSourceType")
        .value("mouse", MouseInputSource::InputSourceType::mouse)
        .value("touch", MouseInputSource::InputSourceType::touch)
        .value("pen", MouseInputSource::InputSourceType::pen)
        .export_values();

    classMouseInputSource
        .def ("getType", &MouseInputSource::getType)
        .def ("isMouse", &MouseInputSource::isMouse)
        .def ("isTouch", &MouseInputSource::isTouch)
        .def ("isPen", &MouseInputSource::isPen)
        .def ("canHover", &MouseInputSource::canHover)
        .def ("hasMouseWheel", &MouseInputSource::hasMouseWheel)
        .def ("getIndex", &MouseInputSource::getIndex)
        .def ("isDragging", &MouseInputSource::isDragging)
        .def ("getScreenPosition", &MouseInputSource::getScreenPosition)
        .def ("getRawScreenPosition", &MouseInputSource::getRawScreenPosition)
        .def ("getCurrentModifiers", &MouseInputSource::getCurrentModifiers)
        .def ("getCurrentPressure", &MouseInputSource::getCurrentPressure)
        .def ("getCurrentOrientation", &MouseInputSource::getCurrentOrientation)
        .def ("getCurrentRotation", &MouseInputSource::getCurrentRotation)
        .def ("getCurrentTilt", &MouseInputSource::getCurrentTilt)
        .def ("isPressureValid", &MouseInputSource::isPressureValid)
        .def ("isOrientationValid", &MouseInputSource::isOrientationValid)
        .def ("isRotationValid", &MouseInputSource::isRotationValid)
        .def ("isTiltValid", &MouseInputSource::isTiltValid)
        .def ("getComponentUnderMouse", &MouseInputSource::getComponentUnderMouse, py::return_value_policy::reference)
        .def ("triggerFakeMove", &MouseInputSource::triggerFakeMove)
        .def ("getNumberOfMultipleClicks", &MouseInputSource::getNumberOfMultipleClicks)
        .def ("getLastMouseDownTime", &MouseInputSource::getLastMouseDownTime)
        .def ("getLastMouseDownPosition", &MouseInputSource::getLastMouseDownPosition)
        .def ("isLongPressOrDrag", &MouseInputSource::isLongPressOrDrag)
        .def ("hasMovedSignificantlySincePressed", &MouseInputSource::hasMovedSignificantlySincePressed)
        .def ("hasMouseCursor", &MouseInputSource::hasMouseCursor)
        .def ("showMouseCursor", &MouseInputSource::showMouseCursor)
        .def ("hideCursor", &MouseInputSource::hideCursor)
        .def ("revealCursor", &MouseInputSource::revealCursor)
        .def ("forceMouseCursorUpdate", &MouseInputSource::forceMouseCursorUpdate)
        .def ("canDoUnboundedMovement", &MouseInputSource::canDoUnboundedMovement)
        .def ("enableUnboundedMouseMovement", &MouseInputSource::enableUnboundedMouseMovement)
        .def ("isUnboundedMouseMovementEnabled", &MouseInputSource::isUnboundedMouseMovementEnabled)
        .def ("setScreenPosition", &MouseInputSource::setScreenPosition)
        .def_readonly_static ("defaultPressure", &MouseInputSource::defaultPressure)
        .def_readonly_static ("defaultRotation", &MouseInputSource::defaultRotation)
        .def_readonly_static ("defaultTiltX", &MouseInputSource::defaultTiltX)
        .def_readonly_static ("defaultTiltY", &MouseInputSource::defaultTiltY)
        .def_readonly_static ("offscreenMousePos", &MouseInputSource::offscreenMousePos)
    ;

    // ============================================================================================ juce::MouseEvent

    py::class_<MouseEvent> classMouseEvent (m, "MouseEvent");
    classMouseEvent
        .def (py::init<MouseInputSource, Point<float>, ModifierKeys, float, float, float, float, float, Component*, Component*, Time, Point<float>, Time, int, bool>())
        .def_readonly ("position", &MouseEvent::position)
        .def_readonly ("x", &MouseEvent::x)
        .def_readonly ("y", &MouseEvent::y)
        .def_readonly ("mods", &MouseEvent::mods)
        .def_readonly ("pressure", &MouseEvent::pressure)
        .def_readonly ("orientation", &MouseEvent::orientation)
        .def_readonly ("rotation", &MouseEvent::rotation)
        .def_readonly ("tiltX", &MouseEvent::tiltX)
        .def_readonly ("tiltY", &MouseEvent::tiltY)
        .def_readonly ("mouseDownPosition", &MouseEvent::mouseDownPosition)
        .def_readonly ("eventComponent", &MouseEvent::eventComponent, py::return_value_policy::reference)
        .def_readonly ("originalComponent", &MouseEvent::originalComponent, py::return_value_policy::reference)
        .def_readonly ("eventTime", &MouseEvent::eventTime)
        .def_readonly ("mouseDownTime", &MouseEvent::mouseDownTime)
        .def_readonly ("source", &MouseEvent::source)
        .def ("getMouseDownX", &MouseEvent::getMouseDownX)
        .def ("getMouseDownY", &MouseEvent::getMouseDownY)
        .def ("getMouseDownPosition", &MouseEvent::getMouseDownPosition)
        .def ("getDistanceFromDragStart", &MouseEvent::getDistanceFromDragStart)
        .def ("getDistanceFromDragStartX", &MouseEvent::getDistanceFromDragStartX)
        .def ("getDistanceFromDragStartY", &MouseEvent::getDistanceFromDragStartY)
        .def ("getOffsetFromDragStart", &MouseEvent::getOffsetFromDragStart)
        .def ("mouseWasDraggedSinceMouseDown", &MouseEvent::mouseWasDraggedSinceMouseDown)
        .def ("mouseWasClicked", &MouseEvent::mouseWasClicked)
        .def ("getNumberOfClicks", &MouseEvent::getNumberOfClicks)
        .def ("getLengthOfMousePress", &MouseEvent::getLengthOfMousePress)
        .def ("isPressureValid", &MouseEvent::isPressureValid)
        .def ("isOrientationValid", &MouseEvent::isOrientationValid)
        .def ("isRotationValid", &MouseEvent::isRotationValid)
        .def ("isTiltValid", &MouseEvent::isTiltValid)
        .def ("getPosition", &MouseEvent::getPosition)
        .def ("getScreenX", &MouseEvent::getScreenX)
        .def ("getScreenY", &MouseEvent::getScreenY)
        .def ("getScreenPosition", &MouseEvent::getScreenPosition)
        .def ("getMouseDownScreenX", &MouseEvent::getMouseDownScreenX)
        .def ("getMouseDownScreenY", &MouseEvent::getMouseDownScreenY)
        .def ("getMouseDownScreenPosition", &MouseEvent::getMouseDownScreenPosition)
        .def ("getEventRelativeTo", &MouseEvent::getEventRelativeTo)
        .def ("withNewPosition", py::overload_cast<Point<float>> (&MouseEvent::withNewPosition, py::const_))
        .def ("withNewPosition", py::overload_cast<Point<int>> (&MouseEvent::withNewPosition, py::const_))
        .def_static ("setDoubleClickTimeout", &MouseEvent::setDoubleClickTimeout)
        .def_static ("getDoubleClickTimeout", &MouseEvent::getDoubleClickTimeout)
    ;

    py::class_<MouseWheelDetails> classMouseWheelDetails (m, "MouseWheelDetails");
    classMouseWheelDetails
        .def (py::init<>())
        .def_readwrite ("deltaX", &MouseWheelDetails::deltaX)
        .def_readwrite ("deltaY", &MouseWheelDetails::deltaY)
        .def_readwrite ("isReversed", &MouseWheelDetails::isReversed)
        .def_readwrite ("isSmooth", &MouseWheelDetails::isSmooth)
        .def_readwrite ("isInertial", &MouseWheelDetails::isInertial)
    ;

    py::class_<PenDetails> classPenDetails (m, "PenDetails");
    classPenDetails
        .def (py::init<>())
        .def_readwrite ("rotation", &PenDetails::rotation)
        .def_readwrite ("tiltX", &PenDetails::tiltX)
        .def_readwrite ("tiltY", &PenDetails::tiltY)
    ;

    // ============================================================================================ juce::MouseListener

    class PyMouseListener : public MouseListener
    {
        using MouseListener::MouseListener;

        void mouseMove (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseMove, event);
        }

        void mouseEnter (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseEnter, event);
        }

        void mouseExit (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseExit, event);
        }

        void mouseDown (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseDown, event);
        }

        void mouseDrag (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseDrag, event);
        }

        void mouseUp (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseUp, event);
        }

        void mouseDoubleClick (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseDoubleClick, event);
        }

        void mouseWheelMove (const MouseEvent& event, const MouseWheelDetails& wheel) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseWheelMove, event, wheel);
        }

        void mouseMagnify (const MouseEvent& event, float scaleFactor) override
        {
            PYBIND11_OVERRIDE (void, MouseListener, mouseMagnify, event, scaleFactor);
        }
    };

    py::class_<MouseListener, PyMouseListener> classMouseListener (m, "MouseListener");

    classMouseListener
        .def (py::init<>())
        .def ("mouseMove", &MouseListener::mouseMove)
        .def ("mouseEnter", &MouseListener::mouseEnter)
        .def ("mouseExit", &MouseListener::mouseExit)
        .def ("mouseDown", &MouseListener::mouseDown)
        .def ("mouseDrag", &MouseListener::mouseDrag)
        .def ("mouseUp", &MouseListener::mouseUp)
        .def ("mouseDoubleClick", &MouseListener::mouseDoubleClick)
        .def ("mouseWheelMove", &MouseListener::mouseWheelMove)
        .def ("mouseMagnify", &MouseListener::mouseMagnify)
    ;

    // ============================================================================================ juce::MouseCursor

    py::class_<MouseCursor> classMouseCursor (m, "MouseCursor");

    py::enum_<MouseCursor::StandardCursorType> (classMouseCursor, "StandardCursorType")
        .value("ParentCursor", MouseCursor::StandardCursorType::ParentCursor)
        .value("NoCursor", MouseCursor::StandardCursorType::NoCursor)
        .value("NormalCursor", MouseCursor::StandardCursorType::NormalCursor)
        .value("WaitCursor", MouseCursor::StandardCursorType::WaitCursor)
        .value("IBeamCursor", MouseCursor::StandardCursorType::IBeamCursor)
        .value("CrosshairCursor", MouseCursor::StandardCursorType::CrosshairCursor)
        .value("CopyingCursor", MouseCursor::StandardCursorType::CopyingCursor)
        .value("PointingHandCursor", MouseCursor::StandardCursorType::PointingHandCursor)
        .value("DraggingHandCursor", MouseCursor::StandardCursorType::DraggingHandCursor)
        .value("LeftRightResizeCursor", MouseCursor::StandardCursorType::LeftRightResizeCursor)
        .value("UpDownResizeCursor", MouseCursor::StandardCursorType::UpDownResizeCursor)
        .value("UpDownLeftRightResizeCursor", MouseCursor::StandardCursorType::UpDownLeftRightResizeCursor)
        .value("TopEdgeResizeCursor", MouseCursor::StandardCursorType::TopEdgeResizeCursor)
        .value("BottomEdgeResizeCursor", MouseCursor::StandardCursorType::BottomEdgeResizeCursor)
        .value("LeftEdgeResizeCursor", MouseCursor::StandardCursorType::LeftEdgeResizeCursor)
        .value("RightEdgeResizeCursor", MouseCursor::StandardCursorType::RightEdgeResizeCursor)
        .value("TopLeftCornerResizeCursor", MouseCursor::StandardCursorType::TopLeftCornerResizeCursor)
        .value("TopRightCornerResizeCursor", MouseCursor::StandardCursorType::TopRightCornerResizeCursor)
        .value("BottomLeftCornerResizeCursor", MouseCursor::StandardCursorType::BottomLeftCornerResizeCursor)
        .value("BottomRightCornerResizeCursor", MouseCursor::StandardCursorType::BottomRightCornerResizeCursor)
        .value("NumStandardCursorTypes", MouseCursor::StandardCursorType::NumStandardCursorTypes)
        .export_values();

    classMouseCursor
        .def (py::init<>())
        .def (py::init<MouseCursor::StandardCursorType>())
        .def (py::init<const Image&, int, int>())
        .def (py::init<const Image&, int, int, float>())
    //.def (py::init<const ScaledImage&, Point<int>>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self == MouseCursor::StandardCursorType())
        .def (py::self != MouseCursor::StandardCursorType())
        .def_static ("showWaitCursor", &MouseCursor::showWaitCursor)
        .def_static ("hideWaitCursor", &MouseCursor::hideWaitCursor)
    ;

    // ============================================================================================ juce::Desktop

    py::class_<Displays> classDisplays (m, "Displays");
    classDisplays
        .def ("physicalToLogical", static_cast<Rectangle<int> (Displays::*)(Rectangle<int>, const Displays::Display*) const> (&Displays::physicalToLogical))
        .def ("physicalToLogical", static_cast<Rectangle<float> (Displays::*)(Rectangle<float>, const Displays::Display*) const> (&Displays::physicalToLogical))
        .def ("physicalToLogical", py::overload_cast<Point<int>, const Displays::Display*> (&Displays::template physicalToLogical<int>, py::const_))
        .def ("physicalToLogical", py::overload_cast<Point<float>, const Displays::Display*> (&Displays::template physicalToLogical<float>, py::const_))
        .def ("logicalToPhysical", static_cast<Rectangle<int> (Displays::*)(Rectangle<int>, const Displays::Display*) const> (&Displays::logicalToPhysical))
        .def ("logicalToPhysical", static_cast<Rectangle<float> (Displays::*)(Rectangle<float>, const Displays::Display*) const> (&Displays::logicalToPhysical))
        .def ("logicalToPhysical", py::overload_cast<Point<int>, const Displays::Display*> (&Displays::template logicalToPhysical<int>, py::const_))
        .def ("logicalToPhysical", py::overload_cast<Point<float>, const Displays::Display*> (&Displays::template logicalToPhysical<float>, py::const_))
        .def ("getDisplayForRect", &Displays::getDisplayForRect, py::return_value_policy::reference)
        .def ("getDisplayForPoint", &Displays::getDisplayForPoint, py::return_value_policy::reference)
        .def ("getPrimaryDisplay", &Displays::getPrimaryDisplay, py::return_value_policy::reference)
        .def ("getRectangleList", &Displays::getRectangleList)
        .def ("getTotalBounds", &Displays::getTotalBounds)
        .def ("refresh", &Displays::refresh)
    ;

    py::class_<Displays::Display> classDisplay (classDisplays, "Display");
    classDisplay
        .def_readwrite ("isMain", &Displays::Display::isMain)
        .def_readwrite ("totalArea", &Displays::Display::totalArea)
        .def_readwrite ("userArea", &Displays::Display::userArea)
        .def_readwrite ("safeAreaInsets", &Displays::Display::safeAreaInsets)
        .def_readwrite ("keyboardInsets", &Displays::Display::keyboardInsets)
        .def_readwrite ("topLeftPhysical", &Displays::Display::topLeftPhysical)
        .def_readwrite ("scale", &Displays::Display::scale)
        .def_readwrite ("dpi", &Displays::Display::dpi)
        .def_readwrite ("verticalFrequencyHz", &Displays::Display::verticalFrequencyHz)
    ;

    // ============================================================================================ juce::LookAndFeel

    struct PyLookAndFeel : LookAndFeel
    {
        using LookAndFeel::LookAndFeel;

        //void drawSpinningWaitAnimation(Graphics& g, const Colour& colour, int x, int y, int w, int h) override
        //{
        //    PYBIND11_OVERRIDE_PURE(void, LookAndFeel, drawSpinningWaitAnimation, g, colour, x, y, w, h);
        //}

        //void getMouseCursorFor(Component& c) override
        //{
        //    PYBIND11_OVERRIDE(MouseCursor, LookAndFeel, getMouseCursorFor, c);
        //}

        void playAlertSound() override
        {
            PYBIND11_OVERRIDE(void, LookAndFeel, playAlertSound);
        }
    };

    py::class_<LookAndFeel, PyLookAndFeel> (m, "LookAndFeel")
    //.def (py::init<>())
        .def_static ("getDefaultLookAndFeel", &LookAndFeel::getDefaultLookAndFeel, py::return_value_policy::reference)
        .def_static ("setDefaultLookAndFeel", &LookAndFeel::setDefaultLookAndFeel)
        .def ("findColour", &LookAndFeel::findColour)
        .def ("setColour", &LookAndFeel::setColour)
        .def ("isColourSpecified", &LookAndFeel::isColourSpecified)
    //.def ("getTypefaceForFont", &LookAndFeel::getTypefaceForFont)
    //.def ("setDefaultSansSerifTypeface", &LookAndFeel::setDefaultSansSerifTypeface)
    //.def ("setDefaultSansSerifTypefaceName", &LookAndFeel::setDefaultSansSerifTypefaceName)
        .def ("setUsingNativeAlertWindows", &LookAndFeel::setUsingNativeAlertWindows)
        .def ("isUsingNativeAlertWindows", &LookAndFeel::isUsingNativeAlertWindows)
        .def ("drawSpinningWaitAnimation", &LookAndFeel::drawSpinningWaitAnimation)
        .def ("getTickShape", &LookAndFeel::getTickShape)
        .def ("getCrossShape", &LookAndFeel::getCrossShape)
    //.def ("createDropShadowerForComponent", &LookAndFeel::createDropShadowerForComponent)
    //.def ("createFocusOutlineForComponent", &LookAndFeel::createFocusOutlineForComponent)
        .def ("getMouseCursorFor", &LookAndFeel::getMouseCursorFor)
    //.def ("createGraphicsContext", &LookAndFeel::createGraphicsContext)
        .def ("playAlertSound", &LookAndFeel::playAlertSound)
    ;

    // ============================================================================================ juce::Desktop

    py::class_<Desktop, std::unique_ptr<Desktop, py::nodelete>> classDesktop (m, "Desktop");

    py::enum_<Desktop::DisplayOrientation> (classDesktop, "DisplayOrientation", py::arithmetic())
        .value("upright", Desktop::upright)
        .value("upsideDown", Desktop::upsideDown)
        .value("rotatedClockwise", Desktop::rotatedClockwise)
        .value("rotatedAntiClockwise", Desktop::rotatedAntiClockwise)
        .value("allOrientations", Desktop::allOrientations)
        .export_values();

    classDesktop
        .def_static ("getInstance", &Desktop::getInstance, py::return_value_policy::reference)
        .def_static ("getMousePosition", &Desktop::getMousePosition)
        .def_static ("setMousePosition", &Desktop::setMousePosition)
        .def_static ("getLastMouseDownPosition", &Desktop::getLastMouseDownPosition)
        .def ("getMouseButtonClickCounter", &Desktop::getMouseButtonClickCounter)
        .def ("getMouseWheelMoveCounter", &Desktop::getMouseWheelMoveCounter)
        .def_static ("setScreenSaverEnabled", &Desktop::setScreenSaverEnabled)
        .def_static ("isScreenSaverEnabled", &Desktop::isScreenSaverEnabled)
        .def ("addGlobalMouseListener", &Desktop::addGlobalMouseListener)
        .def ("removeGlobalMouseListener", &Desktop::removeGlobalMouseListener)
        .def ("addFocusChangeListener", &Desktop::addFocusChangeListener)
        .def ("removeFocusChangeListener", &Desktop::removeFocusChangeListener)
        .def ("addDarkModeSettingListener", &Desktop::addDarkModeSettingListener)
        .def ("removeDarkModeSettingListener", &Desktop::removeDarkModeSettingListener)
        .def ("isDarkModeActive", &Desktop::isDarkModeActive)
        .def ("setKioskModeComponent", &Desktop::setKioskModeComponent)
        .def ("getKioskModeComponent", &Desktop::getKioskModeComponent)
        .def ("getNumComponents", &Desktop::getNumComponents)
        .def ("getComponent", &Desktop::getComponent, py::return_value_policy::reference)
        .def ("findComponentAt", &Desktop::findComponentAt, py::return_value_policy::reference)
        .def ("getAnimator", &Desktop::getAnimator, py::return_value_policy::reference)
        .def ("getDefaultLookAndFeel", &Desktop::getDefaultLookAndFeel, py::return_value_policy::reference)
        .def ("setDefaultLookAndFeel", &Desktop::setDefaultLookAndFeel)
    //.def ("getMouseSources", &Desktop::getMouseSources)
        .def ("getNumMouseSources", &Desktop::getNumMouseSources)
        .def ("getMouseSource", &Desktop::getMouseSource, py::return_value_policy::reference)
        .def ("getMainMouseSource", &Desktop::getMainMouseSource)
        .def ("getNumDraggingMouseSources", &Desktop::getNumDraggingMouseSources)
        .def ("getDraggingMouseSource", &Desktop::getDraggingMouseSource, py::return_value_policy::reference)
        .def ("beginDragAutoRepeat", &Desktop::beginDragAutoRepeat)
        .def ("getCurrentOrientation", &Desktop::getCurrentOrientation)
        .def ("setOrientationsEnabled", &Desktop::setOrientationsEnabled)
        .def ("getOrientationsEnabled", &Desktop::getOrientationsEnabled)
        .def ("isOrientationEnabled", &Desktop::isOrientationEnabled)
        .def ("getDisplays", &Desktop::getDisplays)
        .def ("setGlobalScaleFactor", &Desktop::setGlobalScaleFactor)
        .def ("getGlobalScaleFactor", &Desktop::getGlobalScaleFactor)
        .def_static ("canUseSemiTransparentWindows", &Desktop::canUseSemiTransparentWindows)
        .def ("isHeadless", &Desktop::isHeadless)
    ;

    // ============================================================================================ juce::Desktop

    py::class_<ComponentAnimator> classComponentAnimator (m, "ComponentAnimator");
    classComponentAnimator
        .def (py::init<>())
        .def ("animateComponent", &ComponentAnimator::animateComponent)
        .def ("fadeOut", &ComponentAnimator::fadeOut)
        .def ("fadeIn", &ComponentAnimator::fadeIn)
        .def ("cancelAnimation", &ComponentAnimator::cancelAnimation)
        .def ("cancelAllAnimations", &ComponentAnimator::cancelAllAnimations)
        .def ("getComponentDestination", &ComponentAnimator::getComponentDestination)
        .def ("isAnimating", py::overload_cast<Component*> (&ComponentAnimator::isAnimating, py::const_))
        .def ("isAnimating", py::overload_cast<> (&ComponentAnimator::isAnimating, py::const_))
    ;

    // ============================================================================================ juce::Component

    class PyComponent : public Component
    {
        using Component::Component;

        void setName (const String& newName) override
        {
            PYBIND11_OVERRIDE (void, Component, setName, newName);
        }

        void setVisible (bool shouldBeVisible) override
        {
            PYBIND11_OVERRIDE (void, Component, setVisible, shouldBeVisible);
        }

        void visibilityChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, visibilityChanged);
        }

        void userTriedToCloseWindow() override
        {
            PYBIND11_OVERRIDE (void, Component, userTriedToCloseWindow);
        }

        void minimisationStateChanged(bool isNowMinimised) override
        {
            PYBIND11_OVERRIDE (void, Component, minimisationStateChanged, isNowMinimised);
        }

        float getDesktopScaleFactor() const override
        {
            PYBIND11_OVERRIDE (float, Component, getDesktopScaleFactor);
        }

        void parentHierarchyChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, parentHierarchyChanged);
        }

        void childrenChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, childrenChanged);
        }

        bool hitTest (int x, int y) override
        {
            PYBIND11_OVERRIDE (bool, Component, hitTest, x, y);
        }

        void lookAndFeelChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, lookAndFeelChanged);
        }

        void enablementChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, enablementChanged);
        }

        void alphaChanged() override
        {
            PYBIND11_OVERRIDE (void, Component, alphaChanged);
        }

        void paint (Graphics& g) override
        {
            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<const Component*> (this), "paint"); override_)
                {
                    override_ (std::addressof (g));
                    return;
                }
            }

            return Component::paint (g);
        }

        void paintOverChildren (Graphics& g) override
        {
            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<const Component*> (this), "paintOverChildren"); override_)
                {
                    override_ (std::addressof (g));
                    return;
                }
            }

            return Component::paintOverChildren (g);
        }

        void mouseMove (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseMove, event);
        }

        void mouseEnter (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseEnter, event);
        }

        void mouseExit (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseExit, event);
        }

        void mouseDown (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseDown, event);
        }

        void mouseDrag (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseDrag, event);
        }

        void mouseUp (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseUp, event);
        }

        void mouseDoubleClick (const MouseEvent& event) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseDoubleClick, event);
        }

        void mouseWheelMove (const MouseEvent& event, const MouseWheelDetails& wheel) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseWheelMove, event, wheel);
        }

        void mouseMagnify (const MouseEvent& event, float scaleFactor) override
        {
            PYBIND11_OVERRIDE (void, Component, mouseMagnify, event, scaleFactor);
        }

        bool keyPressed (const KeyPress& key) override
        {
            PYBIND11_OVERRIDE (bool, Component, keyPressed, key);
        }

        bool keyStateChanged (bool isKeyDown) override
        {
            PYBIND11_OVERRIDE (bool, Component, keyStateChanged, isKeyDown);
        }

        void modifierKeysChanged (const ModifierKeys& modifiers) override
        {
            PYBIND11_OVERRIDE (void, Component, modifierKeysChanged, modifiers);
        }

        void focusGained (Component::FocusChangeType cause) override
        {
            PYBIND11_OVERRIDE (void, Component, focusGained, cause);
        }

        void focusGainedWithDirection (Component::FocusChangeType cause, Component::FocusChangeDirection direction) override
        {
            PYBIND11_OVERRIDE (void, Component, focusGainedWithDirection, cause, direction);
        }

        void focusLost (Component::FocusChangeType cause) override
        {
            PYBIND11_OVERRIDE (void, Component, focusLost, cause);
        }

        void focusOfChildComponentChanged (Component::FocusChangeType cause) override
        {
            PYBIND11_OVERRIDE (void, Component, focusOfChildComponentChanged, cause);
        }

        void resized () override
        {
            PYBIND11_OVERRIDE (void, Component, resized);
        }

        void moved () override
        {
            PYBIND11_OVERRIDE (void, Component, moved);
        }

        void childBoundsChanged (Component* child) override
        {
            PYBIND11_OVERRIDE (void, Component, childBoundsChanged, child);
        }

        void parentSizeChanged () override
        {
            PYBIND11_OVERRIDE (void, Component, parentSizeChanged);
        }

        void broughtToFront () override
        {
            PYBIND11_OVERRIDE (void, Component, broughtToFront);
        }

        void handleCommandMessage (int commandId) override
        {
            PYBIND11_OVERRIDE (void, Component, handleCommandMessage, commandId);
        }

        bool canModalEventBeSentToComponent (const Component* targetComponent) override
        {
            PYBIND11_OVERRIDE (bool, Component, canModalEventBeSentToComponent, targetComponent);
        }

        void inputAttemptWhenModal () override
        {
            PYBIND11_OVERRIDE (void, Component, inputAttemptWhenModal);
        }

        void colourChanged () override
        {
            PYBIND11_OVERRIDE (void, Component, colourChanged);
        }
    };

    py::class_<Component, PyComponent> classComponent (m, "Component");

    py::enum_<Component::FocusChangeType> (classComponent, "FocusChangeType")
        .value ("focusChangedByMouseClick", Component::FocusChangeType::focusChangedByMouseClick)
        .value ("focusChangedByTabKey", Component::FocusChangeType::focusChangedByTabKey)
        .value ("focusChangedDirectly", Component::FocusChangeType::focusChangedDirectly)
        .export_values();

    py::enum_<Component::FocusChangeDirection> (classComponent, "FocusChangeDirection")
        .value ("unknown", Component::FocusChangeDirection::unknown)
        .value ("forward", Component::FocusChangeDirection::forward)
        .value ("backward", Component::FocusChangeDirection::backward);

    classComponent
        .def (py::init<>())
        .def (py::init<const String&>())
        .def ("setName", &Component::setName)
        .def ("getName", &Component::getName)
        .def ("getComponentID", &Component::getComponentID)
        .def ("setVisible", &Component::setVisible)
        .def ("isVisible", &Component::isVisible)
        .def ("visibilityChanged", &Component::visibilityChanged)
        .def ("isShowing", &Component::isShowing)
    //.def ("addToDesktop", &Component::addToDesktop)
        .def ("removeFromDesktop", &Component::removeFromDesktop)
        .def ("isOnDesktop", &Component::isOnDesktop)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)
    //.def ("getPeer", &Component::getPeer)
        .def ("userTriedToCloseWindow", &Component::userTriedToCloseWindow)
        .def ("minimisationStateChanged", &Component::minimisationStateChanged)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)
        .def ("toFront", &Component::toFront)
        .def ("toBack", &Component::toBack)
        .def ("toBehind", &Component::toBehind)
        .def ("setAlwaysOnTop", &Component::setAlwaysOnTop)
        .def ("isAlwaysOnTop", &Component::isAlwaysOnTop)
        .def ("getX", &Component::getX)
        .def ("getY", &Component::getY)
        .def ("getWidth", &Component::getWidth)
        .def ("getHeight", &Component::getHeight)
        .def ("getRight", &Component::getRight)
        .def ("getBottom", &Component::getBottom)
        .def ("getPosition", &Component::getPosition)
        .def ("getBounds", &Component::getBounds)
        .def ("getLocalBounds", &Component::getLocalBounds)
        .def ("getBoundsInParent", &Component::getBoundsInParent)
        .def ("getScreenX", &Component::getScreenX)
        .def ("getScreenY", &Component::getScreenY)
        .def ("getScreenPosition", &Component::getScreenPosition)
        .def ("getScreenBounds", &Component::getScreenBounds)
        .def ("setTopLeftPosition", py::overload_cast<int, int> (&Component::setTopLeftPosition))
        .def ("setTopLeftPosition", py::overload_cast<Point<int>> (&Component::setTopLeftPosition))
        .def ("setTopRightPosition", &Component::setTopRightPosition)
        .def ("setSize", &Component::setSize)
        .def ("setBounds", py::overload_cast<int, int, int, int> (&Component::setBounds))
        .def ("setBounds", py::overload_cast<Rectangle<int>> (&Component::setBounds))
        .def ("setBoundsRelative", py::overload_cast<float, float, float, float> (&Component::setBoundsRelative))
        .def ("setBoundsRelative", py::overload_cast<Rectangle<float>> (&Component::setBoundsRelative))
        .def ("setBoundsInset", &Component::setBoundsInset)
        .def ("setBoundsToFit", &Component::setBoundsToFit)
        .def ("setCentrePosition", py::overload_cast<int, int> (&Component::setCentrePosition))
        .def ("setCentrePosition", py::overload_cast<Point<int>> (&Component::setCentrePosition))
        .def ("setCentreRelative", &Component::setCentreRelative)
        .def ("centreWithSize", &Component::centreWithSize)
        .def ("setTransform", &Component::setTransform)
        .def ("getTransform", &Component::getTransform)
        .def ("isTransformed", &Component::isTransformed)
        .def_static ("getApproximateScaleFactorForComponent", &Component::getApproximateScaleFactorForComponent)
        .def ("proportionOfWidth", &Component::proportionOfWidth)
        .def ("proportionOfHeight", &Component::proportionOfHeight)
        .def ("getParentWidth", &Component::getParentWidth)
        .def ("getParentHeight", &Component::getParentHeight)
        .def ("getParentMonitorArea", &Component::getParentMonitorArea)
        .def ("getNumChildComponents", &Component::getNumChildComponents)
        .def ("getChildComponent", &Component::getChildComponent, py::return_value_policy::reference)
        .def ("getIndexOfChildComponent", &Component::getIndexOfChildComponent)
        .def ("findChildWithID", &Component::findChildWithID, py::return_value_policy::reference)
        .def ("addChildComponent", [](Component* self, Component* toAdd) { self->addChildComponent (toAdd); })
        .def ("addChildComponent", py::overload_cast<Component*, int> (&Component::addChildComponent))
        .def ("addAndMakeVisible", [](Component* self, Component* toAdd) { self->addAndMakeVisible (toAdd); })
        .def ("addAndMakeVisible", py::overload_cast<Component*, int> (&Component::addAndMakeVisible))
        .def ("addChildAndSetID", &Component::addChildAndSetID)
        .def ("removeChildComponent", py::overload_cast<Component*> (&Component::removeChildComponent))
        .def ("removeChildComponent", py::overload_cast<int> (&Component::removeChildComponent), py::return_value_policy::reference)
        .def ("removeAllChildren", &Component::removeAllChildren)
        .def ("deleteAllChildren", &Component::deleteAllChildren)
        .def ("getParentComponent", &Component::getParentComponent, py::return_value_policy::reference)
        .def ("getTopLevelComponent", &Component::getTopLevelComponent, py::return_value_policy::reference)
        .def ("isParentOf", &Component::isParentOf)
        .def ("parentHierarchyChanged", &Component::parentHierarchyChanged)
        .def ("childrenChanged", &Component::childrenChanged)
        .def ("hitTest", &Component::hitTest)
        .def ("setInterceptsMouseClicks", &Component::setInterceptsMouseClicks)
        .def ("getInterceptsMouseClicks", [](const Component& self) { bool a, b; self.getInterceptsMouseClicks(a, b); return py::make_tuple(a, b); })
        .def ("contains", py::overload_cast<Point<float>> (&Component::contains))
        .def ("reallyContains", py::overload_cast<Point<float>, bool> (&Component::reallyContains))
        .def ("getComponentAt", py::overload_cast<int, int> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("getComponentAt", py::overload_cast<Point<int>> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("getComponentAt", py::overload_cast<Point<float>> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("repaint", py::overload_cast<> (&Component::repaint))
        .def ("repaint", py::overload_cast<int, int, int, int> (&Component::repaint))
        .def ("repaint", py::overload_cast<Rectangle<int>> (&Component::repaint))
        .def ("setBufferedToImage", &Component::setBufferedToImage)
        .def ("createComponentSnapshot", &Component::createComponentSnapshot)
        .def ("paintEntireComponent", &Component::paintEntireComponent)
        .def ("setPaintingIsUnclipped", &Component::setPaintingIsUnclipped)
        .def ("isPaintingUnclipped", &Component::isPaintingUnclipped)
    //.def ("setComponentEffect", &Component::setComponentEffect)
    //.def ("getComponentEffect", &Component::getComponentEffect)
    //.def ("getLookAndFeel", &Component::getLookAndFeel)
    //.def ("setLookAndFeel", &Component::setLookAndFeel)
        .def ("lookAndFeelChanged", &Component::lookAndFeelChanged)
        .def ("sendLookAndFeelChange", &Component::sendLookAndFeelChange)
        .def ("setOpaque", &Component::setOpaque)
        .def ("isOpaque", &Component::isOpaque)
        .def ("setBroughtToFrontOnMouseClick", &Component::setBroughtToFrontOnMouseClick)
        .def ("isBroughtToFrontOnMouseClick", &Component::isBroughtToFrontOnMouseClick)
        .def ("setExplicitFocusOrder", &Component::setExplicitFocusOrder)
        .def ("getExplicitFocusOrder", &Component::getExplicitFocusOrder)
    //.def ("setFocusContainerType", &Component::setFocusContainerType)
        .def ("isFocusContainer", &Component::isFocusContainer)
        .def ("isKeyboardFocusContainer", &Component::isKeyboardFocusContainer)
        .def ("findFocusContainer", &Component::findFocusContainer, py::return_value_policy::reference)
        .def ("findKeyboardFocusContainer", &Component::findKeyboardFocusContainer, py::return_value_policy::reference)
        .def ("setWantsKeyboardFocus", &Component::setWantsKeyboardFocus)
        .def ("getWantsKeyboardFocus", &Component::getWantsKeyboardFocus)
        .def ("setMouseClickGrabsKeyboardFocus", &Component::setMouseClickGrabsKeyboardFocus)
        .def ("getMouseClickGrabsKeyboardFocus", &Component::getMouseClickGrabsKeyboardFocus)
        .def ("grabKeyboardFocus", &Component::grabKeyboardFocus)
        .def ("giveAwayKeyboardFocus", &Component::giveAwayKeyboardFocus)
        .def ("hasKeyboardFocus", &Component::hasKeyboardFocus)
        .def ("moveKeyboardFocusToSibling", &Component::moveKeyboardFocusToSibling)
        .def_static ("getCurrentlyFocusedComponent", &Component::getCurrentlyFocusedComponent, py::return_value_policy::reference)
        .def_static ("unfocusAllComponents", &Component::unfocusAllComponents)
    //.def ("createFocusTraverser", &Component::createFocusTraverser)
    //.def ("createKeyboardFocusTraverser", &Component::createKeyboardFocusTraverser)
        .def ("setHasFocusOutline", &Component::setHasFocusOutline)
        .def ("hasFocusOutline", &Component::hasFocusOutline)
        .def ("isEnabled", &Component::isEnabled)
        .def ("setEnabled", &Component::setEnabled)
        .def ("enablementChanged", &Component::enablementChanged)
        .def ("getAlpha", &Component::getAlpha)
        .def ("setAlpha", &Component::setAlpha)
        .def ("alphaChanged", &Component::alphaChanged)
        .def ("setMouseCursor", &Component::setMouseCursor)
        .def ("getMouseCursor", &Component::getMouseCursor)
        .def ("updateMouseCursor", &Component::updateMouseCursor)
        .def ("paint", &Component::paint)
        .def ("paintOverChildren", &Component::paintOverChildren)
        .def ("mouseMove", &Component::mouseMove)
        .def ("mouseEnter", &Component::mouseEnter)
        .def ("mouseExit", &Component::mouseExit)
        .def ("mouseDown", &Component::mouseDown)
        .def ("mouseDrag", &Component::mouseDrag)
        .def ("mouseUp", &Component::mouseUp)
        .def ("mouseDoubleClick", &Component::mouseDoubleClick)
        .def ("mouseWheelMove", &Component::mouseWheelMove)
        .def ("mouseMagnify", &Component::mouseMagnify)
        .def_static ("beginDragAutoRepeat", &Component::beginDragAutoRepeat)
        .def ("setRepaintsOnMouseActivity", &Component::setRepaintsOnMouseActivity)
        .def ("addMouseListener", &Component::addMouseListener)
        .def ("removeMouseListener", &Component::removeMouseListener)
    //.def ("addKeyListener", &Component::addKeyListener)
    //.def ("removeKeyListener", &Component::removeKeyListener)
    //.def ("keyPressed", &Component::keyPressed)
    //.def ("keyStateChanged", &Component::keyStateChanged)
    //.def ("modifierKeysChanged", &Component::modifierKeysChanged)
        .def ("isMouseOver", &Component::isMouseOver)
        .def ("isMouseButtonDown", &Component::isMouseButtonDown)
        .def ("isMouseOverOrDragging", &Component::isMouseOverOrDragging)
        .def_static ("isMouseButtonDownAnywhere", &Component::isMouseButtonDownAnywhere)
        .def ("getMouseXYRelative", &Component::getMouseXYRelative)
        .def ("resized", &Component::resized)
        .def ("moved", &Component::moved)
        .def ("childBoundsChanged", &Component::childBoundsChanged)
        .def ("parentSizeChanged", &Component::parentSizeChanged)
        .def ("broughtToFront", &Component::broughtToFront)
    //.def ("addComponentListener", &Component::addComponentListener)
    //.def ("removeComponentListener", &Component::removeComponentListener)
        .def ("postCommandMessage", &Component::postCommandMessage)
        .def ("handleCommandMessage", &Component::handleCommandMessage)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runModalLoop", &Component::runModalLoop)
#endif
    //.def ("enterModalState", &Component::enterModalState)
    //.def ("exitModalState", &Component::exitModalState)
        .def ("isCurrentlyModal", &Component::isCurrentlyModal)
        .def_static ("getNumCurrentlyModalComponents", &Component::getNumCurrentlyModalComponents)
        .def_static ("getCurrentlyModalComponent", &Component::getCurrentlyModalComponent)
        .def ("isCurrentlyBlockedByAnotherModalComponent", &Component::isCurrentlyBlockedByAnotherModalComponent)
        .def ("canModalEventBeSentToComponent", &Component::canModalEventBeSentToComponent)
        .def ("inputAttemptWhenModal", &Component::inputAttemptWhenModal)
        .def ("getProperties", py::overload_cast<> (&Component::getProperties))
        .def ("getProperties", py::overload_cast<> (&Component::getProperties, py::const_))
        .def ("findColour", &Component::findColour)
        .def ("setColour", &Component::setColour)
        .def ("removeColour", &Component::removeColour)
        .def ("isColourSpecified", &Component::isColourSpecified)
        .def ("copyAllExplicitColoursTo", [](const Component& self, Component* target) { self.copyAllExplicitColoursTo (*target); })
        .def ("colourChanged", &Component::colourChanged)
    //.def ("getWindowHandle", &Component::getWindowHandle)
    //.def ("getPositioner", &Component::getPositioner)
    //.def ("setPositioner", &Component::setPositioner)
    //.def ("setCachedComponentImage", &Component::setCachedComponentImage)
    //.def ("getCachedComponentImage", &Component::getCachedComponentImage)
        .def ("setViewportIgnoreDragFlag", &Component::setViewportIgnoreDragFlag)
        .def ("getViewportIgnoreDragFlag", &Component::getViewportIgnoreDragFlag)
        .def ("getTitle", &Component::getTitle)
        .def ("setTitle", &Component::setTitle)
        .def ("getDescription", &Component::getDescription)
        .def ("setDescription", &Component::setDescription)
        .def ("getHelpText", &Component::getHelpText)
        .def ("setHelpText", &Component::setHelpText)
        .def ("setAccessible", &Component::setAccessible)
        .def ("isAccessible", &Component::isAccessible)
    //.def ("getAccessibilityHandler", &Component::getAccessibilityHandler)
        .def ("invalidateAccessibilityHandler", &Component::invalidateAccessibilityHandler)
    //.def ("createAccessibilityHandler", &Component::createAccessibilityHandler)
        .def ("getChildren", [](const juce::Component& self)
        {
            py::list list;
            for (const auto& comp : self.getChildren())
                list.append (comp);
            return list;
        })
        .def ("typeName", [](const juce::Component* self)
        {
            return Helpers::pythonizeClassName (typeid(*self).name());
        })
    ;

    // ============================================================================================ juce::Button

    struct PyButton : Button
    {
        using Button::Button;

        void triggerClick() override
        {
            PYBIND11_OVERRIDE(void, Button, triggerClick);
        }

        void clicked() override
        {
            PYBIND11_OVERRIDE(void, Button, clicked);
        }

        void clicked (const ModifierKeys& modifiers) override
        {
            PYBIND11_OVERRIDE(void, Button, clicked, modifiers);
        }

        void paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override (static_cast<Button*> (this), "paintButton"); override_)
                {
                    override_ (std::addressof (g), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                    return;
                }
            }

            py::pybind11_fail("Tried to call pure virtual function \"Button::paintButton\"");
        }

        void buttonStateChanged() override
        {
            PYBIND11_OVERRIDE(void, Button, buttonStateChanged);
        }
    };

    struct PyButtonListener : Button::Listener
    {
        using Button::Listener::Listener;

        void buttonClicked (Button* button) override
        {
            PYBIND11_OVERRIDE_PURE(void, Button::Listener, buttonClicked, button);
        }

        void buttonStateChanged (Button* button) override
        {
            PYBIND11_OVERRIDE(void, Button::Listener, buttonStateChanged, button);
        }
    };

    py::class_<Button, Component, PyButton> classButton (m, "Button");

    py::enum_<Button::ConnectedEdgeFlags> (classButton, "ConnectedEdgeFlags", py::arithmetic())
        .value ("ConnectedOnLeft", Button::ConnectedEdgeFlags::ConnectedOnLeft)
        .value ("ConnectedOnRight", Button::ConnectedEdgeFlags::ConnectedOnRight)
        .value ("ConnectedOnTop", Button::ConnectedEdgeFlags::ConnectedOnTop)
        .value ("ConnectedOnBottom", Button::ConnectedEdgeFlags::ConnectedOnBottom)
        .export_values();

    py::enum_<Button::ButtonState> (classButton, "ButtonState")
        .value ("buttonNormal", Button::ButtonState::buttonNormal)
        .value ("buttonOver", Button::ButtonState::buttonOver)
        .value ("buttonDown", Button::ButtonState::buttonDown)
        .export_values();

    py::class_<Button::Listener, PyButtonListener> classButtonListener (classButton, "Listener");

    classButtonListener
        .def ("buttonClicked", &Button::Listener::buttonClicked)
        .def ("buttonStateChanged", &Button::Listener::buttonStateChanged)
    ;

    classButton
        .def ("setButtonText", &Button::setButtonText)
        .def ("getButtonText", &Button::getButtonText)
        .def ("isDown", &Button::isDown)
        .def ("isOver", &Button::isOver)
        .def ("setToggleable", &Button::setToggleable)
        .def ("isToggleable", &Button::isToggleable)
        .def ("setToggleState", py::overload_cast<bool, NotificationType> (&Button::setToggleState))
        .def ("getToggleState", &Button::getToggleState)
        .def ("getToggleStateValue", &Button::getToggleStateValue, py::return_value_policy::reference)
        .def ("setClickingTogglesState", &Button::setClickingTogglesState)
        .def ("getClickingTogglesState", &Button::getClickingTogglesState)
        .def ("setRadioGroupId", &Button::setRadioGroupId)
        .def ("getRadioGroupId", &Button::getRadioGroupId)
        .def ("addListener", &Button::addListener)
        .def ("removeListener", &Button::removeListener)
        .def_readwrite ("onClick", &Button::onClick)
        .def_readwrite ("onStateChange", &Button::onStateChange)
        .def ("triggerClick", &Button::triggerClick)
    //.def ("setCommandToTrigger", &Button::setCommandToTrigger)
        .def ("getCommandID", &Button::getCommandID)
        .def ("addShortcut", &Button::addShortcut)
        .def ("clearShortcuts", &Button::clearShortcuts)
        .def ("isRegisteredForShortcut", &Button::isRegisteredForShortcut)
        .def ("setRepeatSpeed", &Button::setRepeatSpeed)
        .def ("setTriggeredOnMouseDown", &Button::setTriggeredOnMouseDown)
        .def ("getTriggeredOnMouseDown", &Button::getTriggeredOnMouseDown)
        .def ("getMillisecondsSinceButtonDown", &Button::getMillisecondsSinceButtonDown)
        .def ("setConnectedEdges", &Button::setConnectedEdges)
        .def ("getConnectedEdgeFlags", &Button::getConnectedEdgeFlags)
        .def ("isConnectedOnLeft", &Button::isConnectedOnLeft)
        .def ("isConnectedOnRight", &Button::isConnectedOnRight)
        .def ("isConnectedOnTop", &Button::isConnectedOnTop)
        .def ("isConnectedOnBottom", &Button::isConnectedOnBottom)
        .def ("setState", &Button::setState)
        .def ("getState", &Button::getState)
    ;

    // ============================================================================================ juce::ArrowButton

    py::class_<ArrowButton, Button> classArrowButton (m, "ArrowButton");

    classArrowButton
        .def (py::init<const String&, float, Colour>())
    ;

    // ============================================================================================ juce::DrawableButton

    struct PyDrawableButton : DrawableButton
    {
        using DrawableButton::DrawableButton;

        Rectangle<float> getImageBounds() const override
        {
            PYBIND11_OVERRIDE(Rectangle<float>, DrawableButton, getImageBounds);
        }
    };

    py::class_<DrawableButton, Button, PyDrawableButton> classDrawableButton (m, "DrawableButton");

    py::enum_<DrawableButton::ButtonStyle> (classDrawableButton, "ButtonStyle")
        .value ("ImageFitted", DrawableButton::ButtonStyle::ImageFitted)
        .value ("ImageRaw", DrawableButton::ButtonStyle::ImageRaw)
        .value ("ImageAboveTextLabel", DrawableButton::ButtonStyle::ImageAboveTextLabel)
        .value ("ImageOnButtonBackground", DrawableButton::ButtonStyle::ImageOnButtonBackground)
        .value ("ImageOnButtonBackgroundOriginalSize", DrawableButton::ButtonStyle::ImageOnButtonBackgroundOriginalSize)
        .value ("ImageStretched", DrawableButton::ButtonStyle::ImageStretched)
        .export_values();

    classDrawableButton
        .def (py::init<const String&, DrawableButton::ButtonStyle>())
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage) { self->setImages (normalImage); })
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage, const Drawable* downImage) { self->setImages (normalImage, downImage); })
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage, const Drawable* downImage, const Drawable* disabledImage) { self->setImages (normalImage, downImage, disabledImage); })
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage, const Drawable* downImage, const Drawable* disabledImage, const Drawable* normalImageOn) { self->setImages (normalImage, downImage, disabledImage, normalImageOn); })
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage, const Drawable* downImage, const Drawable* disabledImage, const Drawable* normalImageOn, const Drawable* overImageOn) { self->setImages (normalImage, downImage, disabledImage, normalImageOn, overImageOn); })
        .def ("setImages", [](DrawableButton* self, const Drawable* normalImage, const Drawable* downImage, const Drawable* disabledImage, const Drawable* normalImageOn, const Drawable* overImageOn, const Drawable* downImageOn) { self->setImages (normalImage, downImage, disabledImage, normalImageOn, overImageOn, downImageOn); })
        .def ("setImages", &DrawableButton::setImages)
        .def ("setButtonStyle", &DrawableButton::setButtonStyle)
        .def ("getStyle", &DrawableButton::getStyle)
        .def ("setEdgeIndent", &DrawableButton::setEdgeIndent)
        .def ("getEdgeIndent", &DrawableButton::getEdgeIndent)
        .def ("getCurrentImage", &DrawableButton::getCurrentImage, py::return_value_policy::reference)
        .def ("getNormalImage", &DrawableButton::getNormalImage, py::return_value_policy::reference)
        .def ("getOverImage", &DrawableButton::getOverImage, py::return_value_policy::reference)
        .def ("getDownImage", &DrawableButton::getDownImage, py::return_value_policy::reference)
        .def ("getImageBounds", &DrawableButton::getImageBounds)
    ;

    classDrawableButton.attr ("textColourId") = py::int_(static_cast<int> (DrawableButton::textColourId));
    classDrawableButton.attr ("textColourOnId") = py::int_(static_cast<int> (DrawableButton::textColourOnId));
    classDrawableButton.attr ("backgroundColourId") = py::int_(static_cast<int> (DrawableButton::backgroundColourId));
    classDrawableButton.attr ("backgroundOnColourId") = py::int_(static_cast<int> (DrawableButton::backgroundOnColourId));

    // ============================================================================================ juce::HyperlinkButton

    py::class_<HyperlinkButton, Button> classHyperlinkButton (m, "HyperlinkButton");

    classHyperlinkButton
        .def (py::init<>())
        .def (py::init<const String&, const URL&>())
        .def ("setFont", &HyperlinkButton::setFont)
        .def ("setURL", &HyperlinkButton::setURL)
        .def ("getURL", &HyperlinkButton::getURL)
        .def ("changeWidthToFitText", &HyperlinkButton::changeWidthToFitText)
        .def ("setJustificationType", &HyperlinkButton::setJustificationType)
        .def ("getJustificationType", &HyperlinkButton::getJustificationType)
    //.def ("createAccessibilityHandler", &ToggleButton::createAccessibilityHandler)
    ;

    classHyperlinkButton.attr ("textColourId") = py::int_(static_cast<int> (HyperlinkButton::textColourId));

    // ============================================================================================ juce::ImageButton

    py::class_<ImageButton, Button> classImageButton (m, "ImageButton");

    classImageButton
        .def (py::init<const String&>(), "name"_a = String())
        .def ("setImages", &ImageButton::setImages)
        .def ("getNormalImage", &ImageButton::getNormalImage)
        .def ("getOverImage", &ImageButton::getOverImage)
        .def ("getDownImage", &ImageButton::getDownImage)
    ;

    // ============================================================================================ juce::ShapeButton

    py::class_<ShapeButton, Button> classShapeButton (m, "ShapeButton");

    classShapeButton
        .def (py::init<const String&, Colour, Colour, Colour>())
        .def ("setShape", &ShapeButton::setShape)
        .def ("setColours", &ShapeButton::setColours)
        .def ("setOnColours", &ShapeButton::setOnColours)
        .def ("shouldUseOnColours", &ShapeButton::shouldUseOnColours)
        .def ("setOutline", &ShapeButton::setOutline)
        .def ("setBorderSize", &ShapeButton::setBorderSize)
    ;

    // ============================================================================================ juce::TextButton

    py::class_<TextButton, Button> classTextButton (m, "TextButton");

    classTextButton
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const String&, const String&>())
        .def ("changeWidthToFitText", py::overload_cast<> (&TextButton::changeWidthToFitText))
        .def ("changeWidthToFitText", py::overload_cast<int> (&TextButton::changeWidthToFitText))
        .def ("getBestWidthForHeight", &TextButton::getBestWidthForHeight)
    ;

    classTextButton.attr ("buttonColourId") = py::int_(static_cast<int> (TextButton::buttonColourId));
    classTextButton.attr ("buttonOnColourId") = py::int_(static_cast<int> (TextButton::buttonOnColourId));
    classTextButton.attr ("textColourOffId") = py::int_(static_cast<int> (TextButton::textColourOffId));
    classTextButton.attr ("textColourOnId") = py::int_(static_cast<int> (TextButton::textColourOnId));

    // ============================================================================================ juce::ToggleButton

    py::class_<ToggleButton, Button> classToggleButton (m, "ToggleButton");

    classToggleButton
        .def (py::init<>())
        .def (py::init<const String&>())
        .def ("changeWidthToFitText", &ToggleButton::changeWidthToFitText)
    //.def ("createAccessibilityHandler", &ToggleButton::createAccessibilityHandler)
    ;

    classToggleButton.attr ("textColourId") = py::int_(static_cast<int> (ToggleButton::textColourId));
    classToggleButton.attr ("tickColourId") = py::int_(static_cast<int> (ToggleButton::tickColourId));
    classToggleButton.attr ("tickDisabledColourId") = py::int_(static_cast<int> (ToggleButton::tickDisabledColourId));

    // ============================================================================================ juce::ToolbarItemFactory

    struct PyToolbarItemFactory : ToolbarItemFactory
    {
        using ToolbarItemFactory::ToolbarItemFactory;

        void getAllToolbarItemIds (Array<int>& ids) override
        {
            //PYBIND11_OVERRIDE(void, ToolbarItemFactory, getAllToolbarItemIds, ids);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override(static_cast<ToolbarItemFactory*>(this), "getAllToolbarItemIds"); override_)
                {
                    auto result = override_ ();

                    ids.addArray (result.cast<Array<int>>());
                }
            }

            py::pybind11_fail("Tried to call pure virtual function \"ToolbarItemFactory::getAllToolbarItemIds\"");
        }

        void getDefaultItemSet (Array <int>& ids) override
        {
            //PYBIND11_OVERRIDE_PURE(void, ToolbarItemFactory, getDefaultItemSet, ids);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override(static_cast<ToolbarItemFactory*>(this), "getDefaultItemSet"); override_)
                {
                    auto result = override_ ();

                    ids.addArray (result.cast<Array<int>>());
                }
            }

            py::pybind11_fail("Tried to call pure virtual function \"ToolbarItemFactory::getDefaultItemSet\"");
        }

        ToolbarItemComponent* createItem (int itemId) override
        {
            PYBIND11_OVERRIDE_PURE(ToolbarItemComponent*, ToolbarItemFactory, createItem, itemId);
        }
    };

    py::class_<ToolbarItemFactory, PyToolbarItemFactory> classToolbarItemFactory (m, "ToolbarItemFactory");

    py::enum_<ToolbarItemFactory::SpecialItemIds> (classToolbarItemFactory, "SpecialItemIds")
        .value ("separatorBarId", ToolbarItemFactory::SpecialItemIds::separatorBarId)
        .value ("spacerId", ToolbarItemFactory::SpecialItemIds::spacerId)
        .value ("flexibleSpacerId", ToolbarItemFactory::SpecialItemIds::flexibleSpacerId)
        .export_values();

    classToolbarItemFactory
        .def (py::init<>())
        .def ("getAllToolbarItemIds", &ToolbarItemFactory::getAllToolbarItemIds)
        .def ("getDefaultItemSet", &ToolbarItemFactory::getDefaultItemSet)
        .def ("createItem", &ToolbarItemFactory::createItem)
    ;

    // ============================================================================================ juce::Toolbar

    py::class_<Toolbar, Component> classToolbar (m, "Toolbar");

    py::enum_<Toolbar::ToolbarItemStyle> (classToolbar, "ToolbarItemStyle")
        .value ("iconsOnly", Toolbar::ToolbarItemStyle::iconsOnly)
        .value ("iconsWithText", Toolbar::ToolbarItemStyle::iconsWithText)
        .value ("textOnly", Toolbar::ToolbarItemStyle::textOnly)
        .export_values();

    py::enum_<Toolbar::CustomisationFlags> (classToolbar, "CustomisationFlags")
        .value ("allowIconsOnlyChoice", Toolbar::CustomisationFlags::allowIconsOnlyChoice)
        .value ("allowIconsWithTextChoice", Toolbar::CustomisationFlags::allowIconsWithTextChoice)
        .value ("allowTextOnlyChoice", Toolbar::CustomisationFlags::allowTextOnlyChoice)
        .value ("showResetToDefaultsButton", Toolbar::CustomisationFlags::showResetToDefaultsButton)
        .value ("allCustomisationOptionsEnabled", Toolbar::CustomisationFlags::allCustomisationOptionsEnabled)
        .export_values();

    classToolbar
        .def (py::init<>())
        .def ("setVertical", &Toolbar::setVertical)
        .def ("isVertical", &Toolbar::isVertical)
        .def ("getThickness", &Toolbar::getThickness)
        .def ("getLength", &Toolbar::getLength)
        .def ("clear", &Toolbar::clear)
        .def ("addItem", &Toolbar::addItem)
        .def ("removeToolbarItem", &Toolbar::removeToolbarItem)
        .def ("removeAndReturnItem", &Toolbar::removeAndReturnItem, py::return_value_policy::reference)
        .def ("getNumItems", &Toolbar::getNumItems)
        .def ("getItemId", &Toolbar::getItemId)
        .def ("getItemComponent", &Toolbar::getItemComponent, py::return_value_policy::reference)
        .def ("addDefaultItems", &Toolbar::addDefaultItems)
        .def ("getStyle", &Toolbar::getStyle)
        .def ("setStyle", &Toolbar::setStyle)
        .def ("showCustomisationDialog", &Toolbar::showCustomisationDialog)
        .def ("setEditingActive", &Toolbar::setEditingActive)
        .def ("toString", &Toolbar::toString)
        .def ("restoreFromString", &Toolbar::restoreFromString)
    ;

    classToolbar.attr ("backgroundColourId") = py::int_(static_cast<int> (Toolbar::backgroundColourId));
    classToolbar.attr ("separatorColourId") = py::int_(static_cast<int> (Toolbar::separatorColourId));
    classToolbar.attr ("buttonMouseOverBackgroundColourId") = py::int_(static_cast<int> (Toolbar::buttonMouseOverBackgroundColourId));
    classToolbar.attr ("buttonMouseDownBackgroundColourId") = py::int_(static_cast<int> (Toolbar::buttonMouseDownBackgroundColourId));
    classToolbar.attr ("labelTextColourId") = py::int_(static_cast<int> (Toolbar::labelTextColourId));
    classToolbar.attr ("editingModeOutlineColourId") = py::int_(static_cast<int> (Toolbar::editingModeOutlineColourId));

    // ============================================================================================ juce::ToolbarItemComponent

    struct PyToolbarItemComponent : ToolbarItemComponent
    {
        using ToolbarItemComponent::ToolbarItemComponent;

        void setStyle (const Toolbar::ToolbarItemStyle& newStyle) override
        {
            PYBIND11_OVERRIDE(void, ToolbarItemComponent, setStyle, newStyle);
        }

        bool getToolbarItemSizes (int toolbarThickness, bool isToolbarVertical, int& preferredSize, int& minSize, int& maxSize) override
        {
            //PYBIND11_OVERRIDE_PURE(bool, ToolbarItemComponent, getToolbarItemSizes, toolbarThickness, ...);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override(static_cast<ToolbarItemComponent*>(this), "getToolbarItemSizes"); override_)
                {
                    auto result = override_ (toolbarThickness, isToolbarVertical, std::ref (preferredSize), std::ref (minSize), std::ref (maxSize));

                    return py::detail::cast_safe<bool> (std::move(result));
                }
            }

            py::pybind11_fail("Tried to call pure virtual function \"ToolbarItemComponent::getToolbarItemSizes\"");
        }

        void paintButtonArea (Graphics& g, int width, int height, bool isMouseOver, bool isMouseDown) override
        {
            //PYBIND11_OVERRIDE_PURE(void, ToolbarItemComponent, paintButtonArea, g, width, height, isMouseOver, isMouseDown);

            {
                py::gil_scoped_acquire gil;

                if (py::function override_ = py::get_override(static_cast<ToolbarItemComponent*>(this), "paintButtonArea"); override_)
                {
                    override_ (std::addressof (g), width, height, isMouseOver, isMouseDown);

                    return;
                }
            }

            py::pybind11_fail("Tried to call pure virtual function \"ToolbarItemComponent::paintButtonArea\"");
        }

        void contentAreaChanged (const Rectangle<int>& newBounds) override
        {
            PYBIND11_OVERRIDE_PURE(void, ToolbarItemComponent, contentAreaChanged, newBounds);
        }
    };

    py::class_<ToolbarItemComponent, Button, PyToolbarItemComponent> classToolbarItemComponent (m, "ToolbarItemComponent");

    py::enum_<ToolbarItemComponent::ToolbarEditingMode> (classToolbarItemComponent, "ToolbarEditingMode")
        .value ("normalMode", ToolbarItemComponent::ToolbarEditingMode::normalMode)
        .value ("editableOnToolbar", ToolbarItemComponent::ToolbarEditingMode::editableOnToolbar)
        .value ("editableOnPalette", ToolbarItemComponent::ToolbarEditingMode::editableOnPalette)
        .export_values();

    classToolbarItemComponent
        .def (py::init<int, const String&, bool>())
        .def ("getItemId", &ToolbarItemComponent::getItemId)
        .def ("getToolbar", &ToolbarItemComponent::getToolbar, py::return_value_policy::reference)
        .def ("isToolbarVertical", &ToolbarItemComponent::isToolbarVertical)
        .def ("getStyle", &ToolbarItemComponent::getStyle)
        .def ("setStyle", &ToolbarItemComponent::setStyle)
        .def ("getContentArea", &ToolbarItemComponent::getContentArea)
        .def ("getToolbarItemSizes", &ToolbarItemComponent::getToolbarItemSizes)
        .def ("paintButtonArea", &ToolbarItemComponent::paintButtonArea)
        .def ("contentAreaChanged", &ToolbarItemComponent::contentAreaChanged)
        .def ("setEditingMode", &ToolbarItemComponent::setEditingMode)
        .def ("getEditingMode", &ToolbarItemComponent::getEditingMode)
    ;

    // ============================================================================================ juce::Slider

    struct PyMenuBarModel : MenuBarModel
    {
        StringArray getMenuBarNames () override
        {
            PYBIND11_OVERRIDE_PURE (StringArray, MenuBarModel, getMenuBarNames);
        }

        PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName) override
        {
            PYBIND11_OVERRIDE_PURE (PopupMenu, MenuBarModel, getMenuForIndex, topLevelMenuIndex, menuName);
        }

        void menuItemSelected (int menuItemID, int topLevelMenuIndex) override
        {
            PYBIND11_OVERRIDE_PURE (void, MenuBarModel, menuItemSelected, menuItemID, topLevelMenuIndex);
        }

        void menuBarActivated (bool isActive) override
        {
            PYBIND11_OVERRIDE (void, MenuBarModel, menuBarActivated, isActive);
        }
    };

    struct PyMenuBarModelListener : MenuBarModel::Listener
    {
        void menuBarItemsChanged (MenuBarModel* menuBarModel) override
        {
            PYBIND11_OVERRIDE_PURE (void, MenuBarModel::Listener, menuBarItemsChanged, menuBarModel);
        }

        void menuCommandInvoked (MenuBarModel* menuBarModel, const ApplicationCommandTarget::InvocationInfo& info) override
        {
            PYBIND11_OVERRIDE_PURE (void, MenuBarModel::Listener, menuCommandInvoked, menuBarModel, info);
        }

        void menuBarActivated (MenuBarModel* menuBarModel, bool isActive) override
        {
            PYBIND11_OVERRIDE_PURE (void, MenuBarModel::Listener, menuBarActivated, menuBarModel, isActive);
        }
    };

    py::class_<MenuBarModel, PyMenuBarModel> classMenuBarModel (m, "MenuBarModel");

    py::class_<MenuBarModel::Listener, PyMenuBarModelListener> classMenuBarModelListener (m, "Listener");

    classMenuBarModelListener
        .def (py::init<>())
        .def ("menuBarItemsChanged", &MenuBarModel::Listener::menuBarItemsChanged)
        .def ("menuCommandInvoked", &MenuBarModel::Listener::menuCommandInvoked)
        .def ("menuBarActivated", &MenuBarModel::Listener::menuBarActivated)
    ;

    classMenuBarModel
        .def (py::init<>())
        .def ("menuItemsChanged", &MenuBarModel::menuItemsChanged)
    //.def ("setApplicationCommandManagerToWatch", &MenuBarModel::setApplicationCommandManagerToWatch)
        .def ("addListener", &MenuBarModel::addListener)
        .def ("removeListener", &MenuBarModel::removeListener)
        .def ("getMenuBarNames", &MenuBarModel::getMenuBarNames)
        .def ("getMenuForIndex", &MenuBarModel::getMenuForIndex)
        .def ("menuItemSelected", &MenuBarModel::menuItemSelected)
        .def ("menuBarActivated", &MenuBarModel::menuBarActivated)
#if JUCE_MAC
        .def_static ("setMacMainMenu", &MenuBarModel::setMacMainMenu)
        .def_static ("getMacMainMenu", &MenuBarModel::getMacMainMenu, py::return_value_policy::reference)
    //.def_static ("getMacExtraAppleItemsMenu", &MenuBarModel::getMacExtraAppleItemsMenu, py::return_value_policy::reference)
#endif
    ;

    // ============================================================================================ juce::Slider

    py::class_<Slider, Component> (m, "Slider")
        .def ("getMouseDragSensitivity", &Slider::getMouseDragSensitivity)
        .def ("getVelocityBasedMode", &Slider::getVelocityBasedMode)
        .def ("getVelocitySensitivity", &Slider::getVelocitySensitivity)
        .def ("getVelocityThreshold", &Slider::getVelocityThreshold)
        .def ("getVelocityOffset", &Slider::getVelocityOffset)
        .def ("getVelocityModeIsSwappable", &Slider::getVelocityModeIsSwappable)
        .def ("getSkewFactor", &Slider::getSkewFactor)
        .def ("isSymmetricSkew", &Slider::isSymmetricSkew)
        .def ("getTextBoxWidth", &Slider::getTextBoxWidth)
        .def ("getTextBoxHeight", &Slider::getTextBoxHeight)
        .def ("isTextBoxEditable", &Slider::isTextBoxEditable)
        .def ("showTextBox", &Slider::showTextBox)
        .def ("hideTextBox", &Slider::hideTextBox)
        .def ("getValue", &Slider::getValue)
        .def ("getMaximum", &Slider::getMaximum)
        .def ("getMinimum", &Slider::getMinimum)
        .def ("getInterval", &Slider::getInterval)
        .def ("getMinValue", &Slider::getMinValue)
        .def ("getMaxValue", &Slider::getMaxValue)
        .def ("getDoubleClickReturnValue", &Slider::getDoubleClickReturnValue)
        .def ("isDoubleClickReturnEnabled", &Slider::isDoubleClickReturnEnabled)
        .def ("getSliderSnapsToMousePosition", &Slider::getSliderSnapsToMousePosition)
        .def ("getCurrentPopupDisplay", &Slider::getCurrentPopupDisplay)
        .def ("isScrollWheelEnabled", &Slider::isScrollWheelEnabled)
        .def ("getThumbBeingDragged", &Slider::getThumbBeingDragged)
        .def ("getValueFromText", &Slider::getValueFromText)
        .def ("getTextFromValue", &Slider::getTextFromValue)
        .def ("getTextValueSuffix", &Slider::getTextValueSuffix)
        .def ("getNumDecimalPlacesToDisplay", &Slider::getNumDecimalPlacesToDisplay)
        .def ("proportionOfLengthToValue", &Slider::proportionOfLengthToValue)
        .def ("valueToProportionOfLength", &Slider::valueToProportionOfLength)
        .def ("getPositionOfValue", &Slider::getPositionOfValue)
        .def ("updateText", &Slider::updateText)
        .def ("isHorizontal", &Slider::isHorizontal)
        .def ("isVertical", &Slider::isVertical)
        .def ("isRotary", &Slider::isRotary)
        .def ("isBar", &Slider::isBar)
        .def ("isTwoValue", &Slider::isTwoValue)
        .def ("isThreeValue", &Slider::isThreeValue)
    ;

    // ============================================================================================ juce::TopLevelWindow

    py::class_<TopLevelWindow, Component> classTopLevelWindow (m, "TopLevelWindow");

    classTopLevelWindow
        .def (py::init<const String&, bool>())
        .def ("isActiveWindow", &TopLevelWindow::isActiveWindow)
        .def ("centreAroundComponent", &TopLevelWindow::centreAroundComponent)
        .def ("setDropShadowEnabled", &TopLevelWindow::setDropShadowEnabled)
        .def ("isDropShadowEnabled", &TopLevelWindow::isDropShadowEnabled)
        .def ("setUsingNativeTitleBar", &TopLevelWindow::setUsingNativeTitleBar)
        .def ("isUsingNativeTitleBar", &TopLevelWindow::isUsingNativeTitleBar)
        .def_static ("getNumTopLevelWindows", &TopLevelWindow::getNumTopLevelWindows)
        .def_static ("getTopLevelWindow", &TopLevelWindow::getTopLevelWindow, py::return_value_policy::reference)
        .def_static ("getActiveTopLevelWindow", &TopLevelWindow::getActiveTopLevelWindow, py::return_value_policy::reference)
        .def ("addToDesktop", py::overload_cast<>(&TopLevelWindow::addToDesktop))
    ;

    // ============================================================================================ juce::ResizableWindow

    py::class_<ResizableWindow, TopLevelWindow> classResizableWindow (m, "ResizableWindow");

    classResizableWindow
        .def (py::init<const String&, bool>())
        .def (py::init<const String&, Colour, bool>())
        .def ("getBackgroundColour", &ResizableWindow::getBackgroundColour)
        .def ("setBackgroundColour", &ResizableWindow::setBackgroundColour)
        .def ("setResizable", &ResizableWindow::setResizable)
        .def ("isResizable", &ResizableWindow::isResizable)
        .def ("setResizeLimits", &ResizableWindow::setResizeLimits)
        .def ("setDraggable", &ResizableWindow::setDraggable)
        .def ("isDraggable", &ResizableWindow::isDraggable)
    //.def ("getConstrainer", &ResizableWindow::getConstrainer)
    //.def ("setConstrainer", &ResizableWindow::setConstrainer)
        .def ("setBoundsConstrained", &ResizableWindow::setBoundsConstrained)
        .def ("isFullScreen", &ResizableWindow::isFullScreen)
        .def ("setFullScreen", &ResizableWindow::setFullScreen)
        .def ("isMinimised", &ResizableWindow::isMinimised)
        .def ("setMinimised", &ResizableWindow::setMinimised)
        .def ("isKioskMode", &ResizableWindow::isKioskMode)
        .def ("getWindowStateAsString", &ResizableWindow::getWindowStateAsString)
        .def ("restoreWindowStateFromString", &ResizableWindow::restoreWindowStateFromString)
        .def ("getContentComponent", &ResizableWindow::getContentComponent, py::return_value_policy::reference)
        .def ("setContentOwned", &ResizableWindow::setContentOwned)
        .def ("setContentNonOwned", &ResizableWindow::setContentNonOwned)
        .def ("clearContentComponent", &ResizableWindow::clearContentComponent)
        .def ("setContentComponentSize", &ResizableWindow::setContentComponentSize)
    //.def ("getBorderThickness", &ResizableWindow::getBorderThickness)
    //.def ("getContentComponentBorder", &ResizableWindow::getContentComponentBorder)
    ;

    classResizableWindow.attr ("backgroundColourId") = py::int_(static_cast<int> (ResizableWindow::backgroundColourId));

    // ============================================================================================ juce::DocumentWindow

    class PyDocumentWindow : public DocumentWindow
    {
        using DocumentWindow::DocumentWindow;

        void closeButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, closeButtonPressed);
        }

        void minimiseButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, minimiseButtonPressed);
        }

        void maximiseButtonPressed() override
        {
            PYBIND11_OVERRIDE(void, DocumentWindow, maximiseButtonPressed);
        }
    };

    py::class_<DocumentWindow, ResizableWindow, PyDocumentWindow> classDocumentWindow (m, "DocumentWindow");

    py::enum_<DocumentWindow::TitleBarButtons> (classDocumentWindow, "TitleBarButtons", py::arithmetic())
        .value("minimiseButton", DocumentWindow::minimiseButton)
        .value("maximiseButton", DocumentWindow::maximiseButton)
        .value("closeButton", DocumentWindow::closeButton)
        .value("allButtons", DocumentWindow::allButtons)
        .export_values();

    classDocumentWindow
        .def (py::init<const String&, Colour, int>())
        .def (py::init<const String&, Colour, int, bool>())
        .def ("setName", &DocumentWindow::setName)
        .def ("setIcon", &DocumentWindow::setIcon)
        .def ("setTitleBarHeight", &DocumentWindow::setTitleBarHeight)
        .def ("getTitleBarHeight", &DocumentWindow::getTitleBarHeight)
        .def ("setTitleBarButtonsRequired", &DocumentWindow::setTitleBarButtonsRequired)
        .def ("setTitleBarTextCentred", &DocumentWindow::setTitleBarTextCentred)
    //.def ("setMenuBar", &DocumentWindow::setMenuBar)
        .def ("getMenuBarComponent", &DocumentWindow::getMenuBarComponent, py::return_value_policy::reference)
        .def ("setMenuBarComponent", &DocumentWindow::setMenuBarComponent)
        .def ("closeButtonPressed", &DocumentWindow::closeButtonPressed)
        .def ("minimiseButtonPressed", &DocumentWindow::minimiseButtonPressed)
        .def ("maximiseButtonPressed", &DocumentWindow::maximiseButtonPressed)
        .def ("getCloseButton", &DocumentWindow::getCloseButton, py::return_value_policy::reference)
        .def ("getMinimiseButton", &DocumentWindow::getMinimiseButton, py::return_value_policy::reference)
        .def ("getMaximiseButton", &DocumentWindow::getMaximiseButton, py::return_value_policy::reference)
    ;

    classDocumentWindow.attr ("textColourId") = py::int_(static_cast<int> (DocumentWindow::textColourId));
}

} // namespace popsicle::Bindings

#endif
