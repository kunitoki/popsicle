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

#if !JUCE_MODULE_AVAILABLE_juce_events
 #error This binding file requires adding the juce_events module in the project
#endif

#include <juce_events/juce_events.h>

#include "ScriptJuceEventsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"

#include <functional>
#include <string_view>
#include <typeinfo>
#include <tuple>

//=================================================================================================

namespace juce {

#if JUCE_MAC
 extern void initialiseNSApplication();
#endif

} // namespace juce

//=================================================================================================

namespace popsicle::Bindings {

void registerJuceEventsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;
    using namespace py::literals;

    // ============================================================================================ juce::NotificationType

#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER
    static int numScopedInitInstances = 0;

    if (numScopedInitInstances++ == 0)
    {
        initialiseJuce_GUI();

        JUCEApplicationBase::createInstance = +[]() -> JUCEApplicationBase* { return nullptr; };

#if JUCE_MAC
        initialiseNSApplication();
#endif
    }

    auto atexit = py::module_::import ("atexit");
    atexit.attr ("register") (py::cpp_function([]
    {
        if (--numScopedInitInstances == 0)
            shutdownJuce_GUI();
    }));
#endif

    // ============================================================================================ juce::NotificationType

    py::enum_<NotificationType> (m, "ButtonState")
        .value ("dontSendNotification", NotificationType::dontSendNotification)
        .value ("sendNotification", NotificationType::sendNotification)
        .value ("sendNotificationSync", NotificationType::sendNotificationSync)
        .value ("sendNotificationAsync", NotificationType::sendNotificationAsync)
        .export_values();

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

    // ============================================================================================ juce::LockingAsyncUpdater

    py::class_<LockingAsyncUpdater> classLockingAsyncUpdater (m, "LockingAsyncUpdater");

    classLockingAsyncUpdater
        //.def (py::init<std::function<void()>>())
        .def ("triggerAsyncUpdate", &LockingAsyncUpdater::triggerAsyncUpdate)
        .def ("cancelPendingUpdate", &LockingAsyncUpdater::cancelPendingUpdate)
        .def ("handleUpdateNowIfNeeded", &LockingAsyncUpdater::handleUpdateNowIfNeeded)
        .def ("isUpdatePending", &LockingAsyncUpdater::isUpdatePending)
    ;

    // ============================================================================================ juce::AsyncUpdater

    struct PyChangeListener : public ChangeListener
    {
        void changeListenerCallback(ChangeBroadcaster* source) override
        {
            PYBIND11_OVERRIDE_PURE(void, ChangeListener, changeListenerCallback, source);
        }
    };

    py::class_<ChangeListener, PyChangeListener> classChangeListener (m, "ChangeListener");

    classChangeListener
        .def (py::init<>())
        .def ("changeListenerCallback", &ChangeListener::changeListenerCallback)
    ;

    py::class_<ChangeBroadcaster> classChangeBroadcaster (m, "ChangeBroadcaster");

    classChangeBroadcaster
        .def (py::init<>())
        .def ("addChangeListener", &ChangeBroadcaster::addChangeListener)
        .def ("removeChangeListener", &ChangeBroadcaster::removeChangeListener)
        .def ("removeAllChangeListeners", &ChangeBroadcaster::removeAllChangeListeners)
        .def ("sendChangeMessage", &ChangeBroadcaster::sendChangeMessage)
        .def ("sendSynchronousChangeMessage", &ChangeBroadcaster::sendSynchronousChangeMessage)
        .def ("dispatchPendingMessages", &ChangeBroadcaster::dispatchPendingMessages)
    ;

    // ============================================================================================ juce::MessageManager

    py::class_<MessageManager> classMessageManager (m, "MessageManager");

    struct PyMessageBase : public MessageManager::MessageBase
    {
        void messageCallback() override
        {
            PYBIND11_OVERRIDE_PURE (void, MessageManager::MessageBase, messageCallback);
        }
    };

    py::class_<MessageManager::MessageBase, PyMessageBase> classMessageManagerMessageBase (classMessageManager, "Message");

    classMessageManagerMessageBase
        .def (py::init<>())
        .def ("messageCallback", &MessageManager::MessageBase::messageCallback)
        .def ("post", &MessageManager::MessageBase::post)
    ;

    py::class_<MessageManager::Lock> classMessageManagerInnerLock (classMessageManager, "Lock");

    classMessageManagerInnerLock
        .def (py::init<>())
        .def ("enter", &MessageManager::Lock::enter)
        .def ("tryEnter", &MessageManager::Lock::tryEnter)
        .def ("exit", &MessageManager::Lock::exit)
        .def ("abort", &MessageManager::Lock::abort)
    ;

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
        .def_static ("callAsync", &MessageManager::callAsync)
    //.def ("callFunctionOnMessageThread", &MessageManager::callFunctionOnMessageThread)
        .def ("isThisTheMessageThread", &MessageManager::isThisTheMessageThread)
        .def ("setCurrentThreadAsMessageThread", &MessageManager::setCurrentThreadAsMessageThread)
        .def ("getCurrentMessageThread", &MessageManager::getCurrentMessageThread)
        .def ("currentThreadHasLockedMessageManager", &MessageManager::currentThreadHasLockedMessageManager)
        .def_static ("existsAndIsLockedByCurrentThread", &MessageManager::existsAndIsLockedByCurrentThread)
        .def_static ("existsAndIsCurrentThread", &MessageManager::existsAndIsCurrentThread)
        .def_static ("broadcastMessage", &MessageManager::broadcastMessage)
        .def ("registerBroadcastListener", &MessageManager::registerBroadcastListener)
        .def ("deregisterBroadcastListener", &MessageManager::deregisterBroadcastListener)
        .def ("deliverBroadcastMessage", &MessageManager::deliverBroadcastMessage)
    ;

    // ============================================================================================ juce::Message

    py::class_<Message, MessageManager::MessageBase> classMessage (m, "Message");

    classMessage
        .def (py::init<>())
    ;

    struct PyCallbackMessage : public CallbackMessage
    {
        void messageCallback() override
        {
            PYBIND11_OVERRIDE_PURE (void, CallbackMessage, messageCallback);
        }
    };

    py::class_<CallbackMessage, MessageManager::MessageBase, PyCallbackMessage> classCallbackMessage (classMessageManager, "CallbackMessage");

    classCallbackMessage
        .def (py::init<>())
    ;

    // ============================================================================================ juce::Message

    struct PyMessageListener : public MessageListener
    {
        void handleMessage (const Message& message) override
        {
            PYBIND11_OVERRIDE_PURE (void, MessageListener, handleMessage, message);
        }
    };

    py::class_<MessageListener, PyMessageListener> classMessageListener (m, "MessageListener");

    classMessageListener
        .def (py::init<>())
        .def ("handleMessage", &MessageListener::handleMessage)
        .def ("postMessage", &MessageListener::postMessage)
    ;

    // ============================================================================================ juce::MessageManagerLock

    struct PyMessageManagerLock
    {
        explicit PyMessageManagerLock (Thread* thread)
            : thread (thread)
        {
        }

        explicit PyMessageManagerLock (ThreadPoolJob* threadPoolJob)
            : threadPoolJob (threadPoolJob)
        {
        }

        Thread* thread = nullptr;
        ThreadPoolJob* threadPoolJob = nullptr;
        std::variant<std::monostate, MessageManagerLock> state;
    };

    py::class_<PyMessageManagerLock> classMessageManagerLock (m, "MessageManagerLock");

    classMessageManagerLock
        .def (py::init<Thread*>(), "threadToCheckForExitSignal"_a = nullptr)
        .def (py::init<ThreadPoolJob*>())
        .def ("__enter__", [](PyMessageManagerLock& self)
        {
            if (self.thread != nullptr)
                self.state.emplace<MessageManagerLock> (self.thread);
            else if (self.threadPoolJob != nullptr)
                self.state.emplace<MessageManagerLock> (self.threadPoolJob);
            else
                py::pybind11_fail ("Invalid constructed MessageManagerLock, either Thread or ThreadPoolJob must be provided");

            return std::addressof (std::as_const (self));
        }, py::return_value_policy::reference)
        .def ("__exit__", [](PyMessageManagerLock& self, const std::optional<py::type>&, const std::optional<py::object>&, const std::optional<py::object>&)
        {
            self.state.emplace<std::monostate>();
        })
        .def ("lockWasGained", [](const PyMessageManagerLock& self)
        {
            if (auto lock = std::get_if<MessageManagerLock> (std::addressof (self.state)))
                return lock->lockWasGained();

            return false;
        })
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
