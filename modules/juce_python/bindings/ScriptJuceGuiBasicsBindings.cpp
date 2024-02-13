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

#include "ScriptJuceGuiBasicsBindings.h"
#include "../scripting/ScriptBindings.h"
#include "../utilities/ClassDemangling.h"

#include <string_view>
#include <typeinfo>
#include <tuple>

// =================================================================================================

namespace PYBIND11_NAMESPACE {

template <>
struct polymorphic_type_hook<juce::Component>
{
    static const void* get (const juce::Component* src, const std::type_info*& type)
    {
        if (src == nullptr)
            return src;

        auto& map = popsicle::Bindings::getComponentTypeMap();
        auto demangledName = popsicle::Helpers::demangleClassName (typeid (*src).name());

        auto it = map.typeMap.find (demangledName);
        if (it != map.typeMap.end())
            return it->second (src, type);

        return src;
    }
};

} // namespace PYBIND11_NAMESPACE

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

// ============================================================================================

void registerJuceGuiBasicsBindings (py::module_& m)
{
    // ============================================================================================ juce::JUCEApplication

    py::class_<JUCEApplication, PyJUCEApplication> classJUCEApplication (m, "JUCEApplication");

    classJUCEApplication
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

    // ============================================================================================ juce::ModifierKeys

    py::class_<ModifierKeys> classModifierKeys (m, "ModifierKeys");

    Helpers::makeArithmeticEnum<ModifierKeys::Flags> (classModifierKeys, "Flags")
        .value ("noModifiers", ModifierKeys::Flags::noModifiers)
        .value ("shiftModifier", ModifierKeys::Flags::shiftModifier)
        .value ("ctrlModifier", ModifierKeys::Flags::ctrlModifier)
        .value ("altModifier", ModifierKeys::Flags::altModifier)
        .value ("leftButtonModifier", ModifierKeys::Flags::leftButtonModifier)
        .value ("rightButtonModifier", ModifierKeys::Flags::rightButtonModifier)
        .value ("middleButtonModifier", ModifierKeys::Flags::middleButtonModifier)
        .value ("commandModifier", ModifierKeys::Flags::commandModifier)
        .value ("popupMenuClickModifier", ModifierKeys::Flags::popupMenuClickModifier)
        .value ("allKeyboardModifiers", ModifierKeys::Flags::allKeyboardModifiers)
        .value ("allMouseButtonModifiers", ModifierKeys::Flags::allMouseButtonModifiers)
        .value ("ctrlAltCommandModifiers", ModifierKeys::Flags::ctrlAltCommandModifiers)
        .export_values();

    classModifierKeys
        .def (py::init<>())
        .def (py::init<int>())
        .def (py::init<ModifierKeys::Flags>())
        .def (py::init<const ModifierKeys&>())
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
        .def ("withoutFlags", [](const ModifierKeys& self, ModifierKeys::Flags flags) { return self.withoutFlags (static_cast<int> (flags)); })
        .def ("withFlags", &ModifierKeys::withFlags)
        .def ("withFlags", [](const ModifierKeys& self, ModifierKeys::Flags flags) { return self.withFlags (static_cast<int> (flags)); })
        .def ("testFlags", &ModifierKeys::testFlags)
        .def ("testFlags", [](const ModifierKeys& self, ModifierKeys::Flags flags) { return self.testFlags (static_cast<int> (flags)); })
        .def ("getNumMouseButtonsDown", &ModifierKeys::getNumMouseButtonsDown)
        .def_readonly_static ("currentModifiers", &ModifierKeys::currentModifiers, py::return_value_policy::copy)
        .def_static ("getCurrentModifiers", &ModifierKeys::getCurrentModifiers)
        .def_static ("getCurrentModifiersRealtime", &ModifierKeys::getCurrentModifiersRealtime)
    ;

    // ============================================================================================ juce::KeyPress

    py::class_<KeyPress> classKeyPress (m, "KeyPress");

    classKeyPress
        .def (py::init<>())
        .def (py::init<int>(), "keyCode"_a)
        .def (py::init ([](char keyCode)
        {
            return new KeyPress (static_cast<int> (keyCode));
        }), "keyCode"_a)
        .def (py::init ([](int keyCode, ModifierKeys modifiers, int textCharacter)
            { return new KeyPress (keyCode, modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](int keyCode, ModifierKeys::Flags modifiers, int textCharacter)
            { return new KeyPress (keyCode, modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](int keyCode, ModifierKeys modifiers, wchar_t textCharacter)
            { return new KeyPress (keyCode, modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](int keyCode, ModifierKeys::Flags modifiers, wchar_t textCharacter)
            { return new KeyPress (keyCode, modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](int keyCode, int modifiers, int textCharacter)
            { return new KeyPress (keyCode, static_cast<ModifierKeys> (modifiers), static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](int keyCode, int modifiers, wchar_t textCharacter)
            { return new KeyPress (keyCode, static_cast<ModifierKeys> (modifiers), static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, ModifierKeys modifiers, int textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, ModifierKeys::Flags modifiers, int textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, ModifierKeys modifiers, wchar_t textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, ModifierKeys::Flags modifiers, wchar_t textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), modifiers, static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, int modifiers, int textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), static_cast<ModifierKeys> (modifiers), static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init ([](char keyCode, int modifiers, wchar_t textCharacter)
            { return new KeyPress (static_cast<int> (keyCode), static_cast<ModifierKeys> (modifiers), static_cast<juce_wchar> (textCharacter)); }), "keyCode"_a, "modifiers"_a, "textCharacter"_a)
        .def (py::init<const KeyPress&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def (py::self == int())
        .def (py::self != int())
        .def ("isValid", &KeyPress::isValid)
        .def ("getKeyCode", &KeyPress::getKeyCode)
        .def ("getModifiers", &KeyPress::getModifiers)
        .def ("getTextCharacter", &KeyPress::getTextCharacter)
        .def ("isKeyCode", &KeyPress::isKeyCode)
        .def_static ("createFromDescription", &KeyPress::createFromDescription)
        .def ("getTextDescription", &KeyPress::getTextDescription)
        .def ("getTextDescriptionWithIcons", &KeyPress::getTextDescriptionWithIcons)
        .def ("isCurrentlyDown", &KeyPress::isCurrentlyDown)
        .def_static ("isKeyCurrentlyDown", &KeyPress::isKeyCurrentlyDown)
    ;

    classKeyPress.attr ("spaceKey") = py::int_ (KeyPress::spaceKey);
    classKeyPress.attr ("escapeKey") = py::int_ (KeyPress::escapeKey);
    classKeyPress.attr ("returnKey") = py::int_ (KeyPress::returnKey);
    classKeyPress.attr ("tabKey") = py::int_ (KeyPress::tabKey);
    classKeyPress.attr ("deleteKey") = py::int_ (KeyPress::deleteKey);
    classKeyPress.attr ("backspaceKey") = py::int_ (KeyPress::backspaceKey);
    classKeyPress.attr ("insertKey") = py::int_ (KeyPress::insertKey);
    classKeyPress.attr ("upKey") = py::int_ (KeyPress::upKey);
    classKeyPress.attr ("downKey") = py::int_ (KeyPress::downKey);
    classKeyPress.attr ("leftKey") = py::int_ (KeyPress::leftKey);
    classKeyPress.attr ("rightKey") = py::int_ (KeyPress::rightKey);
    classKeyPress.attr ("pageUpKey") = py::int_ (KeyPress::pageUpKey);
    classKeyPress.attr ("pageDownKey") = py::int_ (KeyPress::pageDownKey);
    classKeyPress.attr ("homeKey") = py::int_ (KeyPress::homeKey);
    classKeyPress.attr ("endKey") = py::int_ (KeyPress::endKey);
    classKeyPress.attr ("F1Key") = py::int_ (KeyPress::F1Key);
    classKeyPress.attr ("F2Key") = py::int_ (KeyPress::F2Key);
    classKeyPress.attr ("F3Key") = py::int_ (KeyPress::F3Key);
    classKeyPress.attr ("F4Key") = py::int_ (KeyPress::F4Key);
    classKeyPress.attr ("F5Key") = py::int_ (KeyPress::F5Key);
    classKeyPress.attr ("F6Key") = py::int_ (KeyPress::F6Key);
    classKeyPress.attr ("F7Key") = py::int_ (KeyPress::F7Key);
    classKeyPress.attr ("F8Key") = py::int_ (KeyPress::F8Key);
    classKeyPress.attr ("F9Key") = py::int_ (KeyPress::F9Key);
    classKeyPress.attr ("F10Key") = py::int_ (KeyPress::F10Key);
    classKeyPress.attr ("F11Key") = py::int_ (KeyPress::F11Key);
    classKeyPress.attr ("F12Key") = py::int_ (KeyPress::F12Key);
    classKeyPress.attr ("F13Key") = py::int_ (KeyPress::F13Key);
    classKeyPress.attr ("F14Key") = py::int_ (KeyPress::F14Key);
    classKeyPress.attr ("F15Key") = py::int_ (KeyPress::F15Key);
    classKeyPress.attr ("F16Key") = py::int_ (KeyPress::F16Key);
    classKeyPress.attr ("F17Key") = py::int_ (KeyPress::F17Key);
    classKeyPress.attr ("F18Key") = py::int_ (KeyPress::F18Key);
    classKeyPress.attr ("F19Key") = py::int_ (KeyPress::F19Key);
    classKeyPress.attr ("F20Key") = py::int_ (KeyPress::F20Key);
    classKeyPress.attr ("F21Key") = py::int_ (KeyPress::F21Key);
    classKeyPress.attr ("F22Key") = py::int_ (KeyPress::F22Key);
    classKeyPress.attr ("F23Key") = py::int_ (KeyPress::F23Key);
    classKeyPress.attr ("F24Key") = py::int_ (KeyPress::F24Key);
    classKeyPress.attr ("F25Key") = py::int_ (KeyPress::F25Key);
    classKeyPress.attr ("F26Key") = py::int_ (KeyPress::F26Key);
    classKeyPress.attr ("F27Key") = py::int_ (KeyPress::F27Key);
    classKeyPress.attr ("F28Key") = py::int_ (KeyPress::F28Key);
    classKeyPress.attr ("F29Key") = py::int_ (KeyPress::F29Key);
    classKeyPress.attr ("F30Key") = py::int_ (KeyPress::F30Key);
    classKeyPress.attr ("F31Key") = py::int_ (KeyPress::F31Key);
    classKeyPress.attr ("F32Key") = py::int_ (KeyPress::F32Key);
    classKeyPress.attr ("F33Key") = py::int_ (KeyPress::F33Key);
    classKeyPress.attr ("F34Key") = py::int_ (KeyPress::F34Key);
    classKeyPress.attr ("F35Key") = py::int_ (KeyPress::F35Key);
    classKeyPress.attr ("numberPad0") = py::int_ (KeyPress::numberPad0);
    classKeyPress.attr ("numberPad1") = py::int_ (KeyPress::numberPad1);
    classKeyPress.attr ("numberPad2") = py::int_ (KeyPress::numberPad2);
    classKeyPress.attr ("numberPad3") = py::int_ (KeyPress::numberPad3);
    classKeyPress.attr ("numberPad4") = py::int_ (KeyPress::numberPad4);
    classKeyPress.attr ("numberPad5") = py::int_ (KeyPress::numberPad5);
    classKeyPress.attr ("numberPad6") = py::int_ (KeyPress::numberPad6);
    classKeyPress.attr ("numberPad7") = py::int_ (KeyPress::numberPad7);
    classKeyPress.attr ("numberPad8") = py::int_ (KeyPress::numberPad8);
    classKeyPress.attr ("numberPad9") = py::int_ (KeyPress::numberPad9);
    classKeyPress.attr ("numberPadAdd") = py::int_ (KeyPress::numberPadAdd);
    classKeyPress.attr ("numberPadSubtract") = py::int_ (KeyPress::numberPadSubtract);
    classKeyPress.attr ("numberPadMultiply") = py::int_ (KeyPress::numberPadMultiply);
    classKeyPress.attr ("numberPadDivide") = py::int_ (KeyPress::numberPadDivide);
    classKeyPress.attr ("numberPadSeparator") = py::int_ (KeyPress::numberPadSeparator);
    classKeyPress.attr ("numberPadDecimalPoint") = py::int_ (KeyPress::numberPadDecimalPoint);
    classKeyPress.attr ("numberPadEquals") = py::int_ (KeyPress::numberPadEquals);
    classKeyPress.attr ("numberPadDelete") = py::int_ (KeyPress::numberPadDelete);
    classKeyPress.attr ("playKey") = py::int_ (KeyPress::playKey);
    classKeyPress.attr ("stopKey") = py::int_ (KeyPress::stopKey);
    classKeyPress.attr ("fastForwardKey") = py::int_ (KeyPress::fastForwardKey);
    classKeyPress.attr ("rewindKey") = py::int_ (KeyPress::rewindKey);

    // ============================================================================================ juce::KeyListener

    py::class_<KeyListener, PyKeyListener> classKeyListener (m, "KeyListener");

    classKeyListener
        .def (py::init<>())
        .def ("keyPressed", &KeyListener::keyPressed)
        .def ("keyStateChanged", &KeyListener::keyStateChanged)
    ;

    // ============================================================================================ juce::TextInputTarget

    py::class_<TextInputTarget, PyTextInputTarget<>> classTextInputTarget (m, "TextInputTarget");

    classTextInputTarget
        .def (py::init<>())
        .def ("isTextInputActive", &TextInputTarget::isTextInputActive)
        .def ("getHighlightedRegion", &TextInputTarget::getHighlightedRegion)
        .def ("setHighlightedRegion", &TextInputTarget::setHighlightedRegion)
        .def ("setTemporaryUnderlining", &TextInputTarget::setTemporaryUnderlining)
        .def ("getTextInRange", &TextInputTarget::getTextInRange)
        .def ("insertTextAtCaret", &TextInputTarget::insertTextAtCaret)
        .def ("getCaretPosition", &TextInputTarget::getCaretPosition)
        .def ("getCaretRectangleForCharIndex", &TextInputTarget::getCaretRectangleForCharIndex)
        .def ("getTotalNumChars", &TextInputTarget::getTotalNumChars)
        .def ("getCharIndexForPoint", &TextInputTarget::getCharIndexForPoint)
        .def ("getTextBounds", &TextInputTarget::getTextBounds)
        .def ("getKeyboardType", &TextInputTarget::getKeyboardType)
    ;

    // ============================================================================================ juce::SystemClipboard

    py::class_<SystemClipboard> classSystemClipboard (m, "SystemClipboard");

    classSystemClipboard
        .def_static ("copyTextToClipboard", &SystemClipboard::copyTextToClipboard)
        .def_static ("getTextFromClipboard", &SystemClipboard::getTextFromClipboard)
    ;

    // ============================================================================================ juce::MouseInputSource

    py::class_<MouseInputSource> classMouseInputSource (m, "MouseInputSource");

    py::enum_<MouseInputSource::InputSourceType> (classMouseInputSource, "InputSourceType")
        .value ("mouse", MouseInputSource::InputSourceType::mouse)
        .value ("touch", MouseInputSource::InputSourceType::touch)
        .value ("pen", MouseInputSource::InputSourceType::pen)
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
        .def (py::init<MouseInputSource, Point<float>, ModifierKeys, float, float, float, float, float, Component*, Component*, Time, Point<float>, Time, int, bool>(),
            "source"_a, "position"_a, "modifiers"_a, "pressure"_a, "orientation"_a, "rotation"_a, "tiltX"_a, "tiltY"_a,
            "eventComponent"_a, "originator"_a, "eventTime"_a, "mouseDownPos"_a, "mouseDownTime"_a, "numberOfClicks"_a, "mouseWasDragged"_a)
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

    py::class_<MouseListener, PyMouseListener<>> classMouseListener (m, "MouseListener");

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
        .value ("ParentCursor", MouseCursor::StandardCursorType::ParentCursor)
        .value ("NoCursor", MouseCursor::StandardCursorType::NoCursor)
        .value ("NormalCursor", MouseCursor::StandardCursorType::NormalCursor)
        .value ("WaitCursor", MouseCursor::StandardCursorType::WaitCursor)
        .value ("IBeamCursor", MouseCursor::StandardCursorType::IBeamCursor)
        .value ("CrosshairCursor", MouseCursor::StandardCursorType::CrosshairCursor)
        .value ("CopyingCursor", MouseCursor::StandardCursorType::CopyingCursor)
        .value ("PointingHandCursor", MouseCursor::StandardCursorType::PointingHandCursor)
        .value ("DraggingHandCursor", MouseCursor::StandardCursorType::DraggingHandCursor)
        .value ("LeftRightResizeCursor", MouseCursor::StandardCursorType::LeftRightResizeCursor)
        .value ("UpDownResizeCursor", MouseCursor::StandardCursorType::UpDownResizeCursor)
        .value ("UpDownLeftRightResizeCursor", MouseCursor::StandardCursorType::UpDownLeftRightResizeCursor)
        .value ("TopEdgeResizeCursor", MouseCursor::StandardCursorType::TopEdgeResizeCursor)
        .value ("BottomEdgeResizeCursor", MouseCursor::StandardCursorType::BottomEdgeResizeCursor)
        .value ("LeftEdgeResizeCursor", MouseCursor::StandardCursorType::LeftEdgeResizeCursor)
        .value ("RightEdgeResizeCursor", MouseCursor::StandardCursorType::RightEdgeResizeCursor)
        .value ("TopLeftCornerResizeCursor", MouseCursor::StandardCursorType::TopLeftCornerResizeCursor)
        .value ("TopRightCornerResizeCursor", MouseCursor::StandardCursorType::TopRightCornerResizeCursor)
        .value ("BottomLeftCornerResizeCursor", MouseCursor::StandardCursorType::BottomLeftCornerResizeCursor)
        .value ("BottomRightCornerResizeCursor", MouseCursor::StandardCursorType::BottomRightCornerResizeCursor)
        .value ("NumStandardCursorTypes", MouseCursor::StandardCursorType::NumStandardCursorTypes)
        .export_values();

    classMouseCursor
        .def (py::init<>())
        .def (py::init<MouseCursor::StandardCursorType>())
        .def (py::init<const Image&, int, int>())
        .def (py::init<const Image&, int, int, float>())
        .def (py::init<const ScaledImage&, Point<int>>())
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

    py::class_<LookAndFeel, PyLookAndFeel<>> classLookAndFeel (m, "LookAndFeel");

    classLookAndFeel
    //.def (py::init<>())
        .def_static ("getDefaultLookAndFeel", &LookAndFeel::getDefaultLookAndFeel, py::return_value_policy::reference)
        .def_static ("setDefaultLookAndFeel", &LookAndFeel::setDefaultLookAndFeel)
        .def ("findColour", &LookAndFeel::findColour)
        .def ("setColour", &LookAndFeel::setColour)
        .def ("isColourSpecified", &LookAndFeel::isColourSpecified)
    //.def ("getTypefaceForFont", &LookAndFeel::getTypefaceForFont)
    //.def ("setDefaultSansSerifTypeface", &LookAndFeel::setDefaultSansSerifTypeface)
        .def ("setDefaultSansSerifTypefaceName", &LookAndFeel::setDefaultSansSerifTypefaceName)
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

    py::class_<LookAndFeel_V2, LookAndFeel> classLookAndFeel_V2 (m, "LookAndFeel_V2");
    py::class_<LookAndFeel_V1, LookAndFeel_V2> classLookAndFeel_V1  (m, "LookAndFeel_V1");
    py::class_<LookAndFeel_V3, LookAndFeel_V2> classLookAndFeel_V3 (m, "LookAndFeel_V3");
    py::class_<LookAndFeel_V4, LookAndFeel_V3> classLookAndFeel_V4 (m, "LookAndFeel_V4");

    py::class_<LookAndFeel_V4::ColourScheme> classLookAndFeel_V4ColourScheme (classLookAndFeel_V4, "ColourScheme");

    classLookAndFeel_V4ColourScheme
        .def (py::init ([](py::args coloursToUse)
        {
            if (coloursToUse.size() != static_cast<size_t> (LookAndFeel_V4::ColourScheme::numColours))
                py::pybind11_fail ("Must supply one colour for each UIColour item");

            std::tuple<Colour, Colour, Colour, Colour, Colour, Colour, Colour, Colour, Colour> colours;

            std::get<0> (colours) = coloursToUse[0].cast<Colour>();
            std::get<1> (colours) = coloursToUse[1].cast<Colour>();
            std::get<2> (colours) = coloursToUse[2].cast<Colour>();
            std::get<3> (colours) = coloursToUse[3].cast<Colour>();
            std::get<4> (colours) = coloursToUse[4].cast<Colour>();
            std::get<5> (colours) = coloursToUse[5].cast<Colour>();
            std::get<6> (colours) = coloursToUse[6].cast<Colour>();
            std::get<7> (colours) = coloursToUse[7].cast<Colour>();
            std::get<8> (colours) = coloursToUse[8].cast<Colour>();

            return std::apply ([](auto... c) { return LookAndFeel_V4::ColourScheme (c...); }, colours);
        }))
        .def (py::init<const LookAndFeel_V4::ColourScheme&>())
        .def ("getUIColour", &LookAndFeel_V4::ColourScheme::getUIColour)
        .def ("setUIColour", &LookAndFeel_V4::ColourScheme::setUIColour)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    classLookAndFeel_V4ColourScheme.attr ("windowBackground") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::windowBackground));
    classLookAndFeel_V4ColourScheme.attr ("widgetBackground") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::widgetBackground));
    classLookAndFeel_V4ColourScheme.attr ("menuBackground") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::menuBackground));
    classLookAndFeel_V4ColourScheme.attr ("outline") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::outline));
    classLookAndFeel_V4ColourScheme.attr ("defaultText") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::defaultText));
    classLookAndFeel_V4ColourScheme.attr ("defaultFill") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::defaultFill));
    classLookAndFeel_V4ColourScheme.attr ("highlightedText") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::highlightedText));
    classLookAndFeel_V4ColourScheme.attr ("highlightedFill") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::highlightedFill));
    classLookAndFeel_V4ColourScheme.attr ("menuText") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::menuText));
    classLookAndFeel_V4ColourScheme.attr ("numColours") = py::int_ (static_cast<int> (LookAndFeel_V4::ColourScheme::numColours));

    classLookAndFeel_V4
        .def (py::init<>())
        .def (py::init<LookAndFeel_V4::ColourScheme>())
        .def ("setColourScheme", &LookAndFeel_V4::setColourScheme)
        .def ("getCurrentColourScheme", &LookAndFeel_V4::getCurrentColourScheme, py::return_value_policy::reference)
        .def_static ("getDarkColourScheme", &LookAndFeel_V4::getDarkColourScheme)
        .def_static ("getMidnightColourScheme", &LookAndFeel_V4::getMidnightColourScheme)
        .def_static ("getGreyColourScheme", &LookAndFeel_V4::getGreyColourScheme)
        .def_static ("getLightColourScheme", &LookAndFeel_V4::getLightColourScheme)
    ;

    // ============================================================================================ juce::Desktop

    py::class_<Desktop, std::unique_ptr<Desktop, py::nodelete>> classDesktop (m, "Desktop");

    Helpers::makeArithmeticEnum<Desktop::DisplayOrientation> (classDesktop, "DisplayOrientation")
        .value ("upright", Desktop::upright)
        .value ("upsideDown", Desktop::upsideDown)
        .value ("rotatedClockwise", Desktop::rotatedClockwise)
        .value ("rotatedAntiClockwise", Desktop::rotatedAntiClockwise)
        .value ("allOrientations", Desktop::allOrientations)
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
        .def ("getMouseSources", [](const Desktop& self)
        {
            py::list result;
            for (auto& source : self.getMouseSources())
                result.append (source);
            return result;
        })
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

    // ============================================================================================ juce::ComponentTraverser

    py::class_<ComponentTraverser, PyComponentTraverser<>> classComponentTraverser (m, "ComponentTraverser");

    classComponentTraverser
        .def (py::init<>())
        .def ("getDefaultComponent", &ComponentTraverser::getDefaultComponent, py::return_value_policy::reference)
        .def ("getNextComponent", &ComponentTraverser::getNextComponent, py::return_value_policy::reference)
        .def ("getPreviousComponent", &ComponentTraverser::getPreviousComponent, py::return_value_policy::reference)
        .def ("getDefaultComponent", [](ComponentTraverser& self, Component* parentComponent)
        {
            py::list allComponents;

            for (auto component : self.getAllComponents (parentComponent))
                allComponents.append (py::cast (component, py::return_value_policy::reference));

            return allComponents;
        })
    ;

    py::class_<FocusTraverser, ComponentTraverser, PyFocusTraverser<>> (m, "FocusTraverser");

    // ============================================================================================ juce::ModalComponentManager

    py::class_<ModalComponentManager, std::unique_ptr<ModalComponentManager, py::nodelete>> classModalComponentManager (m, "ModalComponentManager");

    py::class_<ModalComponentManager::Callback, PyModalComponentManagerCallback> classModalComponentManagerCallback (classModalComponentManager, "Callback");

    classModalComponentManagerCallback
        .def (py::init<>())
        .def ("modalStateFinished", &ModalComponentManager::Callback::modalStateFinished)
    ;

    classModalComponentManager
        .def_static ("getInstance", &ModalComponentManager::getInstance, py::return_value_policy::reference)
        .def_static ("getInstanceWithoutCreating", &ModalComponentManager::getInstanceWithoutCreating, py::return_value_policy::reference)
        .def_static ("deleteInstance", &ModalComponentManager::deleteInstance)
        .def ("getNumModalComponents", &ModalComponentManager::getNumModalComponents)
        .def ("getModalComponent", &ModalComponentManager::getModalComponent, py::return_value_policy::reference)
        .def ("getNumModalComponents", &ModalComponentManager::getNumModalComponents)
        .def ("isModal", &ModalComponentManager::isModal)
        .def ("isFrontModalComponent", &ModalComponentManager::isFrontModalComponent)
        .def ("attachCallback", [](ModalComponentManager& self, Component* component, py::function callback)
        {
            self.attachCallback (component, new PyModalComponentManagerCallbackCallable (std::move (callback)));
        })
        .def ("bringModalComponentsToFront", &ModalComponentManager::bringModalComponentsToFront, "topOneShouldGrabFocus"_a = true)
        .def ("cancelAllModalComponents", &ModalComponentManager::cancelAllModalComponents)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runEventLoopForCurrentComponent", &ModalComponentManager::runEventLoopForCurrentComponent, py::call_guard<py::gil_scoped_release>())
#endif
    ;

    // ============================================================================================ juce::ComponentListener

    py::class_<ComponentListener, PyComponentListener> classComponentListener (m, "ComponentListener");

    classComponentListener
        .def (py::init<>())
        .def ("componentMovedOrResized", &ComponentListener::componentMovedOrResized)
        .def ("componentBroughtToFront", &ComponentListener::componentBroughtToFront)
        .def ("componentVisibilityChanged", &ComponentListener::componentVisibilityChanged)
        .def ("componentChildrenChanged", &ComponentListener::componentChildrenChanged)
        .def ("componentParentHierarchyChanged", &ComponentListener::componentParentHierarchyChanged)
        .def ("componentNameChanged", &ComponentListener::componentNameChanged)
        .def ("componentBeingDeleted", &ComponentListener::componentBeingDeleted)
        .def ("componentEnablementChanged", &ComponentListener::componentEnablementChanged)
    ;

    // ============================================================================================ juce::ComponentPeer

    py::class_<ComponentPeer> classComponentPeer (m, "ComponentPeer");

    Helpers::makeArithmeticEnum<ComponentPeer::StyleFlags> (classComponentPeer, "StyleFlags")
        .value ("windowAppearsOnTaskbar", ComponentPeer::StyleFlags::windowAppearsOnTaskbar)
        .value ("windowIsTemporary", ComponentPeer::StyleFlags::windowIsTemporary)
        .value ("windowIgnoresMouseClicks", ComponentPeer::StyleFlags::windowIgnoresMouseClicks)
        .value ("windowHasTitleBar", ComponentPeer::StyleFlags::windowHasTitleBar)
        .value ("windowIsResizable", ComponentPeer::StyleFlags::windowIsResizable)
        .value ("windowHasMinimiseButton", ComponentPeer::StyleFlags::windowHasMinimiseButton)
        .value ("windowHasMaximiseButton", ComponentPeer::StyleFlags::windowHasMaximiseButton)
        .value ("windowHasCloseButton", ComponentPeer::StyleFlags::windowHasCloseButton)
        .value ("windowHasDropShadow", ComponentPeer::StyleFlags::windowHasDropShadow)
        .value ("windowIgnoresKeyPresses", ComponentPeer::StyleFlags::windowIgnoresKeyPresses)
        .value ("windowRequiresSynchronousCoreGraphicsRendering", ComponentPeer::StyleFlags::windowRequiresSynchronousCoreGraphicsRendering)
        .value ("windowIsSemiTransparent", ComponentPeer::StyleFlags::windowIsSemiTransparent)
        .export_values();

    classComponentPeer
        .def ("getComponent", &ComponentPeer::getComponent, py::return_value_policy::reference)
        .def ("getStyleFlags", &ComponentPeer::getStyleFlags)
        .def ("getUniqueID", &ComponentPeer::getUniqueID)
        .def ("getNativeHandle", [](const ComponentPeer& self) { return PyNativeHandle (self.getNativeHandle()); })
        .def ("setVisible", &ComponentPeer::setVisible)
        .def ("setTitle", &ComponentPeer::setTitle)
        .def ("setDocumentEditedStatus", &ComponentPeer::setDocumentEditedStatus)
        .def ("setRepresentedFile", &ComponentPeer::setRepresentedFile)
        .def ("setBounds", &ComponentPeer::setBounds)
        .def ("updateBounds", &ComponentPeer::updateBounds)
        .def ("getBounds", &ComponentPeer::getBounds)
        .def ("localToGlobal", py::overload_cast<Point<float>> (&ComponentPeer::localToGlobal))
        .def ("globalToLocal", py::overload_cast<Point<float>> (&ComponentPeer::globalToLocal))
        .def ("localToGlobal", py::overload_cast<Point<int>> (&ComponentPeer::localToGlobal))
        .def ("globalToLocal", py::overload_cast<Point<int>> (&ComponentPeer::globalToLocal))
        .def ("localToGlobal", py::overload_cast<const Rectangle<int>&> (&ComponentPeer::localToGlobal))
        .def ("globalToLocal", py::overload_cast<const Rectangle<int>&> (&ComponentPeer::globalToLocal))
        .def ("localToGlobal", py::overload_cast<const Rectangle<float>&> (&ComponentPeer::localToGlobal))
        .def ("globalToLocal", py::overload_cast<const Rectangle<float>&> (&ComponentPeer::globalToLocal))
        .def ("getAreaCoveredBy", &ComponentPeer::getAreaCoveredBy)
        .def ("setMinimised", &ComponentPeer::setMinimised)
        .def ("isMinimised", &ComponentPeer::isMinimised)
        .def ("setFullScreen", &ComponentPeer::setFullScreen)
        .def ("isFullScreen", &ComponentPeer::isFullScreen)
        .def ("isKioskMode", &ComponentPeer::isKioskMode)
        .def ("setNonFullScreenBounds", &ComponentPeer::setNonFullScreenBounds)
        .def ("getNonFullScreenBounds", &ComponentPeer::getNonFullScreenBounds)
        .def ("setIcon", &ComponentPeer::setIcon)
    //.def ("setConstrainer", &ComponentPeer::setConstrainer)
    //.def ("startHostManagedResize", &ComponentPeer::startHostManagedResize)
    //.def ("getConstrainer", &ComponentPeer::getConstrainer)
        .def ("contains", &ComponentPeer::contains)
        .def ("getFrameSizeIfPresent", &ComponentPeer::getFrameSizeIfPresent)
        .def ("setAlwaysOnTop", &ComponentPeer::setAlwaysOnTop)
        .def ("toFront", &ComponentPeer::toFront)
        .def ("toBehind", &ComponentPeer::toBehind)
        .def ("handleBroughtToFront", &ComponentPeer::handleBroughtToFront)
        .def ("isFocused", &ComponentPeer::isFocused)
        .def ("grabFocus", &ComponentPeer::grabFocus)
        .def ("getLastFocusedSubcomponent", &ComponentPeer::getLastFocusedSubcomponent, py::return_value_policy::reference)
        .def ("closeInputMethodContext", &ComponentPeer::closeInputMethodContext)
        .def ("refreshTextInputTarget", &ComponentPeer::refreshTextInputTarget)
    //.def ("findCurrentTextInputTarget", &ComponentPeer::findCurrentTextInputTarget)
        .def ("repaint", &ComponentPeer::repaint)
        .def ("performAnyPendingRepaintsNow", &ComponentPeer::performAnyPendingRepaintsNow)
        .def ("setAlpha", &ComponentPeer::setAlpha)
        .def_static ("getNumPeers", &ComponentPeer::getNumPeers)
        .def_static ("getPeer", &ComponentPeer::getPeer, py::return_value_policy::reference)
        .def_static ("getPeerFor", &ComponentPeer::getPeerFor, py::return_value_policy::reference)
        .def_static ("isValidPeer", &ComponentPeer::isValidPeer)
        .def ("getAvailableRenderingEngines", &ComponentPeer::getAvailableRenderingEngines)
        .def ("getCurrentRenderingEngine", &ComponentPeer::getCurrentRenderingEngine)
        .def ("setCurrentRenderingEngine", &ComponentPeer::setCurrentRenderingEngine)
        .def_static ("getCurrentModifiersRealtime", &ComponentPeer::getCurrentModifiersRealtime)
    //.def ("addScaleFactorListener", &ComponentPeer::addScaleFactorListener)
    //.def ("removeScaleFactorListener", &ComponentPeer::removeScaleFactorListener)
    //.def ("addVBlankListener", &ComponentPeer::addVBlankListener)
    //.def ("removeVBlankListener", &ComponentPeer::removeVBlankListener)
        .def ("getPlatformScaleFactor", &ComponentPeer::getPlatformScaleFactor)
        .def ("setHasChangedSinceSaved", &ComponentPeer::setHasChangedSinceSaved)
    //.def ("setAppStyle", &ComponentPeer::setAppStyle)
    //.def ("getAppStyle", &ComponentPeer::getAppStyle)
    ;

    // ============================================================================================ juce::Component

    py::class_<Component, PyComponent<>> classComponent (m, "Component");

    py::enum_<Component::FocusChangeType> (classComponent, "FocusChangeType")
        .value ("focusChangedByMouseClick", Component::FocusChangeType::focusChangedByMouseClick)
        .value ("focusChangedByTabKey", Component::FocusChangeType::focusChangedByTabKey)
        .value ("focusChangedDirectly", Component::FocusChangeType::focusChangedDirectly)
        .export_values();

    py::enum_<Component::FocusContainerType> (classComponent, "FocusContainerType")
        .value ("none", Component::FocusContainerType::none)
        .value ("focusContainer", Component::FocusContainerType::focusContainer)
        .value ("keyboardFocusContainer", Component::FocusContainerType::keyboardFocusContainer);

    py::enum_<Component::FocusChangeDirection> (classComponent, "FocusChangeDirection")
        .value ("unknown", Component::FocusChangeDirection::unknown)
        .value ("forward", Component::FocusChangeDirection::forward)
        .value ("backward", Component::FocusChangeDirection::backward);

    classComponent
        .def (py::init_alias<>())
        .def (py::init_alias<const String&>())
        .def ("setName", &Component::setName)
        .def ("getName", &Component::getName)
        .def ("getComponentID", &Component::getComponentID)
        .def ("setVisible", &Component::setVisible)
        .def ("isVisible", &Component::isVisible)
        .def ("visibilityChanged", &Component::visibilityChanged)
        .def ("isShowing", &Component::isShowing)
        .def ("addToDesktop", [](Component& self, ComponentPeer::StyleFlags windowStyleFlags, PyNativeHandle nativeWindowToAttachTo) { self.addToDesktop (static_cast<int> (windowStyleFlags), nativeWindowToAttachTo); },
            "windowStyleFlags"_a, "nativeWindowToAttachTo"_a = nullptr)
        .def ("addToDesktop", [](Component& self, int windowStyleFlags, PyNativeHandle nativeWindowToAttachTo) { self.addToDesktop (windowStyleFlags, nativeWindowToAttachTo); },
            "windowStyleFlags"_a, "nativeWindowToAttachTo"_a = nullptr)
        .def ("removeFromDesktop", &Component::removeFromDesktop)
        .def ("isOnDesktop", &Component::isOnDesktop)
        .def ("getDesktopScaleFactor", &Component::getDesktopScaleFactor)
        .def ("getPeer", &Component::getPeer, py::return_value_policy::reference)
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
        .def ("getInterceptsMouseClicks", [](const Component& self) { bool a, b; self.getInterceptsMouseClicks (a, b); return py::make_tuple(a, b); })
        .def ("contains", py::overload_cast<Point<float>> (&Component::contains))
        .def ("reallyContains", py::overload_cast<Point<float>, bool> (&Component::reallyContains))
        .def ("getComponentAt", py::overload_cast<int, int> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("getComponentAt", py::overload_cast<Point<int>> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("getComponentAt", py::overload_cast<Point<float>> (&Component::getComponentAt), py::return_value_policy::reference)
        .def ("repaint", py::overload_cast<> (&Component::repaint))
        .def ("repaint", py::overload_cast<int, int, int, int> (&Component::repaint))
        .def ("repaint", py::overload_cast<Rectangle<int>> (&Component::repaint))
        .def ("setBufferedToImage", &Component::setBufferedToImage)
        .def ("createComponentSnapshot", &Component::createComponentSnapshot, "areaToGrab"_a, "clipImageToComponentBounds"_a = true, "scaleFactor"_a = 1.0f)
        .def ("paintEntireComponent", &Component::paintEntireComponent)
        .def ("setPaintingIsUnclipped", &Component::setPaintingIsUnclipped)
        .def ("isPaintingUnclipped", &Component::isPaintingUnclipped)
    //.def ("setComponentEffect", &Component::setComponentEffect)
    //.def ("getComponentEffect", &Component::getComponentEffect)
        .def ("getLookAndFeel", &Component::getLookAndFeel, py::return_value_policy::reference)
        .def ("setLookAndFeel", &Component::setLookAndFeel)
        .def ("lookAndFeelChanged", &Component::lookAndFeelChanged)
        .def ("sendLookAndFeelChange", &Component::sendLookAndFeelChange)
        .def ("setOpaque", &Component::setOpaque)
        .def ("isOpaque", &Component::isOpaque)
        .def ("setBroughtToFrontOnMouseClick", &Component::setBroughtToFrontOnMouseClick)
        .def ("isBroughtToFrontOnMouseClick", &Component::isBroughtToFrontOnMouseClick)
        .def ("setExplicitFocusOrder", &Component::setExplicitFocusOrder)
        .def ("getExplicitFocusOrder", &Component::getExplicitFocusOrder)
        .def ("setFocusContainerType", &Component::setFocusContainerType)
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
        .def ("createFocusTraverser", &Component::createFocusTraverser)
        .def ("createKeyboardFocusTraverser", &Component::createKeyboardFocusTraverser)
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
        .def ("addKeyListener", &Component::addKeyListener)
        .def ("removeKeyListener", &Component::removeKeyListener)
        .def ("keyPressed", &Component::keyPressed)
        .def ("keyStateChanged", &Component::keyStateChanged)
        .def ("modifierKeysChanged", &Component::modifierKeysChanged)
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
        .def ("addComponentListener", &Component::addComponentListener)
        .def ("removeComponentListener", &Component::removeComponentListener)
        .def ("postCommandMessage", &Component::postCommandMessage)
        .def ("handleCommandMessage", &Component::handleCommandMessage)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runModalLoop", &Component::runModalLoop, py::call_guard<py::gil_scoped_release>())
#endif
        .def ("enterModalState", [](Component& self, bool takeKeyboardFocus, py::function callback, bool deleteWhenDismissed)
        {
            self.enterModalState (takeKeyboardFocus, new PyModalComponentManagerCallbackCallable (std::move (callback)), deleteWhenDismissed);
        }, "takeKeyboardFocus"_a = true, "callback"_a = py::none(), "deleteWhenDismissed"_a = false)
        .def ("exitModalState", &Component::exitModalState)
        .def ("isCurrentlyModal", &Component::isCurrentlyModal)
        .def_static ("getNumCurrentlyModalComponents", &Component::getNumCurrentlyModalComponents)
        .def_static ("getCurrentlyModalComponent", &Component::getCurrentlyModalComponent)
        .def ("isCurrentlyBlockedByAnotherModalComponent", &Component::isCurrentlyBlockedByAnotherModalComponent)
        .def ("canModalEventBeSentToComponent", &Component::canModalEventBeSentToComponent)
        .def ("inputAttemptWhenModal", &Component::inputAttemptWhenModal)
        .def ("getProperties", py::overload_cast<> (&Component::getProperties))
        .def ("getProperties", py::overload_cast<> (&Component::getProperties, py::const_))
        .def ("findColour", &Component::findColour, "colourID"_a, "inheritFromParent"_a = false)
        .def ("setColour", &Component::setColour)
        .def ("removeColour", &Component::removeColour)
        .def ("isColourSpecified", &Component::isColourSpecified)
        .def ("copyAllExplicitColoursTo", [](const Component& self, Component* target) { self.copyAllExplicitColoursTo (*target); })
        .def ("colourChanged", &Component::colourChanged)
        .def ("getWindowHandle", [](const Component& self) { return PyNativeHandle (self.getWindowHandle()); })
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
        .def ("releaseOwnership", [](py::object self) { return self.release(); })
        .def ("typeName", [](const juce::Component* self)
        {
            return Helpers::pythonizeClassName (typeid (*self).name());
        })
    ;

    // ============================================================================================ juce::Drawable

    py::class_<Drawable, Component, PyDrawable<>> classDrawable (m, "Drawable");

    classDrawable
        .def (py::init<>())
        .def ("draw", &Drawable::draw, "g"_a, "opacity"_a, "transform"_a = AffineTransform())
        .def ("drawAt", &Drawable::drawAt)
        .def ("drawWithin", &Drawable::drawWithin)
        .def ("setOriginWithOriginalSize", &Drawable::setOriginWithOriginalSize)
        .def ("setTransformToFit", &Drawable::setTransformToFit)
        .def ("getParent", &Drawable::getParent, py::return_value_policy::reference)
    //.def ("setClipPath", &Drawable::setClipPath)
        .def_static ("createFromImageData", [](py::buffer data)
        {
            auto info = data.request();
            return Drawable::createFromImageData (info.ptr, static_cast<size_t> (info.size)).release();
        })
        .def_static ("createFromImageDataStream", &Drawable::createFromImageDataStream)
        .def_static ("createFromImageFile", &Drawable::createFromImageFile)
        .def_static ("createFromSVG", &Drawable::createFromSVG)
        .def_static ("createFromSVGFile", &Drawable::createFromSVGFile)
        .def_static ("parseSVGPath", &Drawable::parseSVGPath)
        .def ("getDrawableBounds", &Drawable::getDrawableBounds)
        .def ("replaceColour", &Drawable::replaceColour)
        .def ("setDrawableTransform", &Drawable::setDrawableTransform)
    ;

    py::class_<DrawableComposite, Drawable, PyDrawableComposite<>> classDrawableComposite (m, "DrawableComposite");

    classDrawableComposite
        .def (py::init<>())
        .def (py::init<const PyDrawableComposite<>&>())
        .def ("setBoundingBox", py::overload_cast<Parallelogram<float>> (&DrawableComposite::setBoundingBox))
        .def ("setBoundingBox", py::overload_cast<Rectangle<float>> (&DrawableComposite::setBoundingBox))
        .def ("getBoundingBox", &DrawableComposite::getBoundingBox)
        .def ("resetBoundingBoxToContentArea", &DrawableComposite::resetBoundingBoxToContentArea)
        .def ("getContentArea", &DrawableComposite::getContentArea)
        .def ("setContentArea", &DrawableComposite::setContentArea)
        .def ("resetContentAreaAndBoundingBoxToFitChildren", &DrawableComposite::resetContentAreaAndBoundingBoxToFitChildren)
    ;

    py::class_<DrawableImage, Drawable, PyDrawable<DrawableImage>> classDrawableImage (m, "DrawableImage");

    classDrawableImage
        .def (py::init<>())
        .def (py::init<const Image&>())
        .def (py::init<const PyDrawable<DrawableImage>&>())
        .def ("setImage", &DrawableImage::setImage)
        .def ("getImage", &DrawableImage::getImage)
        .def ("setOpacity", &DrawableImage::setOpacity)
        .def ("getOpacity", &DrawableImage::getOpacity)
        .def ("setOverlayColour", &DrawableImage::setOverlayColour)
        .def ("getOverlayColour", &DrawableImage::getOverlayColour)
        .def ("setBoundingBox", py::overload_cast<Parallelogram<float>> (&DrawableImage::setBoundingBox))
        .def ("setBoundingBox", py::overload_cast<Rectangle<float>> (&DrawableImage::setBoundingBox))
        .def ("getBoundingBox", &DrawableImage::getBoundingBox)
    ;

    py::class_<DrawablePath, Drawable, PyDrawablePath<>> classDrawablePath (m, "DrawablePath");

    classDrawablePath
        .def (py::init<>())
        .def (py::init<const PyDrawablePath<>&>())
        .def ("setPath", py::overload_cast<const Path&> (&DrawablePath::setPath))
        .def ("getPath", &DrawablePath::getPath)
        .def ("getStrokePath", &DrawablePath::getStrokePath)
    ;

    py::class_<DrawableRectangle, Drawable, PyDrawable<DrawableRectangle>> classDrawableRectangle (m, "DrawableRectangle");

    classDrawableRectangle
        .def (py::init<>())
        .def (py::init<const PyDrawable<DrawableRectangle>&>())
        .def ("setRectangle", &DrawableRectangle::setRectangle)
        .def ("getRectangle", &DrawableRectangle::getRectangle)
        .def ("getCornerSize", &DrawableRectangle::getCornerSize)
        .def ("setCornerSize", &DrawableRectangle::setCornerSize)
    ;

    py::class_<DrawableShape, Drawable, PyDrawableShape<>> classDrawableShape (m, "DrawableShape");

    classDrawableShape
        .def (py::init<>())
        .def (py::init<const PyDrawableShape<>&>())
        .def ("setFill", &DrawableShape::setFill)
        .def ("getFill", &DrawableShape::getFill)
        .def ("setStrokeFill", &DrawableShape::setStrokeFill)
        .def ("getStrokeFill", &DrawableShape::getStrokeFill)
        .def ("setStrokeType", &DrawableShape::setStrokeType)
        .def ("setStrokeThickness", &DrawableShape::setStrokeThickness)
        .def ("getStrokeType", &DrawableShape::getStrokeType)
        .def ("setDashLengths", &DrawableShape::setDashLengths)
        .def ("getDashLengths", &DrawableShape::getDashLengths)
    ;

    py::class_<DrawableText, Drawable, PyDrawableText<>> classDrawableText (m, "DrawableText");

    classDrawableText
        .def (py::init<>())
        .def (py::init<const PyDrawableText<>&>())
        .def ("setText", &DrawableText::setText)
        .def ("getText", &DrawableText::getText)
        .def ("setColour", &DrawableText::setColour)
        .def ("getColour", &DrawableText::getColour)
        .def ("setFont", &DrawableText::setFont)
        .def ("getFont", &DrawableText::getFont)
        .def ("setJustification", &DrawableText::setJustification)
        .def ("setJustification", [](DrawableText& self, Justification::Flags flags) { self.setJustification (flags); })
        .def ("setJustification", [](DrawableText& self, int flags) { self.setJustification (Justification (flags)); })
        .def ("getJustification", &DrawableText::getJustification)
        .def ("getBoundingBox", &DrawableText::getBoundingBox)
        .def ("setBoundingBox", &DrawableText::setBoundingBox)
        .def ("getFontHeight", &DrawableText::getFontHeight)
        .def ("setFontHeight", &DrawableText::setFontHeight)
        .def ("getFontHorizontalScale", &DrawableText::getFontHorizontalScale)
        .def ("setFontHorizontalScale", &DrawableText::setFontHorizontalScale)
    ;

    // ============================================================================================ juce::Button

    py::class_<Button, Component, PyButton<>> classButton (m, "Button");

    Helpers::makeArithmeticEnum<Button::ConnectedEdgeFlags> (classButton, "ConnectedEdgeFlags")
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
        .def (py::init<>())
        .def ("buttonClicked", &Button::Listener::buttonClicked)
        .def ("buttonStateChanged", &Button::Listener::buttonStateChanged)
    ;

    classButton
        .def (py::init<const String&>())
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
        .def ("setConnectedEdges", [](Button& self, Button::ConnectedEdgeFlags flags) { self.setConnectedEdges (static_cast<int> (flags)); })
        .def ("getConnectedEdgeFlags", &Button::getConnectedEdgeFlags)
        .def ("isConnectedOnLeft", &Button::isConnectedOnLeft)
        .def ("isConnectedOnRight", &Button::isConnectedOnRight)
        .def ("isConnectedOnTop", &Button::isConnectedOnTop)
        .def ("isConnectedOnBottom", &Button::isConnectedOnBottom)
        .def ("setState", &Button::setState)
        .def ("getState", &Button::getState)
    ;

    // ============================================================================================ juce::ArrowButton

    py::class_<ArrowButton, Button, PyButton<ArrowButton>> classArrowButton (m, "ArrowButton");

    classArrowButton
        .def (py::init<const String&, float, Colour>())
    ;

    // ============================================================================================ juce::DrawableButton

    py::class_<DrawableButton, Button, PyDrawableButton<>> classDrawableButton (m, "DrawableButton");

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

    classDrawableButton.attr ("textColourId") = py::int_ (static_cast<int> (DrawableButton::textColourId));
    classDrawableButton.attr ("textColourOnId") = py::int_ (static_cast<int> (DrawableButton::textColourOnId));
    classDrawableButton.attr ("backgroundColourId") = py::int_ (static_cast<int> (DrawableButton::backgroundColourId));
    classDrawableButton.attr ("backgroundOnColourId") = py::int_ (static_cast<int> (DrawableButton::backgroundOnColourId));

    // ============================================================================================ juce::HyperlinkButton

    py::class_<HyperlinkButton, Button, PyButton<HyperlinkButton>> classHyperlinkButton (m, "HyperlinkButton");

    classHyperlinkButton
        .def (py::init<>())
        .def (py::init<const String&, const URL&>())
        .def ("setFont", &HyperlinkButton::setFont)
        .def ("setURL", &HyperlinkButton::setURL)
        .def ("getURL", &HyperlinkButton::getURL)
        .def ("changeWidthToFitText", &HyperlinkButton::changeWidthToFitText)
        .def ("setJustificationType", &HyperlinkButton::setJustificationType)
        .def ("setJustificationType", [](HyperlinkButton& self, Justification::Flags flags) { self.setJustificationType (flags); })
        .def ("setJustificationType", [](HyperlinkButton& self, int flags) { self.setJustificationType (Justification (flags)); })
        .def ("getJustificationType", &HyperlinkButton::getJustificationType)
    //.def ("createAccessibilityHandler", &ToggleButton::createAccessibilityHandler)
    ;

    classHyperlinkButton.attr ("textColourId") = py::int_ (static_cast<int> (HyperlinkButton::textColourId));

    // ============================================================================================ juce::ImageButton

    py::class_<ImageButton, Button, PyButton<ImageButton>> classImageButton (m, "ImageButton");

    classImageButton
        .def (py::init<const String&>(), "name"_a = String())
        .def ("setImages", &ImageButton::setImages)
        .def ("getNormalImage", &ImageButton::getNormalImage)
        .def ("getOverImage", &ImageButton::getOverImage)
        .def ("getDownImage", &ImageButton::getDownImage)
    ;

    // ============================================================================================ juce::ShapeButton

    py::class_<ShapeButton, Button, PyButton<ShapeButton>> classShapeButton (m, "ShapeButton");

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

    py::class_<TextButton, Button, PyButton<TextButton>> classTextButton (m, "TextButton");

    classTextButton
        .def (py::init<>())
        .def (py::init<const String&>())
        .def (py::init<const String&, const String&>())
        .def ("changeWidthToFitText", py::overload_cast<> (&TextButton::changeWidthToFitText))
        .def ("changeWidthToFitText", py::overload_cast<int> (&TextButton::changeWidthToFitText))
        .def ("getBestWidthForHeight", &TextButton::getBestWidthForHeight)
    ;

    classTextButton.attr ("buttonColourId") = py::int_ (static_cast<int> (TextButton::buttonColourId));
    classTextButton.attr ("buttonOnColourId") = py::int_ (static_cast<int> (TextButton::buttonOnColourId));
    classTextButton.attr ("textColourOffId") = py::int_ (static_cast<int> (TextButton::textColourOffId));
    classTextButton.attr ("textColourOnId") = py::int_ (static_cast<int> (TextButton::textColourOnId));

    // ============================================================================================ juce::ToggleButton

    py::class_<ToggleButton, Button, PyButton<ToggleButton>> classToggleButton (m, "ToggleButton");

    classToggleButton
        .def (py::init<>())
        .def (py::init<const String&>())
        .def ("changeWidthToFitText", &ToggleButton::changeWidthToFitText)
    //.def ("createAccessibilityHandler", &ToggleButton::createAccessibilityHandler)
    ;

    classToggleButton.attr ("textColourId") = py::int_ (static_cast<int> (ToggleButton::textColourId));
    classToggleButton.attr ("tickColourId") = py::int_ (static_cast<int> (ToggleButton::tickColourId));
    classToggleButton.attr ("tickDisabledColourId") = py::int_ (static_cast<int> (ToggleButton::tickDisabledColourId));

    // ============================================================================================ juce::ToolbarItemFactory

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

    py::class_<Toolbar, Component, PyComponent<Toolbar>> classToolbar (m, "Toolbar");

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

    classToolbar.attr ("backgroundColourId") = py::int_ (static_cast<int> (Toolbar::backgroundColourId));
    classToolbar.attr ("separatorColourId") = py::int_ (static_cast<int> (Toolbar::separatorColourId));
    classToolbar.attr ("buttonMouseOverBackgroundColourId") = py::int_ (static_cast<int> (Toolbar::buttonMouseOverBackgroundColourId));
    classToolbar.attr ("buttonMouseDownBackgroundColourId") = py::int_ (static_cast<int> (Toolbar::buttonMouseDownBackgroundColourId));
    classToolbar.attr ("labelTextColourId") = py::int_ (static_cast<int> (Toolbar::labelTextColourId));
    classToolbar.attr ("editingModeOutlineColourId") = py::int_ (static_cast<int> (Toolbar::editingModeOutlineColourId));

    // ============================================================================================ juce::ToolbarItemComponent

    py::class_<ToolbarItemComponent, Button, PyToolbarItemComponent<>> classToolbarItemComponent (m, "ToolbarItemComponent");

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

    // ============================================================================================ juce::MenuBarModel

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

    // ============================================================================================ juce::Label

    py::class_<Label, Component, PyLabel<>> classLabel (m, "Label");
    py::class_<Label::Listener, PyLabelListener> classLabelListener (classLabel, "Listener");

    classLabelListener
        .def (py::init<>())
        .def ("labelTextChanged", &Label::Listener::labelTextChanged)
        .def ("editorShown", &Label::Listener::editorShown)
        .def ("editorHidden", &Label::Listener::editorHidden)
    ;

    classLabel
        .def (py::init<const String&, const String&>(), "componentName"_a = String(), "labelText"_a = String())
        .def ("setText", &Label::setText)
        .def ("getText", &Label::getText, "returnActiveEditorContents"_a = false)
        .def ("getTextValue", &Label::getTextValue, py::return_value_policy::reference_internal)
        .def ("setFont", &Label::setFont)
        .def ("getFont", &Label::getFont)
        .def ("setJustificationType", &Label::setJustificationType)
        .def ("setJustificationType", [](Label& self, Justification::Flags flags) { self.setJustificationType (flags); })
        .def ("setJustificationType", [](Label& self, int flags) { self.setJustificationType (Justification (flags)); })
        .def ("getJustificationType", &Label::getJustificationType)
        .def ("setBorderSize", &Label::setBorderSize)
        .def ("getBorderSize", &Label::getBorderSize)
        .def ("attachToComponent", &Label::attachToComponent)
        .def ("getAttachedComponent", &Label::getAttachedComponent, py::return_value_policy::reference)
        .def ("isAttachedOnLeft", &Label::isAttachedOnLeft)
        .def ("setMinimumHorizontalScale", &Label::setMinimumHorizontalScale)
        .def ("getMinimumHorizontalScale", &Label::getMinimumHorizontalScale)
    //.def ("setKeyboardType", &Label::setKeyboardType)
        .def ("addListener", &Label::addListener)
        .def ("removeListener", &Label::removeListener)
        .def_readwrite ("onTextChange", &Label::onTextChange)
        .def_readwrite ("onEditorShow", &Label::onEditorShow)
        .def_readwrite ("onEditorHide", &Label::onEditorHide)
        .def ("setEditable", &Label::setEditable, "editOnSingleClick"_a, "editOnDoubleClick"_a = false, "lossOfFocusDiscardsChanges"_a = false)
        .def ("isEditableOnSingleClick", &Label::isEditableOnSingleClick)
        .def ("isEditableOnDoubleClick", &Label::isEditableOnDoubleClick)
        .def ("doesLossOfFocusDiscardChanges", &Label::doesLossOfFocusDiscardChanges)
        .def ("isEditable", &Label::isEditable)
        .def ("showEditor", &Label::showEditor)
        .def ("hideEditor", &Label::hideEditor)
        .def ("isBeingEdited", &Label::isBeingEdited)
    //.def ("getCurrentTextEditor", &Label::getCurrentTextEditor, py::return_value_policy::reference_internal)
    //.def ("createAccessibilityHandler", &Label::createAccessibilityHandler)
    ;

    classLabel.attr ("backgroundColourId") = py::int_ (static_cast<int> (Label::backgroundColourId));
    classLabel.attr ("textColourId") = py::int_ (static_cast<int> (Label::textColourId));
    classLabel.attr ("outlineColourId") = py::int_ (static_cast<int> (Label::outlineColourId));
    classLabel.attr ("backgroundWhenEditingColourId") = py::int_ (static_cast<int> (Label::backgroundWhenEditingColourId));
    classLabel.attr ("textWhenEditingColourId") = py::int_ (static_cast<int> (Label::textWhenEditingColourId));
    classLabel.attr ("outlineWhenEditingColourId") = py::int_ (static_cast<int> (Label::outlineWhenEditingColourId));

    // ============================================================================================ juce::Label

    py::class_<TextEditor, TextInputTarget, Component, PyTextEditor<>> classTextEditor (m, "TextEditor", py::multiple_inheritance());
    py::class_<TextEditor::Listener, PyTextEditorListener> classTextEditorListener (classTextEditor, "Listener");
    py::class_<TextEditor::InputFilter, PyTextEditorInputFilter<>> classTextEditorInputFilter (classTextEditor, "InputFilter");
    py::class_<TextEditor::LengthAndCharacterRestriction, TextEditor::InputFilter, PyTextEditorInputFilter<TextEditor::LengthAndCharacterRestriction>>
        classTextEditorLengthAndCharacterRestriction (classTextEditor, "LengthAndCharacterRestriction");

    classTextEditorListener
        .def (py::init<>())
        .def ("textEditorTextChanged", &TextEditor::Listener::textEditorTextChanged)
        .def ("textEditorReturnKeyPressed", &TextEditor::Listener::textEditorReturnKeyPressed)
        .def ("textEditorEscapeKeyPressed", &TextEditor::Listener::textEditorEscapeKeyPressed)
        .def ("textEditorFocusLost", &TextEditor::Listener::textEditorFocusLost)
    ;

    classTextEditorInputFilter
        .def (py::init<>())
        .def ("filterNewText", &TextEditor::InputFilter::filterNewText)
    ;

    classTextEditorLengthAndCharacterRestriction
        .def (py::init<int, const String&>())
    ;

    classTextEditor
        .def (py::init<const String&, juce_wchar>(), "componentName"_a = String(), "passwordCharacter"_a = L'\0')
    //.def (py::init([](const String& componentName, const String& passwordCharacter)
    //    { return new TextEditor (componentName, passwordCharacter.isNotEmpty() ? passwordCharacter[0] : 0); }),
    //    "componentName"_a = String(), "passwordCharacter"_a = "\0")
        .def ("setMultiLine", &TextEditor::setMultiLine, "shouldBeMultiLine"_a, "shouldWordWrap"_a = true)
        .def ("isMultiLine", &TextEditor::isMultiLine)
        .def ("setReturnKeyStartsNewLine", &TextEditor::setReturnKeyStartsNewLine)
        .def ("getReturnKeyStartsNewLine", &TextEditor::getReturnKeyStartsNewLine)
        .def ("setTabKeyUsedAsCharacter", &TextEditor::setTabKeyUsedAsCharacter)
        .def ("isTabKeyUsedAsCharacter", &TextEditor::isTabKeyUsedAsCharacter)
        .def ("setEscapeAndReturnKeysConsumed", &TextEditor::setEscapeAndReturnKeysConsumed)
        .def ("setReadOnly", &TextEditor::setReadOnly)
        .def ("isReadOnly", &TextEditor::isReadOnly)
        .def ("setCaretVisible", &TextEditor::setCaretVisible)
        .def ("isCaretVisible", &TextEditor::isCaretVisible)
        .def ("setScrollbarsShown", &TextEditor::setScrollbarsShown)
        .def ("areScrollbarsShown", &TextEditor::areScrollbarsShown)
    //.def ("setPasswordCharacter", &TextEditor::setPasswordCharacter)
    //.def ("getPasswordCharacter", &TextEditor::getPasswordCharacter)
        .def ("setPopupMenuEnabled", &TextEditor::setPopupMenuEnabled)
        .def ("isPopupMenuEnabled", &TextEditor::isPopupMenuEnabled)
        .def ("isPopupMenuCurrentlyActive", &TextEditor::isPopupMenuCurrentlyActive)
        .def ("setFont", &TextEditor::setFont)
        .def ("applyFontToAllText", &TextEditor::applyFontToAllText, "newFont"_a, "changeCurrentFont"_a = true)
        .def ("getFont", &TextEditor::getFont)
        .def ("applyColourToAllText", &TextEditor::applyColourToAllText, "newColour"_a, "changeCurrentTextColour"_a = true)
        .def ("setWhitespaceUnderlined", &TextEditor::setWhitespaceUnderlined)
        .def ("isWhitespaceUnderlined", &TextEditor::isWhitespaceUnderlined)
        .def ("setSelectAllWhenFocused", &TextEditor::setSelectAllWhenFocused)
        .def ("setTextToShowWhenEmpty", &TextEditor::setTextToShowWhenEmpty)
        .def ("getTextToShowWhenEmpty", &TextEditor::getTextToShowWhenEmpty)
        .def ("setScrollBarThickness", &TextEditor::setScrollBarThickness)
        .def ("addListener", &TextEditor::addListener)
        .def ("removeListener", &TextEditor::removeListener)
        .def_readwrite ("onTextChange", &TextEditor::onTextChange)
        .def_readwrite ("onReturnKey", &TextEditor::onReturnKey)
        .def_readwrite ("onEscapeKey", &TextEditor::onEscapeKey)
        .def_readwrite ("onFocusLost", &TextEditor::onFocusLost)
        .def ("getText", &TextEditor::getText)
        .def ("getTextInRange", &TextEditor::getTextInRange)
        .def ("isEmpty", &TextEditor::isEmpty)
        .def ("setText", &TextEditor::setText, "newText"_a, "sendTextChangeMessage"_a = true)
        .def ("getTextValue", &TextEditor::getTextValue)
        .def ("insertTextAtCaret", &TextEditor::insertTextAtCaret)
        .def ("clear", &TextEditor::clear)
        .def ("cut", &TextEditor::cut)
        .def ("copy", &TextEditor::copy)
        .def ("paste", &TextEditor::paste)
        .def ("getCaretPosition", &TextEditor::getCaretPosition)
        .def ("setCaretPosition", &TextEditor::setCaretPosition)
        .def ("scrollEditorToPositionCaret", &TextEditor::scrollEditorToPositionCaret)
        .def ("getCaretRectangleForCharIndex", &TextEditor::getCaretRectangleForCharIndex)
        .def ("setHighlightedRegion", &TextEditor::setHighlightedRegion)
        .def ("getHighlightedRegion", &TextEditor::getHighlightedRegion)
        .def ("getHighlightedText", &TextEditor::getHighlightedText)
    //.def ("getTextIndexAt", &TextEditor::getTextIndexAt)
    //.def ("getTextIndexAt", &TextEditor::getTextIndexAt)
        .def ("getCharIndexForPoint", &TextEditor::getCharIndexForPoint)
        .def ("getTotalNumChars", &TextEditor::getTotalNumChars)
        .def ("getTextWidth", &TextEditor::getTextWidth)
        .def ("getTextHeight", &TextEditor::getTextHeight)
        .def ("setIndents", &TextEditor::setIndents)
        .def ("getTopIndent", &TextEditor::getTopIndent)
        .def ("getLeftIndent", &TextEditor::getLeftIndent)
        .def ("setBorder", &TextEditor::setBorder)
        .def ("getBorder", &TextEditor::getBorder)
        .def ("setScrollToShowCursor", &TextEditor::setScrollToShowCursor)
        .def ("setJustification", &TextEditor::setJustification)
        .def ("setJustification", [](TextEditor& self, Justification::Flags flags) { self.setJustification (flags); })
        .def ("setJustification", [](TextEditor& self, int flags) { self.setJustification (Justification (flags)); })
        .def ("getJustificationType", &TextEditor::getJustificationType)
        .def ("setLineSpacing", &TextEditor::setLineSpacing)
        .def ("getLineSpacing", &TextEditor::getLineSpacing)
        .def ("getTextBounds", &TextEditor::getTextBounds)
        .def ("moveCaretToEnd", py::overload_cast<> (&TextEditor::moveCaretToEnd))
        .def ("moveCaretLeft", &TextEditor::moveCaretLeft)
        .def ("moveCaretRight", &TextEditor::moveCaretRight)
        .def ("moveCaretUp", &TextEditor::moveCaretUp)
        .def ("moveCaretDown", &TextEditor::moveCaretDown)
        .def ("pageUp", &TextEditor::pageUp)
        .def ("pageDown", &TextEditor::pageDown)
        .def ("scrollDown", &TextEditor::scrollDown)
        .def ("scrollUp", &TextEditor::scrollUp)
        .def ("moveCaretToTop", &TextEditor::moveCaretToTop)
        .def ("moveCaretToStartOfLine", &TextEditor::moveCaretToStartOfLine)
        .def ("moveCaretToEnd", py::overload_cast<bool> (&TextEditor::moveCaretToEnd))
        .def ("moveCaretToEndOfLine", &TextEditor::moveCaretToEndOfLine)
        .def ("deleteBackwards", &TextEditor::deleteBackwards)
        .def ("deleteForwards", &TextEditor::deleteForwards)
        .def ("copyToClipboard", &TextEditor::copyToClipboard)
        .def ("cutToClipboard", &TextEditor::cutToClipboard)
        .def ("pasteFromClipboard", &TextEditor::pasteFromClipboard)
        .def ("selectAll", &TextEditor::selectAll)
        .def ("undo", &TextEditor::undo)
        .def ("redo", &TextEditor::redo)
    //.def ("addPopupMenuItems", &TextEditor::addPopupMenuItems)
    //.def ("performPopupMenuAction", &TextEditor::performPopupMenuAction)
        .def ("setInputFilter", &TextEditor::setInputFilter)
        .def ("getInputFilter", &TextEditor::getInputFilter, py::return_value_policy::reference)
        .def ("setInputRestrictions", &TextEditor::setInputRestrictions, "maxTextLength"_a, "allowedCharacters"_a = String())
    //.def ("setKeyboardType", &TextEditor::setKeyboardType)
        .def ("setClicksOutsideDismissVirtualKeyboard", &TextEditor::setClicksOutsideDismissVirtualKeyboard)
        .def ("getClicksOutsideDismissVirtualKeyboard", &TextEditor::getClicksOutsideDismissVirtualKeyboard)
    ;

    // ============================================================================================ juce::ListBox

    py::class_<ListBoxModel, PyListBoxModel> classListBoxModel (m, "ListBoxModel");

    classListBoxModel
        .def (py::init<>())
        // TODO
    ;

    py::class_<ListBox, Component, PyListBox<>> classListBox (m, "ListBox");

    classListBox
        .def (py::init<const String&, ListBoxModel*>(), "componentName"_a = String(), "model"_a = nullptr)
        .def ("setModel", &ListBox::setModel)
        .def ("getListBoxModel", &ListBox::getListBoxModel, py::return_value_policy::reference)
        .def ("updateContent", &ListBox::updateContent)
        .def ("setMultipleSelectionEnabled", &ListBox::setMultipleSelectionEnabled)
        .def ("setClickingTogglesRowSelection", &ListBox::setClickingTogglesRowSelection)
        .def ("setRowSelectedOnMouseDown", &ListBox::setRowSelectedOnMouseDown)
        .def ("getRowSelectedOnMouseDown", &ListBox::getRowSelectedOnMouseDown)
        .def ("setMouseMoveSelectsRows", &ListBox::setMouseMoveSelectsRows)
        .def ("selectRow", &ListBox::selectRow, "rowNumber"_a, "dontScrollToShowThisRow"_a = false, "deselectOthersFirst"_a = true)
        .def ("selectRangeOfRows", &ListBox::selectRangeOfRows, "firstRow"_a, "lastRow"_a, "dontScrollToShowThisRange"_a = false)
        .def ("deselectRow", &ListBox::deselectRow)
        .def ("deselectAllRows", &ListBox::deselectAllRows)
        .def ("flipRowSelection", &ListBox::flipRowSelection)
    //.def ("getSelectedRows", &ListBox::getSelectedRows)
    //.def ("setSelectedRows", &ListBox::setSelectedRows, "setOfRowsToBeSelected"_a, "sendNotificationEventToModel"_a = sendNotification)
        .def ("isRowSelected", &ListBox::isRowSelected)
        .def ("getNumSelectedRows", &ListBox::getNumSelectedRows)
        .def ("getSelectedRow", &ListBox::getSelectedRow, "index"_a = 0)
        .def ("getLastRowSelected", &ListBox::getLastRowSelected)
        .def ("selectRowsBasedOnModifierKeys", &ListBox::selectRowsBasedOnModifierKeys)
        .def ("setVerticalPosition", &ListBox::setVerticalPosition)
        .def ("getVerticalPosition", &ListBox::getVerticalPosition)
        .def ("scrollToEnsureRowIsOnscreen", &ListBox::scrollToEnsureRowIsOnscreen)
    //.def ("getVerticalScrollBar", &ListBox::getVerticalScrollBar, py::return_value_policy::reference_internal)
    //.def ("getHorizontalScrollBar", &ListBox::getHorizontalScrollBar, py::return_value_policy::reference_internal)
        .def ("getRowContainingPosition", &ListBox::getRowContainingPosition)
        .def ("getInsertionIndexForPosition", &ListBox::getInsertionIndexForPosition)
        .def ("getRowPosition", &ListBox::getRowPosition)
        .def ("getComponentForRowNumber", &ListBox::getComponentForRowNumber, py::return_value_policy::reference_internal)
        .def ("getRowNumberOfComponent", &ListBox::getRowNumberOfComponent)
        .def ("getVisibleRowWidth", &ListBox::getVisibleRowWidth)
        .def ("setRowHeight", &ListBox::setRowHeight)
        .def ("getRowHeight", &ListBox::getRowHeight)
        .def ("getNumRowsOnScreen", &ListBox::getNumRowsOnScreen)
        .def ("setOutlineThickness", &ListBox::setOutlineThickness)
        .def ("getOutlineThickness", &ListBox::getOutlineThickness)
    //.def ("setHeaderComponent", &ListBox::setHeaderComponent)
    //.def ("getHeaderComponent", &ListBox::getHeaderComponent, py::return_value_policy::reference_internal)
        .def ("setMinimumContentWidth", &ListBox::setMinimumContentWidth)
        .def ("getVisibleContentWidth", &ListBox::getVisibleContentWidth)
        .def ("repaintRow", &ListBox::repaintRow)
    //.def ("createSnapshotOfRows", &ListBox::createSnapshotOfRows)
    //.def ("getViewport", &ListBox::getViewport, py::return_value_policy::reference_internal)
    ;

    classListBox.attr ("backgroundColourId") = py::int_ (static_cast<int> (ListBox::backgroundColourId));
    classListBox.attr ("outlineColourId") = py::int_ (static_cast<int> (ListBox::outlineColourId));
    classListBox.attr ("textColourId") = py::int_ (static_cast<int> (ListBox::textColourId));

    // ============================================================================================ juce::TableHeaderComponent

    py::class_<TableHeaderComponent, Component, PyTableHeaderComponent<>> classTableHeaderComponent (m, "TableHeaderComponent");

    Helpers::makeArithmeticEnum<TableHeaderComponent::ColumnPropertyFlags> (classTableHeaderComponent, "ColumnPropertyFlags")
        .value ("visible", TableHeaderComponent::ColumnPropertyFlags::visible)
        .value ("resizable", TableHeaderComponent::ColumnPropertyFlags::resizable)
        .value ("draggable", TableHeaderComponent::ColumnPropertyFlags::draggable)
        .value ("appearsOnColumnMenu", TableHeaderComponent::ColumnPropertyFlags::appearsOnColumnMenu)
        .value ("sortable", TableHeaderComponent::ColumnPropertyFlags::sortable)
        .value ("sortedForwards", TableHeaderComponent::ColumnPropertyFlags::sortedForwards)
        .value ("sortedBackwards", TableHeaderComponent::ColumnPropertyFlags::sortedBackwards)
        .value ("defaultFlags", TableHeaderComponent::ColumnPropertyFlags::defaultFlags)
        .value ("notResizable", TableHeaderComponent::ColumnPropertyFlags::notResizable)
        .value ("notResizableOrSortable", TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable)
        .value ("notSortable", TableHeaderComponent::ColumnPropertyFlags::notSortable)
        .export_values();

    py::class_<TableHeaderComponent::Listener, PyTableHeaderComponentListener> classTableHeaderComponentListener (classTableHeaderComponent, "Listener");

    classTableHeaderComponentListener
        .def (py::init<>())
        // TODO
    ;

    classTableHeaderComponent
        .def (py::init<>())
        .def ("addColumn", &TableHeaderComponent::addColumn,
            "columnName"_a, "columnId"_a, "width"_a, "minimumWidth"_a = 30, "maximumWidth"_a = -1, "propertyFlags"_a = TableHeaderComponent::defaultFlags, "insertIndex"_a = -1)
        .def ("addColumn", [](TableHeaderComponent& self, const String& columnName, int columnId, int width, int minimumWidth, int maximumWidth, TableHeaderComponent::ColumnPropertyFlags propertyFlags, int insertIndex)
        {
            self.addColumn (columnName, columnId, width, minimumWidth, maximumWidth, static_cast<int> (propertyFlags), insertIndex);
        }, "columnName"_a, "columnId"_a, "width"_a, "minimumWidth"_a = 30, "maximumWidth"_a = -1, "propertyFlags"_a = TableHeaderComponent::defaultFlags, "insertIndex"_a = -1)
        .def ("removeColumn", &TableHeaderComponent::removeColumn)
        .def ("removeAllColumns", &TableHeaderComponent::removeAllColumns)
        .def ("getNumColumns", &TableHeaderComponent::getNumColumns)
        .def ("getColumnName", &TableHeaderComponent::getColumnName)
        .def ("setColumnName", &TableHeaderComponent::setColumnName)
        .def ("moveColumn", &TableHeaderComponent::moveColumn)
        .def ("getColumnWidth", &TableHeaderComponent::getColumnWidth)
        .def ("setColumnWidth", &TableHeaderComponent::setColumnWidth)
        .def ("setColumnVisible", &TableHeaderComponent::setColumnVisible)
        .def ("isColumnVisible", &TableHeaderComponent::isColumnVisible)
        .def ("setSortColumnId", &TableHeaderComponent::setSortColumnId)
        .def ("getSortColumnId", &TableHeaderComponent::getSortColumnId)
        .def ("isSortedForwards", &TableHeaderComponent::isSortedForwards)
        .def ("reSortTable", &TableHeaderComponent::reSortTable)
        .def ("getTotalWidth", &TableHeaderComponent::getTotalWidth)
        .def ("getIndexOfColumnId", &TableHeaderComponent::getIndexOfColumnId)
        .def ("getColumnIdOfIndex", &TableHeaderComponent::getColumnIdOfIndex)
        .def ("getColumnPosition", &TableHeaderComponent::getColumnPosition)
        .def ("getColumnIdAtX", &TableHeaderComponent::getColumnIdAtX)
        .def ("setStretchToFitActive", &TableHeaderComponent::setStretchToFitActive)
        .def ("isStretchToFitActive", &TableHeaderComponent::isStretchToFitActive)
        .def ("resizeAllColumnsToFit", &TableHeaderComponent::resizeAllColumnsToFit)
        .def ("setPopupMenuActive", &TableHeaderComponent::setPopupMenuActive)
        .def ("isPopupMenuActive", &TableHeaderComponent::isPopupMenuActive)
        .def ("toString", &TableHeaderComponent::toString)
        .def ("restoreFromString", &TableHeaderComponent::restoreFromString)
        .def ("addListener", &TableHeaderComponent::addListener)
        .def ("removeListener", &TableHeaderComponent::removeListener)
        .def ("columnClicked", &TableHeaderComponent::columnClicked)
    //.def ("addMenuItems", &TableHeaderComponent::addMenuItems)
        .def ("reactToMenuItem", &TableHeaderComponent::reactToMenuItem)
        .def ("showColumnChooserMenu", &TableHeaderComponent::showColumnChooserMenu)
    ;

    classTableHeaderComponent.attr ("textColourId") = py::int_ (static_cast<int> (TableHeaderComponent::textColourId));
    classTableHeaderComponent.attr ("backgroundColourId") = py::int_ (static_cast<int> (TableHeaderComponent::backgroundColourId));
    classTableHeaderComponent.attr ("outlineColourId") = py::int_ (static_cast<int> (TableHeaderComponent::outlineColourId));
    classTableHeaderComponent.attr ("highlightColourId") = py::int_ (static_cast<int> (TableHeaderComponent::highlightColourId));

    // ============================================================================================ juce::TableListBox

    py::class_<TableListBoxModel, PyTableListBoxModel> classTableListBoxModel (m, "TableListBoxModel");

    classTableListBoxModel
        .def (py::init<>())
        // TODO
    ;

    py::class_<TableListBox, ListBox, PyListBox<TableListBox>> classTableListBox (m, "TableListBox");

    classTableListBox
        .def (py::init<const String&, TableListBoxModel*>(), "componentName"_a = String(), "model"_a = nullptr)
        .def ("setModel", &TableListBox::setModel)
        .def ("getTableListBoxModel", &TableListBox::getTableListBoxModel, py::return_value_policy::reference)
        .def ("getHeader", &TableListBox::getHeader, py::return_value_policy::reference_internal)
        .def ("setHeader", [](TableListBox& self, py::object header)
        {
            if (header.is_none() || ! py::isinstance<TableHeaderComponent> (header))
                py::pybind11_fail ("Invalid specified message type in \"TableListBox::setHeader\"");

            self.setHeader (std::unique_ptr<TableHeaderComponent> (header.release().cast<TableHeaderComponent*>()));
        })
        .def ("setHeaderHeight", &TableListBox::setHeaderHeight)
        .def ("getHeaderHeight", &TableListBox::getHeaderHeight)
        .def ("autoSizeColumn", &TableListBox::autoSizeColumn)
        .def ("autoSizeAllColumns", &TableListBox::autoSizeAllColumns)
        .def ("setAutoSizeMenuOptionShown", &TableListBox::setAutoSizeMenuOptionShown)
        .def ("isAutoSizeMenuOptionShown", &TableListBox::isAutoSizeMenuOptionShown)
        .def ("getCellPosition", &TableListBox::getCellPosition)
        .def ("getCellComponent", &TableListBox::getCellComponent, py::return_value_policy::reference)
        .def ("scrollToEnsureColumnIsOnscreen", &TableListBox::scrollToEnsureColumnIsOnscreen)
    ;

    // ============================================================================================ juce::Slider

    py::class_<Slider, Component, PySlider<>> classSlider (m, "Slider");

    py::enum_<Slider::SliderStyle> (classSlider, "SliderStyle")
        .value ("LinearHorizontal", Slider::SliderStyle::LinearHorizontal)
        .value ("LinearVertical", Slider::SliderStyle::LinearVertical)
        .value ("LinearBar", Slider::SliderStyle::LinearBar)
        .value ("LinearBarVertical", Slider::SliderStyle::LinearBarVertical)
        .value ("Rotary", Slider::SliderStyle::Rotary)
        .value ("RotaryHorizontalDrag", Slider::SliderStyle::RotaryHorizontalDrag)
        .value ("RotaryVerticalDrag", Slider::SliderStyle::RotaryVerticalDrag)
        .value ("RotaryHorizontalVerticalDrag", Slider::SliderStyle::RotaryHorizontalVerticalDrag)
        .value ("IncDecButtons", Slider::SliderStyle::IncDecButtons)
        .value ("TwoValueHorizontal", Slider::SliderStyle::TwoValueHorizontal)
        .value ("TwoValueVertical", Slider::SliderStyle::TwoValueVertical)
        .value ("ThreeValueHorizontal", Slider::SliderStyle::ThreeValueHorizontal)
        .value ("ThreeValueVertical", Slider::SliderStyle::ThreeValueVertical)
        .export_values();

    py::enum_<Slider::TextEntryBoxPosition> (classSlider, "TextEntryBoxPosition")
        .value ("NoTextBox", Slider::TextEntryBoxPosition::NoTextBox)
        .value ("TextBoxLeft", Slider::TextEntryBoxPosition::TextBoxLeft)
        .value ("TextBoxRight", Slider::TextEntryBoxPosition::TextBoxRight)
        .value ("TextBoxAbove", Slider::TextEntryBoxPosition::TextBoxAbove)
        .value ("TextBoxBelow", Slider::TextEntryBoxPosition::TextBoxBelow)
        .export_values();

    py::enum_<Slider::DragMode> (classSlider, "DragMode")
        .value ("notDragging", Slider::DragMode::notDragging)
        .value ("absoluteDrag", Slider::DragMode::absoluteDrag)
        .value ("velocityDrag", Slider::DragMode::velocityDrag)
        .export_values();

    py::enum_<Slider::IncDecButtonMode> (classSlider, "IncDecButtonMode")
        .value ("incDecButtonsNotDraggable", Slider::IncDecButtonMode::incDecButtonsNotDraggable)
        .value ("incDecButtonsDraggable_AutoDirection", Slider::IncDecButtonMode::incDecButtonsDraggable_AutoDirection)
        .value ("incDecButtonsDraggable_Horizontal", Slider::IncDecButtonMode::incDecButtonsDraggable_Horizontal)
        .value ("incDecButtonsDraggable_Vertical", Slider::IncDecButtonMode::incDecButtonsDraggable_Vertical)
        .export_values();

    py::class_<Slider::Listener, PySliderListener> classSliderListener (classSlider, "Listener");

    classSliderListener
        .def (py::init<>())
        .def ("sliderValueChanged", &Slider::Listener::sliderValueChanged)
        .def ("sliderDragStarted", &Slider::Listener::sliderDragStarted)
        .def ("sliderDragEnded", &Slider::Listener::sliderDragEnded)
    ;

    py::class_<Slider::RotaryParameters> classSliderRotaryParameters (classSlider, "RotaryParameters");

    classSliderRotaryParameters
        .def (py::init<>())
        .def_readwrite ("startAngleRadians", &Slider::RotaryParameters::startAngleRadians)
        .def_readwrite ("endAngleRadians", &Slider::RotaryParameters::endAngleRadians)
        .def_readwrite ("stopAtEnd", &Slider::RotaryParameters::stopAtEnd)
    ;

    py::class_<Slider::SliderLayout> classSliderSliderLayout (classSlider, "SliderLayout");

    classSliderSliderLayout
        .def (py::init<>())
        .def_readwrite ("sliderBounds", &Slider::SliderLayout::sliderBounds)
        .def_readwrite ("textBoxBounds", &Slider::SliderLayout::textBoxBounds)
    ;

    classSlider
        .def (py::init<>())
        .def (py::init<const String&>(), "componentName"_a)
        .def (py::init<Slider::SliderStyle, Slider::TextEntryBoxPosition>(), "style"_a, "textBoxPosition"_a)
        .def ("setSliderStyle", &Slider::setSliderStyle, "newStyle"_a)
        .def ("getSliderStyle", &Slider::getSliderStyle)
        .def ("setRotaryParameters", py::overload_cast<Slider::RotaryParameters> (&Slider::setRotaryParameters), "newParameters"_a)
        .def ("setRotaryParameters", py::overload_cast<float, float, bool> (&Slider::setRotaryParameters), "startAngleRadians"_a, "endAngleRadians"_a, "stopAtEnd"_a)
        .def ("getRotaryParameters", &Slider::getRotaryParameters)
        .def ("setMouseDragSensitivity", &Slider::setMouseDragSensitivity, "distanceForFullScaleDrag"_a)
        .def ("getMouseDragSensitivity", &Slider::getMouseDragSensitivity)
        .def ("setVelocityBasedMode", &Slider::setVelocityBasedMode, "isVelocityBased"_a)
        .def ("getVelocityBasedMode", &Slider::getVelocityBasedMode)
        .def ("setVelocityModeParameters", &Slider::setVelocityModeParameters,
            "sensitivity"_a = 1.0, "threshold"_a = 1, "offset"_a = 0.0, "userCanPressKeyToSwapMode"_a = true, "modifiersToSwapModes"_a = ModifierKeys::ctrlAltCommandModifiers)
        .def ("getVelocitySensitivity", &Slider::getVelocitySensitivity)
        .def ("getVelocityThreshold", &Slider::getVelocityThreshold)
        .def ("getVelocityOffset", &Slider::getVelocityOffset)
        .def ("getVelocityModeIsSwappable", &Slider::getVelocityModeIsSwappable)
        .def ("setSkewFactor", &Slider::setSkewFactor, "factor"_a, "symmetricSkew"_a = false)
        .def ("setSkewFactorFromMidPoint", &Slider::setSkewFactorFromMidPoint, "sliderValueToShowAtMidPoint"_a)
        .def ("getSkewFactor", &Slider::getSkewFactor)
        .def ("isSymmetricSkew", &Slider::isSymmetricSkew)
        .def ("setIncDecButtonsMode", &Slider::setIncDecButtonsMode)
        .def ("setTextBoxStyle", &Slider::setTextBoxStyle, "newPosition"_a, "isReadOnly"_a, "textEntryBoxWidth"_a, "textEntryBoxHeight"_a)
        .def ("getTextBoxPosition", &Slider::getTextBoxPosition)
        .def ("getTextBoxWidth", &Slider::getTextBoxWidth)
        .def ("getTextBoxHeight", &Slider::getTextBoxHeight)
        .def ("setTextBoxIsEditable", &Slider::setTextBoxIsEditable, "shouldBeEditable"_a)
        .def ("isTextBoxEditable", &Slider::isTextBoxEditable)
        .def ("showTextBox", &Slider::showTextBox)
        .def ("hideTextBox", &Slider::hideTextBox, "discardCurrentEditorContents"_a)
        .def ("setValue", py::overload_cast<double, NotificationType> (&Slider::setValue), "newValue"_a, "notification"_a = sendNotificationAsync)
        .def ("getValue", &Slider::getValue)
        .def ("getValueObject", &Slider::getValueObject, py::return_value_policy::reference_internal)
        .def ("setRange", py::overload_cast<double, double, double> (&Slider::setRange), "newMinimum"_a, "newMaximum"_a, "newInterval"_a = 0.0)
        .def ("setRange", py::overload_cast<Range<double>, double> (&Slider::setRange), "newRange"_a, "newInterval"_a)
    //.def ("setNormalisableRange", &Slider::setNormalisableRange)
    //.def ("getNormalisableRange", &Slider::getNormalisableRange)
        .def ("getRange", &Slider::getRange)
        .def ("getMaximum", &Slider::getMaximum)
        .def ("getMinimum", &Slider::getMinimum)
        .def ("getInterval", &Slider::getInterval)
        .def ("getMinValue", &Slider::getMinValue)
        .def ("getMinValueObject", &Slider::getMinValueObject, py::return_value_policy::reference_internal)
        .def ("setMinValue", py::overload_cast<double, NotificationType, bool> (&Slider::setMinValue), "newValue"_a, "notification"_a = sendNotificationAsync, "allowNudgingOfOtherValues"_a = false)
        .def ("getMaxValue", &Slider::getMaxValue)
        .def ("getMaxValueObject", &Slider::getMaxValueObject, py::return_value_policy::reference_internal)
        .def ("setMaxValue", py::overload_cast<double, NotificationType, bool> (&Slider::setMaxValue), "newValue"_a, "notification"_a = sendNotificationAsync, "allowNudgingOfOtherValues"_a = false)
        .def ("setMinAndMaxValues", py::overload_cast<double, double, NotificationType> (&Slider::setMinAndMaxValues), "newMinValue"_a, "newMaxValue"_a, "notification"_a = sendNotificationAsync)
        .def ("addListener", &Slider::addListener)
        .def ("removeListener", &Slider::removeListener)
        .def_readwrite ("onValueChange", &Slider::onValueChange)
        .def_readwrite ("onDragStart", &Slider::onDragStart)
        .def_readwrite ("onDragEnd", &Slider::onDragEnd)
        .def_readwrite ("valueFromTextFunction", &Slider::valueFromTextFunction)
        .def_readwrite ("textFromValueFunction", &Slider::textFromValueFunction)
        .def ("setDoubleClickReturnValue", &Slider::setDoubleClickReturnValue)
        .def ("getDoubleClickReturnValue", &Slider::getDoubleClickReturnValue)
        .def ("isDoubleClickReturnEnabled", &Slider::isDoubleClickReturnEnabled)
        .def ("setChangeNotificationOnlyOnRelease", &Slider::setChangeNotificationOnlyOnRelease)
        .def ("setSliderSnapsToMousePosition", &Slider::setSliderSnapsToMousePosition)
        .def ("getSliderSnapsToMousePosition", &Slider::getSliderSnapsToMousePosition)
        .def ("setPopupDisplayEnabled", &Slider::setPopupDisplayEnabled)
        .def ("getCurrentPopupDisplay", &Slider::getCurrentPopupDisplay, py::return_value_policy::reference)
        .def ("setPopupMenuEnabled", &Slider::setPopupMenuEnabled)
        .def ("setScrollWheelEnabled", &Slider::setScrollWheelEnabled)
        .def ("isScrollWheelEnabled", &Slider::isScrollWheelEnabled)
        .def ("getThumbBeingDragged", &Slider::getThumbBeingDragged)
        .def ("startedDragging", &Slider::startedDragging)
        .def ("stoppedDragging", &Slider::stoppedDragging)
        .def ("valueChanged", &Slider::valueChanged)
        .def ("getValueFromText", &Slider::getValueFromText)
        .def ("getTextFromValue", &Slider::getTextFromValue)
        .def ("setTextValueSuffix", &Slider::setTextValueSuffix)
        .def ("getTextValueSuffix", &Slider::getTextValueSuffix)
        .def ("getNumDecimalPlacesToDisplay", &Slider::getNumDecimalPlacesToDisplay)
        .def ("setNumDecimalPlacesToDisplay", &Slider::setNumDecimalPlacesToDisplay)
        .def ("proportionOfLengthToValue", &Slider::proportionOfLengthToValue)
        .def ("valueToProportionOfLength", &Slider::valueToProportionOfLength)
        .def ("getPositionOfValue", &Slider::getPositionOfValue)
        .def ("snapValue", &Slider::snapValue)
        .def ("updateText", &Slider::updateText)
        .def ("isHorizontal", &Slider::isHorizontal)
        .def ("isVertical", &Slider::isVertical)
        .def ("isRotary", &Slider::isRotary)
        .def ("isBar", &Slider::isBar)
        .def ("isTwoValue", &Slider::isTwoValue)
        .def ("isThreeValue", &Slider::isThreeValue)
    ;

    classSlider.attr ("backgroundColourId") = py::int_ (static_cast<int> (Slider::backgroundColourId));
    classSlider.attr ("thumbColourId") = py::int_ (static_cast<int> (Slider::thumbColourId));
    classSlider.attr ("trackColourId") = py::int_ (static_cast<int> (Slider::trackColourId));
    classSlider.attr ("rotarySliderFillColourId") = py::int_ (static_cast<int> (Slider::rotarySliderFillColourId));
    classSlider.attr ("rotarySliderOutlineColourId") = py::int_ (static_cast<int> (Slider::rotarySliderOutlineColourId));
    classSlider.attr ("textBoxTextColourId") = py::int_ (static_cast<int> (Slider::textBoxTextColourId));
    classSlider.attr ("textBoxBackgroundColourId") = py::int_ (static_cast<int> (Slider::textBoxBackgroundColourId));
    classSlider.attr ("textBoxHighlightColourId") = py::int_ (static_cast<int> (Slider::textBoxHighlightColourId));
    classSlider.attr ("textBoxOutlineColourId") = py::int_ (static_cast<int> (Slider::textBoxOutlineColourId));

    // ============================================================================================ juce::TopLevelWindow

    py::class_<TopLevelWindow, Component, PyComponent<TopLevelWindow>> classTopLevelWindow (m, "TopLevelWindow");

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

    py::class_<ResizableWindow, TopLevelWindow, PyComponent<ResizableWindow>> classResizableWindow (m, "ResizableWindow");

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

    classResizableWindow.attr ("backgroundColourId") = py::int_ (static_cast<int> (ResizableWindow::backgroundColourId));

    // ============================================================================================ juce::DocumentWindow

    py::class_<DocumentWindow, ResizableWindow, PyDocumentWindow<>> classDocumentWindow (m, "DocumentWindow");

    Helpers::makeArithmeticEnum<DocumentWindow::TitleBarButtons> (classDocumentWindow, "TitleBarButtons")
        .value ("minimiseButton", DocumentWindow::minimiseButton)
        .value ("maximiseButton", DocumentWindow::maximiseButton)
        .value ("closeButton", DocumentWindow::closeButton)
        .value ("allButtons", DocumentWindow::allButtons)
        .export_values();

    classDocumentWindow
        .def (py::init(
            [](const String& name, Colour backgroundColour, DocumentWindow::TitleBarButtons requiredButtons, bool addToDesktop)
                { return new DocumentWindow (name, backgroundColour, static_cast<int> (requiredButtons), addToDesktop); },
            [](const String& name, Colour backgroundColour, DocumentWindow::TitleBarButtons requiredButtons, bool addToDesktop)
                { return new PyDocumentWindow<> (name, backgroundColour, static_cast<int> (requiredButtons), addToDesktop); }),
            "name"_a, "backgroundColour"_a, "requiredButtons"_a, "addToDesktop"_a = true)
        .def (py::init(
            [](const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop)
                { return new DocumentWindow (name, backgroundColour, requiredButtons, addToDesktop); },
            [](const String& name, Colour backgroundColour, int requiredButtons, bool addToDesktop)
                { return new PyDocumentWindow<> (name, backgroundColour, requiredButtons, addToDesktop); }),
            "name"_a, "backgroundColour"_a, "requiredButtons"_a, "addToDesktop"_a = true)
        .def ("setName", &DocumentWindow::setName)
        .def ("setIcon", &DocumentWindow::setIcon)
        .def ("setTitleBarHeight", &DocumentWindow::setTitleBarHeight)
        .def ("getTitleBarHeight", &DocumentWindow::getTitleBarHeight)
        .def ("setTitleBarButtonsRequired", [](DocumentWindow& self, DocumentWindow::TitleBarButtons requiredButtons, bool positionTitleBarButtonsOnLeft)
        {
            self.setTitleBarButtonsRequired (static_cast<int> (requiredButtons), positionTitleBarButtonsOnLeft);
        })
        .def ("setTitleBarTextCentred", &DocumentWindow::setTitleBarTextCentred)
        .def ("setMenuBar", &DocumentWindow::setMenuBar)
        .def ("getMenuBarComponent", &DocumentWindow::getMenuBarComponent, py::return_value_policy::reference)
        .def ("setMenuBarComponent", &DocumentWindow::setMenuBarComponent)
        .def ("closeButtonPressed", &DocumentWindow::closeButtonPressed)
        .def ("minimiseButtonPressed", &DocumentWindow::minimiseButtonPressed)
        .def ("maximiseButtonPressed", &DocumentWindow::maximiseButtonPressed)
        .def ("getCloseButton", &DocumentWindow::getCloseButton, py::return_value_policy::reference)
        .def ("getMinimiseButton", &DocumentWindow::getMinimiseButton, py::return_value_policy::reference)
        .def ("getMaximiseButton", &DocumentWindow::getMaximiseButton, py::return_value_policy::reference)
    ;

    classDocumentWindow.attr ("textColourId") = py::int_ (static_cast<int> (DocumentWindow::textColourId));

    // ============================================================================================ juce::FileChooser

    py::class_<FileChooser> classFileChooser (m, "FileChooser");

    classFileChooser
        .def (py::init<const String&, const File&, const String&, bool, bool, Component*>(),
            "dialogBoxTitle"_a, "initialFileOrDirectory"_a = File(), "filePatternsAllowed"_a = String(), "useOSNativeDialogBox"_a = true, "treatFilePackagesAsDirectories"_a = false, "parentComponent"_a = static_cast<Component*> (nullptr))
#if JUCE_MODAL_LOOPS_PERMITTED
    //.def ("browseForFileToOpen", &FileChooser::browseForFileToOpen, py::call_guard<py::gil_scoped_release>())
        .def ("browseForFileToOpen", [](FileChooser& self) { return self.browseForFileToOpen(); }, py::call_guard<py::gil_scoped_release>())
    //.def ("browseForMultipleFilesToOpen", &FileChooser::browseForFileToOpen, py::call_guard<py::gil_scoped_release>())
        .def ("browseForMultipleFilesToOpen", [](FileChooser& self) { return self.browseForMultipleFilesToOpen(); }, py::call_guard<py::gil_scoped_release>())
        .def ("browseForFileToSave", &FileChooser::browseForFileToSave, "warnAboutOverwritingExistingFiles"_a, py::call_guard<py::gil_scoped_release>())
        .def ("browseForDirectory", &FileChooser::browseForDirectory, py::call_guard<py::gil_scoped_release>())
    //.def ("browseForMultipleFilesOrDirectories", &FileChooser::browseForMultipleFilesOrDirectories, py::call_guard<py::gil_scoped_release>())
        .def ("browseForMultipleFilesOrDirectories", [](FileChooser& self) { return self.browseForMultipleFilesOrDirectories(); }, py::call_guard<py::gil_scoped_release>())
    //.def ("showDialog", &FileChooser::showDialog, py::call_guard<py::gil_scoped_release>())
        .def ("showDialog", [](FileChooser& self, int flags) { return self.showDialog (flags, nullptr); }, py::call_guard<py::gil_scoped_release>())
#endif
    //.def ("launchAsync", &FileChooser::launchAsync)
        .def ("launchAsync", [](FileChooser& self, int flags, std::function<void (const FileChooser&)> callback) { self.launchAsync (flags, std::move (callback), nullptr); })
        .def ("getResult", &FileChooser::getResult)
        .def ("getResults", &FileChooser::getResults)
        .def ("getURLResult", &FileChooser::getURLResult)
        .def ("getURLResults", &FileChooser::getURLResults)
        .def_static ("isPlatformDialogAvailable", &FileChooser::isPlatformDialogAvailable)
        .def_static ("registerCustomMimeTypeForFileExtension", &FileChooser::registerCustomMimeTypeForFileExtension)
    ;

    // ============================================================================================ juce::FlexBox

    py::class_<FlexBox> classFlexBox (m, "FlexBox");

    py::enum_<FlexBox::Direction> (classFlexBox, "Direction")
        .value ("row", FlexBox::Direction::row)
        .value ("rowReverse", FlexBox::Direction::rowReverse)
        .value ("column", FlexBox::Direction::column)
        .value ("columnReverse", FlexBox::Direction::columnReverse);

    py::enum_<FlexBox::Wrap> (classFlexBox, "Wrap")
        .value ("noWrap", FlexBox::Wrap::noWrap)
        .value ("wrap", FlexBox::Wrap::wrap)
        .value ("wrapReverse", FlexBox::Wrap::wrapReverse);

    py::enum_<FlexBox::AlignContent> (classFlexBox, "AlignContent")
        .value ("stretch", FlexBox::AlignContent::stretch)
        .value ("flexStart", FlexBox::AlignContent::flexStart)
        .value ("flexEnd", FlexBox::AlignContent::flexEnd)
        .value ("center", FlexBox::AlignContent::center)
        .value ("spaceBetween", FlexBox::AlignContent::spaceBetween)
        .value ("spaceAround", FlexBox::AlignContent::spaceAround);

    py::enum_<FlexBox::AlignItems> (classFlexBox, "AlignItems")
        .value ("stretch", FlexBox::AlignItems::stretch)
        .value ("flexStart", FlexBox::AlignItems::flexStart)
        .value ("flexEnd", FlexBox::AlignItems::flexEnd)
        .value ("center", FlexBox::AlignItems::center);

    py::enum_<FlexBox::JustifyContent> (classFlexBox, "JustifyContent")
        .value ("flexStart", FlexBox::JustifyContent::flexStart)
        .value ("flexEnd", FlexBox::JustifyContent::flexEnd)
        .value ("center", FlexBox::JustifyContent::center)
        .value ("spaceBetween", FlexBox::JustifyContent::spaceBetween)
        .value ("spaceAround", FlexBox::JustifyContent::spaceAround);

    py::class_<FlexItem> classFlexItem (m, "FlexItem");

    py::class_<FlexItem::Margin> classFlexItemMargin (classFlexItem, "Margin");

    py::enum_<FlexItem::AlignSelf> (classFlexItem, "AlignSelf")
        .value ("autoAlign", FlexItem::AlignSelf::autoAlign)
        .value ("flexStart", FlexItem::AlignSelf::flexStart)
        .value ("flexEnd", FlexItem::AlignSelf::flexEnd)
        .value ("center", FlexItem::AlignSelf::center)
        .value ("stretch", FlexItem::AlignSelf::stretch);

    classFlexBox
        .def (py::init<>())
        .def (py::init<FlexBox::Direction, FlexBox::Wrap, FlexBox::AlignContent, FlexBox::AlignItems, FlexBox::JustifyContent>())
        .def (py::init<FlexBox::JustifyContent>())
        .def ("performLayout", py::overload_cast<Rectangle<float>> (&FlexBox::performLayout))
        .def ("performLayout", py::overload_cast<Rectangle<int>> (&FlexBox::performLayout))
        .def_readwrite ("flexDirection", &FlexBox::flexDirection)
        .def_readwrite ("flexWrap", &FlexBox::flexWrap)
        .def_readwrite ("alignContent", &FlexBox::alignContent)
        .def_readwrite ("alignItems", &FlexBox::alignItems)
        .def_readwrite ("justifyContent", &FlexBox::justifyContent)
        .def_readwrite ("items", &FlexBox::items)
    ;

    classFlexItemMargin
        .def (py::init<>())
        .def (py::init<float>())
        .def (py::init<float, float, float, float>())
        .def_readwrite ("left", &FlexItem::Margin::left)
        .def_readwrite ("right", &FlexItem::Margin::right)
        .def_readwrite ("top", &FlexItem::Margin::top)
        .def_readwrite ("bottom", &FlexItem::Margin::bottom)
    ;

    classFlexItem
        .def (py::init<>())
        .def (py::init<float, float>())
        .def (py::init<float, float, Component&>())
        .def (py::init<float, float, FlexBox&>())
        .def (py::init<Component&>())
        .def (py::init<FlexBox&>())
        .def_readwrite ("currentBounds", &FlexItem::currentBounds)
        .def_readwrite ("associatedComponent", &FlexItem::associatedComponent, py::return_value_policy::reference)
        .def_readwrite ("associatedFlexBox", &FlexItem::associatedFlexBox, py::return_value_policy::reference)
        .def_readwrite ("order", &FlexItem::order)
        .def_readwrite ("flexGrow", &FlexItem::flexGrow)
        .def_readwrite ("flexShrink", &FlexItem::flexShrink)
        .def_readwrite ("flexBasis", &FlexItem::flexBasis)
        .def_readwrite ("alignSelf", &FlexItem::alignSelf)
    //.def_property_readonly_static ("autoValue", [] { return FlexItem::autoValue; })
    //.def_property_readonly_static ("notAssigned", [] { return FlexItem::notAssigned; })
        .def_readwrite ("width", &FlexItem::width)
        .def_readwrite ("minWidth", &FlexItem::minWidth)
        .def_readwrite ("maxWidth", &FlexItem::maxWidth)
        .def_readwrite ("height", &FlexItem::height)
        .def_readwrite ("minHeight", &FlexItem::minHeight)
        .def_readwrite ("maxHeight", &FlexItem::maxHeight)
        .def_readwrite ("margin", &FlexItem::margin)
        .def ("withFlex", py::overload_cast<float> (&FlexItem::withFlex, py::const_))
        .def ("withFlex", py::overload_cast<float, float> (&FlexItem::withFlex, py::const_))
        .def ("withFlex", py::overload_cast<float, float, float> (&FlexItem::withFlex, py::const_))
        .def ("withWidth", &FlexItem::withWidth)
        .def ("withMinWidth", &FlexItem::withMinWidth)
        .def ("withMaxWidth", &FlexItem::withMaxWidth)
        .def ("withHeight", &FlexItem::withHeight)
        .def ("withMinHeight", &FlexItem::withMinHeight)
        .def ("withMaxHeight", &FlexItem::withMaxHeight)
        .def ("withMargin", &FlexItem::withMargin)
        .def ("withOrder", &FlexItem::withOrder)
        .def ("withAlignSelf", &FlexItem::withAlignSelf)
    ;

    registerArray<Array, FlexItem> (m);
}

} // namespace popsicle::Bindings
