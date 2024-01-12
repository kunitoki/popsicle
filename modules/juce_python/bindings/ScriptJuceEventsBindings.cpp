/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#if __has_include(<juce_events/juce_events.h>)

#include "ScriptJuceGuiBasicsBindings.h"
#include "ScriptJuceEventsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"
#undef JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#undef JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceEventsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::ActionListener

    struct PyActionListener : public ActionListener
    {
        void actionListenerCallback(const String& message) override
        {
            PYBIND11_OVERRIDE_PURE(void, ActionListener, actionListenerCallback, message);
        }
    };

    py::class_<ActionListener, PyActionListener> classActionListener (m, "ActionListener");
    classActionListener
        .def (py::init<>())
        .def ("actionListenerCallback", &ActionListener::actionListenerCallback)
    ;

    // ============================================================================================ juce::ActionBroadcaster

    py::class_<ActionBroadcaster> classActionBroadcaster (m, "ActionBroadcaster");
    classActionBroadcaster
        .def (py::init<>())
        .def ("addActionListener", &ActionBroadcaster::addActionListener)
        .def ("removeActionListener", &ActionBroadcaster::removeActionListener)
        .def ("removeAllActionListeners", &ActionBroadcaster::removeAllActionListeners)
        .def ("sendActionMessage", &ActionBroadcaster::sendActionMessage)
    ;

    // ============================================================================================ juce::AsyncUpdater

    struct PyAsyncUpdater : public AsyncUpdater
    {
        void handleAsyncUpdate() override
        {
            PYBIND11_OVERRIDE_PURE(void, AsyncUpdater, handleAsyncUpdate);
        }
    };

    py::class_<AsyncUpdater, PyAsyncUpdater> classAsyncUpdater (m, "AsyncUpdater");
    classAsyncUpdater
        .def (py::init<>())
        .def ("handleAsyncUpdate", &AsyncUpdater::handleAsyncUpdate)
        .def ("triggerAsyncUpdate", &AsyncUpdater::triggerAsyncUpdate)
        .def ("cancelPendingUpdate", &AsyncUpdater::cancelPendingUpdate)
        .def ("handleUpdateNowIfNeeded", &AsyncUpdater::handleUpdateNowIfNeeded)
        .def ("isUpdatePending", &AsyncUpdater::isUpdatePending)
    ;

    // ============================================================================================ juce::MessageManager

    py::class_<MessageManager> classMessageManager (m, "MessageManager");
    classMessageManager
        .def_static ("getInstance", &MessageManager::getInstance, py::return_value_policy::reference)
        .def_static ("getInstanceWithoutCreating", &MessageManager::getInstanceWithoutCreating, py::return_value_policy::reference)
        .def_static ("deleteInstance", &MessageManager::deleteInstance)
        .def ("runDispatchLoop", &MessageManager::runDispatchLoop)
        .def ("stopDispatchLoop", &MessageManager::stopDispatchLoop)
        .def ("hasStopMessageBeenSent", &MessageManager::hasStopMessageBeenSent)
#if JUCE_MODAL_LOOPS_PERMITTED
        .def ("runDispatchLoopUntil", &MessageManager::runDispatchLoopUntil)
#endif
        .def_static ("callAsync", &MessageManager::callAsync) // TODO - test
        .def ("callFunctionOnMessageThread", &MessageManager::callFunctionOnMessageThread)
        .def ("isThisTheMessageThread", &MessageManager::isThisTheMessageThread)
        .def ("setCurrentThreadAsMessageThread", &MessageManager::setCurrentThreadAsMessageThread)
    //.def ("getCurrentMessageThread", &MessageManager::getCurrentMessageThread)
        .def ("currentThreadHasLockedMessageManager", &MessageManager::currentThreadHasLockedMessageManager)
        .def_static ("existsAndIsLockedByCurrentThread", &MessageManager::existsAndIsLockedByCurrentThread)
        .def_static ("existsAndIsCurrentThread", &MessageManager::existsAndIsCurrentThread)
        .def_static ("broadcastMessage", &MessageManager::broadcastMessage)
        .def ("registerBroadcastListener", &MessageManager::registerBroadcastListener)
        .def ("deregisterBroadcastListener", &MessageManager::deregisterBroadcastListener)
        .def ("deliverBroadcastMessage", &MessageManager::deliverBroadcastMessage)
    ;

    // ============================================================================================ juce::MessageManagerLock

    py::class_<MessageManagerLock> classMessageManagerLock (m, "MessageManagerLock");
    classMessageManagerLock
        .def (py::init<>())
        .def ("lockWasGained", &MessageManagerLock::lockWasGained)
    ;

    // ============================================================================================ juce::Timer

    struct PyTimer : public Timer
    {
        void timerCallback() override
        {
            PYBIND11_OVERRIDE_PURE(void, Timer, timerCallback);
        }
    };

    py::class_<Timer, PyTimer> classTimer (m, "Timer");
    classTimer
        .def (py::init<>())
        .def ("timerCallback", &Timer::timerCallback)
        .def ("startTimer", &Timer::startTimer)
        .def ("startTimerHz", &Timer::startTimerHz)
        .def ("stopTimer", &Timer::stopTimer)
        .def ("isTimerRunning", &Timer::isTimerRunning)
        .def ("getTimerInterval", &Timer::getTimerInterval)
        .def_static ("callAfterDelay", &Timer::callAfterDelay)
        .def_static ("callPendingTimersSynchronously", &Timer::callPendingTimersSynchronously)
    ;
}

} // namespace popsicle::Bindings

#endif
