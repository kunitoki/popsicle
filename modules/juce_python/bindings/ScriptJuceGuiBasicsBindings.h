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

#if !JUCE_MODULE_AVAILABLE_juce_gui_basics
 #error This binding file requires adding the juce_gui_basics module in the project
#else
 #include <juce_gui_basics/juce_gui_basics.h>
#endif

#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#define JUCE_PYTHON_INCLUDE_PYBIND11_IOSTREAM
#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#include "../utilities/PyBind11Includes.h"

#include "ScriptJuceOptionsBindings.h"

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <vector>
#include <utility>

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceGuiBasicsBindings (pybind11::module_& m);

// =================================================================================================

struct PyNativeHandle
{
    explicit PyNativeHandle (void* value) noexcept
        : value (value)
    {
    }

    operator void*() const noexcept
    {
        return value;
    }

    bool operator== (const PyNativeHandle& other) const noexcept
    {
        return value == other.value;
    }

    bool operator!= (const PyNativeHandle& other) const noexcept
    {
        return value != other.value;
    }

private:
    void* value;
};

// =================================================================================================

struct PyJUCEApplication : juce::JUCEApplication
{
    const juce::String getApplicationName() override
    {
        PYBIND11_OVERRIDE_PURE (const juce::String, juce::JUCEApplication, getApplicationName);
    }

    const juce::String getApplicationVersion() override
    {
        PYBIND11_OVERRIDE_PURE (const juce::String, juce::JUCEApplication, getApplicationVersion);
    }

    bool moreThanOneInstanceAllowed() override
    {
        PYBIND11_OVERRIDE (bool, juce::JUCEApplication, moreThanOneInstanceAllowed);
    }

    void initialise (const juce::String& commandLineParameters) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::JUCEApplication, initialise, commandLineParameters);
    }

    void shutdown() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::JUCEApplication, shutdown);
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        PYBIND11_OVERRIDE (void, juce::JUCEApplication, anotherInstanceStarted, commandLine);
    }

    void systemRequestedQuit() override
    {
        PYBIND11_OVERRIDE (void, juce::JUCEApplication, systemRequestedQuit);
    }

    void suspended() override
    {
        PYBIND11_OVERRIDE (void, juce::JUCEApplication, suspended);
    }

    void resumed() override
    {
        PYBIND11_OVERRIDE (void, juce::JUCEApplication, resumed);
    }

    void unhandledException (const std::exception* ex, const juce::String& sourceFilename, int lineNumber) override
    {
        pybind11::gil_scoped_acquire gil;

        if (pybind11::function override_ = pybind11::get_override (static_cast<juce::JUCEApplication*> (this), "unhandledException"); override_)
        {
            //juce::String code;
            //code << "RuntimeError(\"\"\"" << (ex != nullptr ? ex->what() : "unhandled exception") << "\"\"\")";
            //auto pyex = pybind11::eval (code.toRawUTF8());

            //auto pyex = exceptionClass (ex != nullptr ? ex->what() : "unhandled exception");

            auto pyex = pybind11::none();

            override_ (pyex, sourceFilename, lineNumber);

            return;
        }

        if (globalOptions().catchExceptionsAndContinue)
        {
            pybind11::print (ex->what());
            pybind11::module_::import ("traceback").attr ("print_stack")();

            if (const auto* e = dynamic_cast<const pybind11::error_already_set*> (ex))
            {
                if (PyErr_CheckSignals() != 0 || e->matches (PyExc_KeyboardInterrupt))
                    globalOptions().caughtKeyboardInterrupt = true;
            }
        }
        else
        {
            std::terminate();
        }
    }

    void memoryWarningReceived() override
    {
        PYBIND11_OVERRIDE (void, juce::JUCEApplication, memoryWarningReceived);
    }

    bool backButtonPressed() override
    {
        PYBIND11_OVERRIDE (bool, juce::JUCEApplication, backButtonPressed);
    }
};

// =================================================================================================

struct PyKeyListener : juce::KeyListener
{
    using juce::KeyListener::KeyListener;

    bool keyPressed (const juce::KeyPress& key, juce::Component* originatingComponent) override
    {
        PYBIND11_OVERRIDE_PURE (bool, juce::KeyListener, keyPressed, key, originatingComponent);
    }

    bool keyStateChanged (bool isKeyDown, juce::Component* originatingComponent) override
    {
        PYBIND11_OVERRIDE (bool, KeyListener, keyStateChanged, isKeyDown, originatingComponent);
    }
};

// =================================================================================================

template <class Base = juce::MouseListener>
struct PyMouseListener : Base
{
    using Base::Base;

    void mouseMove (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseMove, event);
    }

    void mouseEnter (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseEnter, event);
    }

    void mouseExit (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseExit, event);
    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseDown, event);
    }

    void mouseDrag (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseDrag, event);
    }

    void mouseUp (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseUp, event);
    }

    void mouseDoubleClick (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseDoubleClick, event);
    }

    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseWheelMove, event, wheel);
    }

    void mouseMagnify (const juce::MouseEvent& event, float scaleFactor) override
    {
        PYBIND11_OVERRIDE (void, Base, mouseMagnify, event, scaleFactor);
    }
};

// =================================================================================================

template <class Base = juce::TextInputTarget>
struct PyTextInputTarget : Base
{
    using Base::Base;

    bool isTextInputActive() const override
    {
        PYBIND11_OVERRIDE_PURE (bool, Base, isTextInputActive);
    }

    juce::Range<int> getHighlightedRegion() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Range<int>, Base, getHighlightedRegion);
    }

    void setHighlightedRegion (const juce::Range<int>& newRange) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setHighlightedRegion, newRange);
    }

    void setTemporaryUnderlining (const juce::Array<juce::Range<int>>& underlinedRegions) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, setTemporaryUnderlining, underlinedRegions);
    }

    juce::String getTextInRange (const juce::Range<int>& range) const override
    {
        PYBIND11_OVERRIDE_PURE (juce::String, Base, getTextInRange, range);
    }

    void insertTextAtCaret (const juce::String& textToInsert) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, insertTextAtCaret, textToInsert);
    }

    int getCaretPosition() const override
    {
        PYBIND11_OVERRIDE_PURE (int, Base, getCaretPosition);
    }

    juce::Rectangle<int> getCaretRectangleForCharIndex (int characterIndex) const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Rectangle<int>, Base, getCaretRectangleForCharIndex, characterIndex);
    }

    int getTotalNumChars() const override
    {
        PYBIND11_OVERRIDE_PURE (int, Base, getTotalNumChars);
    }

    int getCharIndexForPoint (juce::Point<int> point) const override
    {
        PYBIND11_OVERRIDE_PURE (int, Base, getCharIndexForPoint, point);
    }

    juce::RectangleList<int> getTextBounds (juce::Range<int> textRange) const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Rectangle<int>, Base, getTextBounds, textRange);
    }

    juce::TextInputTarget::VirtualKeyboardType getKeyboardType() override
    {
        PYBIND11_OVERRIDE (juce::TextInputTarget::VirtualKeyboardType, Base, getKeyboardType);
    }
};

// =================================================================================================

template <class Base = juce::LookAndFeel>
struct PyLookAndFeel : Base
{
    using Base::Base;

    void drawSpinningWaitAnimation(juce::Graphics& g, const juce::Colour& colour, int x, int y, int w, int h) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawSpinningWaitAnimation"))
            {
                override_ (std::addressof (g), colour, x, y, w, h);
                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"LookAndFeel::drawSpinningWaitAnimation\"");
    }

    juce::Path getTickShape (float height) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Path, Base, getTickShape, height);
    }

    juce::Path getCrossShape (float height) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Path, Base, getCrossShape, height);
    }

    std::unique_ptr<juce::DropShadower> createDropShadowerForComponent (juce::Component&) override
    {
        return {}; // TODO
    }

    std::unique_ptr<juce::FocusOutline> createFocusOutlineForComponent (juce::Component&) override
    {
        return {}; // TODO
    }

    juce::MouseCursor getMouseCursorFor (juce::Component& c) override
    {
        PYBIND11_OVERRIDE (juce::MouseCursor, Base, getMouseCursorFor, c);
    }

    void playAlertSound() override
    {
        PYBIND11_OVERRIDE (void, Base, playAlertSound);
    }
};

template <class Base = juce::LookAndFeel_V2>
struct PyLookAndFeel_V2 : PyLookAndFeel<Base>
{
    using PyLookAndFeel<Base>::PyLookAndFeel;

    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawButtonBackground"))
            {
                override_ (std::addressof (g), std::addressof (b), backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        Base::drawButtonBackground (g, b, backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    juce::Font getTextButtonFont (juce::TextButton& button, int buttonHeight) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "getTextButtonFont"))
            {
                return override_ (std::addressof (button), buttonHeight).cast<juce::Font>();
            }
        }

        return Base::getTextButtonFont (button, buttonHeight);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawButtonText"))
            {
                override_ (std::addressof (g), std::addressof (button), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        Base::drawButtonText (g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    int getTextButtonWidthToFitText (juce::TextButton& button, int buttonHeight) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override(static_cast<Base*>(this), "getTextButtonWidthToFitText"))
            {
                return override_ (std::addressof(button), buttonHeight).cast<int>();
            }
        }

        return Base::getTextButtonWidthToFitText(button, buttonHeight);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawToggleButton"))
            {
                override_ (std::addressof (g), std::addressof (button), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        Base::drawToggleButton (g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    void changeToggleButtonWidthToFitText (juce::ToggleButton& button) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "changeToggleButtonWidthToFitText"); override_)
            {
                override_ (std::addressof (button));
                return;
            }
        }

        Base::changeToggleButtonWidthToFitText (button);
    }

    void drawTickBox (juce::Graphics& g, juce::Component& component,
                      float x, float y, float w, float h,
                      bool ticked, bool isEnabled,
                      bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawTickBox"); override_)
            {
                override_ (std::addressof (g), std::addressof (component), x, y, w, h, ticked, isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        Base::drawTickBox (g, component, x, y, w, h, ticked, isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    void drawDrawableButton (juce::Graphics& g, juce::DrawableButton& button,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawDrawableButton"); override_)
            {
                override_ (std::addressof (g), std::addressof (button), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        Base::drawDrawableButton (g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }

    juce::AlertWindow* createAlertWindow (const juce::String& title, const juce::String& message,
                                          const juce::String& button1, const juce::String& button2,
                                          const juce::String& button3, juce::MessageBoxIconType iconType,
                                          int numButtons, juce::Component* associatedComponent) override
    {
        PYBIND11_OVERRIDE (juce::AlertWindow*, Base, createAlertWindow, title, message, button1, button2, button3, iconType, numButtons, associatedComponent);
    }

    void drawAlertBox (juce::Graphics& g, juce::AlertWindow& alertWindow, const juce::Rectangle<int>& textArea, juce::TextLayout& textLayout) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "drawAlertBox"); override_)
            {
                override_ (std::addressof (g), std::addressof (alertWindow), textArea, std::addressof (textLayout));
                return;
            }
        }

        Base::drawAlertBox (g, alertWindow, textArea, textLayout);
    }

    int getAlertBoxWindowFlags() override
    {
        PYBIND11_OVERRIDE (int, Base, getAlertBoxWindowFlags);
    }

    juce::Array<int> getWidthsForTextButtons (juce::AlertWindow& alertWindow, const juce::Array<juce::TextButton*>& buttons) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "getWidthsForTextButtons"); override_)
            {
                pybind11::list list (buttons.size());
                for (int i = 0; i < buttons.size(); ++i)
                    list[static_cast<size_t> (i)] = buttons.getUnchecked (i);

                return override_ (std::addressof (alertWindow), list).cast<juce::Array<int>>();
            }
        }

        return Base::getWidthsForTextButtons (alertWindow, buttons);
    }

    int getAlertWindowButtonHeight() override
    {
        PYBIND11_OVERRIDE (int, Base, getAlertWindowButtonHeight);
    }

    juce::Font getAlertWindowTitleFont() override
    {
        PYBIND11_OVERRIDE (juce::Font, Base, getAlertWindowTitleFont);
    }

    juce::Font getAlertWindowMessageFont() override
    {
        PYBIND11_OVERRIDE (juce::Font, Base, getAlertWindowMessageFont);
    }

    juce::Font getAlertWindowFont() override
    {
        PYBIND11_OVERRIDE (juce::Font, Base, getAlertWindowFont);
    }
};

// =================================================================================================

template <class Base = juce::ComponentTraverser>
struct PyComponentTraverser : Base
{
    using Base::Base;

    juce::Component* getDefaultComponent (juce::Component* parentComponent) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Component*, Base, getDefaultComponent, parentComponent);
    }

    juce::Component* getNextComponent (juce::Component* current) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Component*, Base, getNextComponent, current);
    }

    juce::Component* getPreviousComponent (juce::Component* current) override
    {
        PYBIND11_OVERRIDE_PURE (juce::Component*, Base, getPreviousComponent, current);
    }

    std::vector<juce::Component*> getAllComponents (juce::Component* parentComponent) override
    {
        PYBIND11_OVERRIDE_PURE (std::vector<juce::Component*>, Base, getAllComponents, parentComponent);
    }
};

template <class Base = juce::FocusTraverser>
struct PyFocusTraverser : PyComponentTraverser<Base>
{
    using PyComponentTraverser<Base>::PyComponentTraverser;
};

// =================================================================================================

struct PyComponentListener : juce::ComponentListener
{
    using juce::ComponentListener::ComponentListener;

    void componentMovedOrResized (juce::Component& component, bool wasMoved, bool wasResized) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentMovedOrResized, component, wasMoved, wasResized);
    }

    void componentBroughtToFront (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentBroughtToFront, component);
    }

    void componentVisibilityChanged (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentVisibilityChanged, component);
    }

    void componentChildrenChanged (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentChildrenChanged, component);
    }

    void componentParentHierarchyChanged (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentParentHierarchyChanged, component);
    }

    void componentNameChanged (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentNameChanged, component);
    }

    void componentBeingDeleted (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentBeingDeleted, component);
    }

    void componentEnablementChanged (juce::Component& component) override
    {
        PYBIND11_OVERRIDE (void, juce::ComponentListener, componentEnablementChanged, component);
    }
};

// =================================================================================================

struct PyModalComponentManagerCallback : juce::ModalComponentManager::Callback
{
    using juce::ModalComponentManager::Callback::Callback;

    void modalStateFinished (int returnValue) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::ModalComponentManager::Callback, modalStateFinished, returnValue);
    }
};

struct PyModalComponentManagerCallbackCallable : juce::ModalComponentManager::Callback
{
    explicit PyModalComponentManagerCallbackCallable (pybind11::function f)
        : fn (std::move (f))
    {
    }

    void modalStateFinished (int result) override
    {
        if (fn)
            fn (result);
    }

private:
    pybind11::function fn;
};

// =================================================================================================

template <class Base = juce::Component>
struct PyComponent : PyMouseListener<Base>
{
    using PyMouseListener<Base>::PyMouseListener;

    void setName (const juce::String& newName) override
    {
        PYBIND11_OVERRIDE (void, Base, setName, newName);
    }

    void setVisible (bool shouldBeVisible) override
    {
        PYBIND11_OVERRIDE (void, Base, setVisible, shouldBeVisible);
    }

    void visibilityChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, visibilityChanged);
    }

    void userTriedToCloseWindow() override
    {
        PYBIND11_OVERRIDE (void, Base, userTriedToCloseWindow);
    }

    void minimisationStateChanged(bool isNowMinimised) override
    {
        PYBIND11_OVERRIDE (void, Base, minimisationStateChanged, isNowMinimised);
    }

    float getDesktopScaleFactor() const override
    {
        PYBIND11_OVERRIDE (float, Base, getDesktopScaleFactor);
    }

    void parentHierarchyChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, parentHierarchyChanged);
    }

    void childrenChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, childrenChanged);
    }

    bool hitTest (int x, int y) override
    {
        PYBIND11_OVERRIDE (bool, Base, hitTest, x, y);
    }

    void lookAndFeelChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, lookAndFeelChanged);
    }

    void enablementChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, enablementChanged);
    }

    void alphaChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, alphaChanged);
    }

    void paint (juce::Graphics& g) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<const Base*> (this), "paint"); override_)
            {
                override_ (std::addressof (g));
                return;
            }
        }

        return Base::paint (g);
    }

    void paintOverChildren (juce::Graphics& g) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<const Base*> (this), "paintOverChildren"); override_)
            {
                override_ (std::addressof (g));
                return;
            }
        }

        return Base::paintOverChildren (g);
    }

    bool keyPressed (const juce::KeyPress& key) override
    {
        PYBIND11_OVERRIDE (bool, Base, keyPressed, key);
    }

    bool keyStateChanged (bool isDown) override
    {
        PYBIND11_OVERRIDE (bool, Base, keyStateChanged, isDown);
    }

    void modifierKeysChanged (const juce::ModifierKeys& modifiers) override
    {
        PYBIND11_OVERRIDE (void, Base, modifierKeysChanged, modifiers);
    }

    void focusGained (juce::Component::FocusChangeType cause) override
    {
        PYBIND11_OVERRIDE (void, Base, focusGained, cause);
    }

    void focusGainedWithDirection (juce::Component::FocusChangeType cause, juce::Component::FocusChangeDirection direction) override
    {
        PYBIND11_OVERRIDE (void, Base, focusGainedWithDirection, cause, direction);
    }

    void focusLost (juce::Component::FocusChangeType cause) override
    {
        PYBIND11_OVERRIDE (void, Base, focusLost, cause);
    }

    void focusOfChildComponentChanged (juce::Component::FocusChangeType cause) override
    {
        PYBIND11_OVERRIDE (void, Base, focusOfChildComponentChanged, cause);
    }

    void resized () override
    {
        PYBIND11_OVERRIDE (void, Base, resized);
    }

    void moved () override
    {
        PYBIND11_OVERRIDE (void, Base, moved);
    }

    void childBoundsChanged (juce::Component* child) override
    {
        PYBIND11_OVERRIDE (void, Base, childBoundsChanged, child);
    }

    void parentSizeChanged () override
    {
        PYBIND11_OVERRIDE (void, Base, parentSizeChanged);
    }

    void broughtToFront () override
    {
        PYBIND11_OVERRIDE (void, Base, broughtToFront);
    }

    void handleCommandMessage (int commandId) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "handleCommandMessage"); override_)
            {
                override_ (commandId);
                return;
            }
        }

        if constexpr (! std::is_same_v<Base, juce::TextEditor>)
            Base::handleCommandMessage (commandId);
    }

    bool canModalEventBeSentToComponent (const juce::Component* targetComponent) override
    {
        PYBIND11_OVERRIDE (bool, Base, canModalEventBeSentToComponent, targetComponent);
    }

    void inputAttemptWhenModal () override
    {
        PYBIND11_OVERRIDE (void, Base, inputAttemptWhenModal);
    }

    void colourChanged () override
    {
        PYBIND11_OVERRIDE (void, Base, colourChanged);
    }
};

// =================================================================================================

template <class Base = juce::Drawable>
struct PyDrawable : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    std::unique_ptr<juce::Drawable> createCopy() const override
    {
        /*
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<const Base*> (this), "createCopy"); override_)
            {
                pybind11::object result = override_();

                return std::unique_ptr<Drawable> (result.release().cast<Base*>());
            }
        }

        pybind11::pybind11_fail("Tried to call pure virtual function \"Drawable::createCopy\"");
        */

        return nullptr;
    }

    juce::Path getOutlineAsPath() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Path, Base, getOutlineAsPath);
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        PYBIND11_OVERRIDE_PURE (juce::Rectangle<float>, Base, getDrawableBounds);
    }

    bool replaceColour (juce::Colour originalColour, juce::Colour replacementColour) override
    {
        PYBIND11_OVERRIDE (bool, Base, replaceColour, originalColour, replacementColour);
    }
};

template <class Base = juce::DrawableComposite>
struct PyDrawableComposite : PyDrawable<Base>
{
    using PyDrawable<Base>::PyDrawable;

    std::unique_ptr<juce::Drawable> createCopy() const override { return nullptr; }

    juce::Path getOutlineAsPath() const override
    {
        PYBIND11_OVERRIDE (juce::Path, Base, getOutlineAsPath);
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        PYBIND11_OVERRIDE (juce::Rectangle<float>, Base, getDrawableBounds);
    }
};

template <class Base = juce::DrawableImage>
struct PyDrawableImage : PyDrawable<Base>
{
    using PyDrawable<Base>::PyDrawable;

    std::unique_ptr<juce::Drawable> createCopy() const override { return nullptr; }

    juce::Path getOutlineAsPath() const override
    {
        PYBIND11_OVERRIDE (juce::Path, Base, getOutlineAsPath);
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        PYBIND11_OVERRIDE (juce::Rectangle<float>, Base, getDrawableBounds);
    }
};

template <class Base = juce::DrawablePath>
struct PyDrawablePath : PyDrawable<Base>
{
    using PyDrawable<Base>::PyDrawable;

    std::unique_ptr<juce::Drawable> createCopy() const override { return nullptr; }
};

template <class Base = juce::DrawableShape>
struct PyDrawableShape : PyDrawable<Base>
{
    using PyDrawable<Base>::PyDrawable;

    std::unique_ptr<juce::Drawable> createCopy() const override { return nullptr; }

    juce::Path getOutlineAsPath() const override
    {
        PYBIND11_OVERRIDE (juce::Path, Base, getOutlineAsPath);
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        PYBIND11_OVERRIDE (juce::Rectangle<float>, Base, getDrawableBounds);
    }

    bool replaceColour (juce::Colour originalColour, juce::Colour replacementColour) override
    {
        PYBIND11_OVERRIDE (bool, Base, replaceColour, originalColour, replacementColour);
    }
};

template <class Base = juce::DrawableText>
struct PyDrawableText : PyDrawable<Base>
{
    using PyDrawable<Base>::PyDrawable;

    std::unique_ptr<juce::Drawable> createCopy() const override { return nullptr; }

    juce::Path getOutlineAsPath() const override
    {
        PYBIND11_OVERRIDE (juce::Path, Base, getOutlineAsPath);
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        PYBIND11_OVERRIDE (juce::Rectangle<float>, Base, getDrawableBounds);
    }

    bool replaceColour (juce::Colour originalColour, juce::Colour replacementColour) override
    {
        PYBIND11_OVERRIDE (bool, Base, replaceColour, originalColour, replacementColour);
    }
};

// =================================================================================================

template <class Base = juce::Button>
struct PyButton : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    explicit PyButton (const juce::String& name)
        : PyComponent<Base> (name)
    {
    }

    void triggerClick() override
    {
        PYBIND11_OVERRIDE (void, Base, triggerClick);
    }

    void clicked() override
    {
        PYBIND11_OVERRIDE (void, Base, clicked);
    }

    void clicked (const juce::ModifierKeys& modifiers) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "clickedWithModifiers"); override_)
            {
                override_ (modifiers);
                return;
            }
        }

        if constexpr (! std::is_same_v<Base, juce::HyperlinkButton>)
            Base::clicked (modifiers);
    }

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "paintButton"); override_)
            {
                override_ (std::addressof (g), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"Button::paintButton\"");
    }

    void buttonStateChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, buttonStateChanged);
    }
};

struct PyButtonListener : juce::Button::Listener
{
    using juce::Button::Listener::Listener;

    void buttonClicked (juce::Button* button) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Button::Listener, buttonClicked, button);
    }

    void buttonStateChanged (juce::Button* button) override
    {
        PYBIND11_OVERRIDE (void, juce::Button::Listener, buttonStateChanged, button);
    }
};

template <class Base = juce::DrawableButton>
struct PyDrawableButton : PyButton<Base>
{
    using PyButton<Base>::PyButton;

    juce::Rectangle<float> getImageBounds() const override
    {
        PYBIND11_OVERRIDE (juce::Rectangle<float>, Base, getImageBounds);
    }
};

// =================================================================================================

template <class Base = juce::Label>
struct PyLabel : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    juce::TextEditor* createEditorComponent() override
    {
        PYBIND11_OVERRIDE (juce::TextEditor*, Base, createEditorComponent);
    }

    void textWasEdited() override
    {
        PYBIND11_OVERRIDE (void, Base, textWasEdited);
    }

    void textWasChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, textWasChanged);
    }

    void editorShown (juce::TextEditor* e) override
    {
        PYBIND11_OVERRIDE (void, Base, editorShown, e);
    }

    void editorAboutToBeHidden (juce::TextEditor* e) override
    {
        PYBIND11_OVERRIDE (void, Base, editorAboutToBeHidden, e);
    }
};

struct PyLabelListener : juce::Label::Listener
{
    using juce::Label::Listener::Listener;

    void labelTextChanged (juce::Label* labelThatHasChanged) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Label::Listener, labelTextChanged, labelThatHasChanged);
    }

    void editorShown (juce::Label* label, juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE (void, juce::Label::Listener, editorShown, label, e);
    }

    void editorHidden (juce::Label* label, juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE (void, juce::Label::Listener, editorHidden, label, e);
    }
};

// =================================================================================================

template <class Base = juce::TextEditor>
struct PyTextEditor : /*PyTextInputTarget<Base>,*/ PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;
    //using PyTextInputTarget<Base>::PyTextInputTarget;

    void addPopupMenuItems (juce::PopupMenu& menuToAddTo, const juce::MouseEvent* mouseClickEvent) override
    {
        PYBIND11_OVERRIDE (void, Base, addPopupMenuItems, menuToAddTo, mouseClickEvent);
    }

    void performPopupMenuAction (int menuItemID) override
    {
        PYBIND11_OVERRIDE (void, Base, performPopupMenuAction, menuItemID);
    }
};

struct PyTextEditorListener : juce::TextEditor::Listener
{
    using juce::TextEditor::Listener::Listener;

    void textEditorTextChanged (juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TextEditor::Listener, textEditorTextChanged, e);
    }

    void textEditorReturnKeyPressed (juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TextEditor::Listener, textEditorReturnKeyPressed, e);
    }

    void textEditorEscapeKeyPressed (juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TextEditor::Listener, textEditorEscapeKeyPressed, e);
    }

    void textEditorFocusLost (juce::TextEditor& e) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TextEditor::Listener, textEditorFocusLost, e);
    }
};

template <class Base = juce::TextEditor::InputFilter>
struct PyTextEditorInputFilter : Base
{
    using Base::Base;

    juce::String filterNewText (juce::TextEditor& e, const juce::String& newInput) override
    {
        PYBIND11_OVERRIDE_PURE (juce::String, Base, filterNewText, e, newInput);
    }
};

// =================================================================================================

struct PyListBoxModel : juce::ListBoxModel
{
    using juce::ListBoxModel::ListBoxModel;

    int getNumRows() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::ListBoxModel, getNumRows);
    }

    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::ListBoxModel*> (this), "paintListBoxItem"); override_)
            {
                override_ (rowNumber, std::addressof (g), width, height, rowIsSelected);
                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"ListBoxModel::paintListBoxItem\"");
    }

    juce::Component* refreshComponentForRow (int rowNumber, bool isRowSelected, juce::Component* existingComponentToUpdate) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::ListBoxModel*> (this), "refreshComponentForRow"); override_)
            {
                auto result = override_ (rowNumber, isRowSelected, existingComponentToUpdate);
                if (result.is_none())
                    return nullptr;

                if (! pybind11::isinstance<juce::Component> (result))
                    pybind11::pybind11_fail ("Method \"ListBoxModel::refreshComponentForRow\" returned something else than a \"Component\"");

                return result.release().cast<juce::Component*>();
            }
        }

        return juce::ListBoxModel::refreshComponentForRow (rowNumber, isRowSelected, existingComponentToUpdate);
    }

    juce::String getNameForRow (int rowNumber) override
    {
        PYBIND11_OVERRIDE (juce::String, juce::ListBoxModel, getNameForRow, rowNumber);
    }

    void listBoxItemClicked (int row, const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, listBoxItemClicked, row, event);
    }

    void listBoxItemDoubleClicked (int row, const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, listBoxItemDoubleClicked, row, event);
    }

    void backgroundClicked (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, backgroundClicked, event);
    }

    void selectedRowsChanged (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, selectedRowsChanged, lastRowSelected);
    }

    void deleteKeyPressed (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, deleteKeyPressed, lastRowSelected);
    }

    void returnKeyPressed (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, returnKeyPressed, lastRowSelected);
    }

    void listWasScrolled() override
    {
        PYBIND11_OVERRIDE (void, juce::ListBoxModel, listWasScrolled);
    }

    juce::var getDragSourceDescription (const juce::SparseSet<int>& rowsToDescribe) override
    {
        PYBIND11_OVERRIDE (juce::var, juce::ListBoxModel, getDragSourceDescription, rowsToDescribe);
    }

    bool mayDragToExternalWindows() const override
    {
        PYBIND11_OVERRIDE (bool, juce::ListBoxModel, mayDragToExternalWindows);
    }

    juce::String getTooltipForRow (int row) override
    {
        PYBIND11_OVERRIDE (juce::String, juce::ListBoxModel, getTooltipForRow, row);
    }

    juce::MouseCursor getMouseCursorForRow (int row) override
    {
        PYBIND11_OVERRIDE (juce::MouseCursor, juce::ListBoxModel, getMouseCursorForRow, row);
    }
};

template <class Base = juce::ListBox>
struct PyListBox : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    juce::ScaledImage createSnapshotOfRows (const juce::SparseSet<int>& rows, int& x, int& y) override
    {
        juce::ignoreUnused (rows, x, y);

        return {}; // TODO
    }
};

// =================================================================================================

template <class Base = juce::TableHeaderComponent>
struct PyTableHeaderComponent : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    void columnClicked (int columnId, const juce::ModifierKeys& mods) override
    {
        PYBIND11_OVERRIDE (void, juce::TableHeaderComponent, columnClicked, columnId, mods);
    }

    void addMenuItems (juce::PopupMenu& menu, int columnIdClicked) override
    {
        PYBIND11_OVERRIDE (void, juce::TableHeaderComponent, addMenuItems, menu, columnIdClicked);
    }

    void reactToMenuItem (int menuReturnId, int columnIdClicked) override
    {
        PYBIND11_OVERRIDE (void, juce::TableHeaderComponent, reactToMenuItem, menuReturnId, columnIdClicked);
    }

    void showColumnChooserMenu (int columnIdClicked) override
    {
        PYBIND11_OVERRIDE (void, juce::TableHeaderComponent, showColumnChooserMenu, columnIdClicked);
    }
};

struct PyTableHeaderComponentListener : juce::TableHeaderComponent::Listener
{
    using juce::TableHeaderComponent::Listener::Listener;

    void tableColumnsChanged (juce::TableHeaderComponent* tableHeader) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TableHeaderComponent::Listener, tableColumnsChanged, tableHeader);
    }

    void tableColumnsResized (juce::TableHeaderComponent* tableHeader) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TableHeaderComponent::Listener, tableColumnsResized, tableHeader);
    }

    void tableSortOrderChanged (juce::TableHeaderComponent* tableHeader) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::TableHeaderComponent::Listener, tableSortOrderChanged, tableHeader);
    }

    void tableColumnDraggingChanged (juce::TableHeaderComponent* tableHeader, int columnIdNowBeingDragged) override
    {
        PYBIND11_OVERRIDE (void, juce::TableHeaderComponent::Listener, tableColumnDraggingChanged, tableHeader, columnIdNowBeingDragged);
    }
};

// =================================================================================================

struct PyTableListBoxModel : juce::TableListBoxModel
{
    using juce::TableListBoxModel::TableListBoxModel;

    int getNumRows() override
    {
        PYBIND11_OVERRIDE_PURE (int, juce::TableListBoxModel, getNumRows);
    }

    void paintRowBackground (juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::TableListBoxModel*> (this), "paintRowBackground"); override_)
            {
                override_ (std::addressof (g), rowNumber, width, height, rowIsSelected);
                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"TableListBoxModel::paintListBoxItem\"");
    }

    void paintCell (juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::TableListBoxModel*> (this), "paintCell"); override_)
            {
                override_ (std::addressof (g), rowNumber, columnId, width, height, rowIsSelected);
                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"TableListBoxModel::paintCell\"");
    }

    juce::Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::TableListBoxModel*> (this), "refreshComponentForCell"); override_)
            {
                auto result = override_ (rowNumber, columnId, isRowSelected, existingComponentToUpdate);
                if (result.is_none())
                    return nullptr;

                if (! pybind11::isinstance<juce::Component> (result))
                    pybind11::pybind11_fail ("Method \"TableListBoxModel::refreshComponentForRow\" returned something else than a \"Component\"");

                return result.release().cast<juce::Component*>();
            }
        }

        return juce::TableListBoxModel::refreshComponentForCell (rowNumber, columnId, isRowSelected, existingComponentToUpdate);
    }

    void cellClicked (int rowNumber, int columnId, const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, cellClicked, rowNumber, columnId, event);
    }

    void cellDoubleClicked (int rowNumber, int columnId, const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, cellDoubleClicked, rowNumber, columnId, event);
    }

    void backgroundClicked (const juce::MouseEvent& event) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, backgroundClicked, event);
    }

    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, sortOrderChanged, newSortColumnId, isForwards);
    }

    int getColumnAutoSizeWidth (int columnId) override
    {
        PYBIND11_OVERRIDE (int, juce::TableListBoxModel, getColumnAutoSizeWidth, columnId);
    }

    juce::String getCellTooltip (int rowNumber, int columnId) override
    {
        PYBIND11_OVERRIDE ( juce::String, juce::TableListBoxModel, getCellTooltip, rowNumber, columnId);
    }

    void selectedRowsChanged (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, selectedRowsChanged, lastRowSelected);
    }

    void deleteKeyPressed (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, deleteKeyPressed, lastRowSelected);
    }

    void returnKeyPressed (int lastRowSelected) override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, returnKeyPressed, lastRowSelected);
    }

    void listWasScrolled() override
    {
        PYBIND11_OVERRIDE (void, juce::TableListBoxModel, listWasScrolled);
    }

    juce::var getDragSourceDescription (const juce::SparseSet<int>& currentlySelectedRows) override
    {
        PYBIND11_OVERRIDE (juce::var, juce::TableListBoxModel, getDragSourceDescription, currentlySelectedRows);
    }

    bool mayDragToExternalWindows() const override
    {
        PYBIND11_OVERRIDE (bool, juce::TableListBoxModel, mayDragToExternalWindows);
    }
};

// =================================================================================================

struct PyToolbarItemFactory : juce::ToolbarItemFactory
{
    using juce::ToolbarItemFactory::ToolbarItemFactory;

    void getAllToolbarItemIds (juce::Array<int>& ids) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::ToolbarItemFactory*> (this), "getAllToolbarItemIds"); override_)
            {
                auto result = override_ ();

                ids.addArray (result.cast<juce::Array<int>>());
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"ToolbarItemFactory::getAllToolbarItemIds\"");
    }

    void getDefaultItemSet (juce::Array<int>& ids) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<juce::ToolbarItemFactory*> (this), "getDefaultItemSet"); override_)
            {
                auto result = override_ ();

                ids.addArray (result.cast<juce::Array<int>>());
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"ToolbarItemFactory::getDefaultItemSet\"");
    }

    juce::ToolbarItemComponent* createItem (int itemId) override
    {
        PYBIND11_OVERRIDE_PURE (juce::ToolbarItemComponent*, juce::ToolbarItemFactory, createItem, itemId);
    }
};

template <class Base = juce::ToolbarItemComponent>
struct PyToolbarItemComponent : PyButton<Base>
{
    using PyButton<Base>::PyButton;

    void setStyle (const juce::Toolbar::ToolbarItemStyle& newStyle) override
    {
        PYBIND11_OVERRIDE (void, Base, setStyle, newStyle);
    }

    bool getToolbarItemSizes (int toolbarThickness, bool isToolbarVertical, int& preferredSize, int& minSize, int& maxSize) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "getToolbarItemSizes"); override_)
            {
                auto result = override_ (toolbarThickness, isToolbarVertical, std::ref (preferredSize), std::ref (minSize), std::ref (maxSize));

                return pybind11::detail::cast_safe<bool> (std::move (result));
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"ToolbarItemComponent::getToolbarItemSizes\"");
    }

    void paintButtonArea (juce::Graphics& g, int width, int height, bool isMouseOver, bool isMouseDown) override
    {
        {
            pybind11::gil_scoped_acquire gil;

            if (pybind11::function override_ = pybind11::get_override (static_cast<Base*> (this), "paintButtonArea"); override_)
            {
                override_ (std::addressof (g), width, height, isMouseOver, isMouseDown);

                return;
            }
        }

        pybind11::pybind11_fail ("Tried to call pure virtual function \"ToolbarItemComponent::paintButtonArea\"");
    }

    void contentAreaChanged (const juce::Rectangle<int>& newBounds) override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, contentAreaChanged, newBounds);
    }
};

// ============================================================================================

struct PyMenuBarModel : juce::MenuBarModel
{
    juce::StringArray getMenuBarNames() override
    {
        PYBIND11_OVERRIDE_PURE (juce::StringArray, juce::MenuBarModel, getMenuBarNames);
    }

    juce::PopupMenu getMenuForIndex (int topLevelMenuIndex, const juce::String& menuName) override
    {
        PYBIND11_OVERRIDE_PURE (juce::PopupMenu, juce::MenuBarModel, getMenuForIndex, topLevelMenuIndex, menuName);
    }

    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MenuBarModel, menuItemSelected, menuItemID, topLevelMenuIndex);
    }

    void menuBarActivated (bool isActive) override
    {
        PYBIND11_OVERRIDE (void, juce::MenuBarModel, menuBarActivated, isActive);
    }
};

struct PyMenuBarModelListener : juce::MenuBarModel::Listener
{
    void menuBarItemsChanged (juce::MenuBarModel* menuBarModel) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MenuBarModel::Listener, menuBarItemsChanged, menuBarModel);
    }

    void menuCommandInvoked (juce::MenuBarModel* menuBarModel, const juce::ApplicationCommandTarget::InvocationInfo& info) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MenuBarModel::Listener, menuCommandInvoked, menuBarModel, info);
    }

    void menuBarActivated (juce::MenuBarModel* menuBarModel, bool isActive) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MenuBarModel::Listener, menuBarActivated, menuBarModel, isActive);
    }
};

// =================================================================================================

template <class Base = juce::Slider>
struct PySlider : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    void startedDragging() override
    {
        PYBIND11_OVERRIDE (void, Base, startedDragging);
    }

    void stoppedDragging() override
    {
        PYBIND11_OVERRIDE (void, Base, stoppedDragging);
    }

    void valueChanged() override
    {
        PYBIND11_OVERRIDE (void, Base, valueChanged);
    }

    double getValueFromText (const juce::String& text) override
    {
        PYBIND11_OVERRIDE (double, Base, getValueFromText, text);
    }

    juce::String getTextFromValue (double value) override
    {
        PYBIND11_OVERRIDE (juce::String, Base, getTextFromValue, value);
    }

    double proportionOfLengthToValue (double proportion) override
    {
        PYBIND11_OVERRIDE (double, Base, proportionOfLengthToValue, proportion);
    }

    double valueToProportionOfLength (double value) override
    {
        PYBIND11_OVERRIDE (double, Base, valueToProportionOfLength, value);
    }

    double snapValue (double attemptedValue, juce::Slider::DragMode dragMode) override
    {
        PYBIND11_OVERRIDE (double, Base, snapValue, attemptedValue, dragMode);
    }
};

struct PySliderListener : juce::Slider::Listener
{
    using juce::Slider::Listener::Listener;

    void sliderValueChanged (juce::Slider* slider) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Slider::Listener, sliderValueChanged, slider);
    }

    void sliderDragStarted (juce::Slider* slider) override
    {
        PYBIND11_OVERRIDE (void, juce::Slider::Listener, sliderDragStarted, slider);
    }

    void sliderDragEnded (juce::Slider* slider) override
    {
        PYBIND11_OVERRIDE (void, juce::Slider::Listener, sliderDragEnded, slider);
    }
};

// ============================================================================================

template <class Base = juce::DocumentWindow>
struct PyDocumentWindow : PyComponent<Base>
{
    using PyComponent<Base>::PyComponent;

    void closeButtonPressed() override
    {
        PYBIND11_OVERRIDE (void, Base, closeButtonPressed);
    }

    void minimiseButtonPressed() override
    {
        PYBIND11_OVERRIDE (void, Base, minimiseButtonPressed);
    }

    void maximiseButtonPressed() override
    {
        PYBIND11_OVERRIDE (void, Base, maximiseButtonPressed);
    }
};

} // namespace popsicle::Bindings
