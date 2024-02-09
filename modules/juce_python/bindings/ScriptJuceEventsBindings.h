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

#if !JUCE_MODULE_AVAILABLE_juce_events
 #error This binding file requires adding the juce_events module in the project
#else
 #include <juce_events/juce_events.h>
#endif

#include "../utilities/PyBind11Includes.h"

#include <variant>

namespace popsicle::Bindings {

// =================================================================================================

void registerJuceEventsBindings (pybind11::module_& m);

// =================================================================================================

struct PyActionListener : public juce::ActionListener
{
    void actionListenerCallback (const juce::String& message) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::ActionListener, actionListenerCallback, message);
    }
};

// =================================================================================================

struct PyAsyncUpdater : public juce::AsyncUpdater
{
    void handleAsyncUpdate() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::AsyncUpdater, handleAsyncUpdate);
    }
};

// =================================================================================================

struct PyChangeListener : public juce::ChangeListener
{
    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::ChangeListener, changeListenerCallback, source);
    }
};

// =================================================================================================

template <class Base = juce::MessageManager::MessageBase>
struct PyMessageBase : public Base
{
    using Base::Base;

    void messageCallback() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, messageCallback);
    }
};

// =================================================================================================

template <class Base = juce::CallbackMessage>
struct PyCallbackMessage : public PyMessageBase<Base>
{
    using PyMessageBase<Base>::PyMessageBase;

    void messageCallback() override
    {
        PYBIND11_OVERRIDE_PURE (void, Base, messageCallback);
    }
};

// =================================================================================================

struct PyMessageListener : public juce::MessageListener
{
    void handleMessage (const juce::Message& message) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MessageListener, handleMessage, message);
    }
};

// =================================================================================================

struct PyMessageManagerLock
{
    explicit PyMessageManagerLock (juce::Thread* thread)
        : thread (thread)
    {
    }

    explicit PyMessageManagerLock (juce::ThreadPoolJob* threadPoolJob)
        : threadPoolJob (threadPoolJob)
    {
    }

    juce::Thread* thread = nullptr;
    juce::ThreadPoolJob* threadPoolJob = nullptr;
    std::variant<std::monostate, juce::MessageManagerLock> state;
};

// =================================================================================================

struct PyTimer : public juce::Timer
{
    using juce::Timer::Timer;

    void timerCallback() override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::Timer, timerCallback);
    }
};

// =================================================================================================

struct PyMultiTimer : public juce::MultiTimer
{
    using juce::MultiTimer::MultiTimer;

    void timerCallback (int timerID) override
    {
        PYBIND11_OVERRIDE_PURE (void, juce::MultiTimer, timerCallback, timerID);
    }
};

} // namespace popsicle::Bindings
