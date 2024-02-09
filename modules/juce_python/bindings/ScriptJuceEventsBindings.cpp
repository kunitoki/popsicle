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

#include "ScriptJuceCoreBindings.h"
#include "ScriptJuceEventsBindings.h"

#define JUCE_PYTHON_INCLUDE_PYBIND11_OPERATORS
#define JUCE_PYTHON_INCLUDE_PYBIND11_FUNCTIONAL
#include "../utilities/PyBind11Includes.h"

#include <atomic>
#include <functional>

// =================================================================================================

namespace juce {

#if JUCE_MAC
 extern void initialiseNSApplication();
#endif

} // namespace juce

// =================================================================================================

namespace popsicle::Bindings {

using namespace juce;

namespace py = pybind11;
using namespace py::literals;

void registerJuceEventsBindings (py::module_& m)
{
    // ============================================================================================ juce::NotificationType

    py::enum_<NotificationType> (m, "NotificationType")
        .value ("dontSendNotification", NotificationType::dontSendNotification)
        .value ("sendNotification", NotificationType::sendNotification)
        .value ("sendNotificationSync", NotificationType::sendNotificationSync)
        .value ("sendNotificationAsync", NotificationType::sendNotificationAsync)
        .export_values();

    // ============================================================================================ juce::ActionListener

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
        .def (py::init<std::function<void()>>())
        .def ("triggerAsyncUpdate", &LockingAsyncUpdater::triggerAsyncUpdate)
        .def ("cancelPendingUpdate", &LockingAsyncUpdater::cancelPendingUpdate)
        .def ("handleUpdateNowIfNeeded", &LockingAsyncUpdater::handleUpdateNowIfNeeded)
        .def ("isUpdatePending", &LockingAsyncUpdater::isUpdatePending)
    ;

    // ============================================================================================ juce::AsyncUpdater

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

    py::class_<MessageManager::MessageBase, PyMessageBase<>> classMessageManagerMessageBase (classMessageManager, "MessageBase");

    py::class_<MessageManager::Lock> classMessageManagerInnerLock (classMessageManager, "Lock");

    classMessageManagerMessageBase
        .def (py::init<>())
        .def ("messageCallback", &MessageManager::MessageBase::messageCallback)
        .def ("post", [](py::object self) { return self.release().cast<MessageManager::MessageBase*>()->post(); })
    ;

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
        .def ("callFunctionOnMessageThread", [](MessageManager& self, py::function func) -> py::object
        {
            void* result = nullptr;

            if (func.is_none())
                py::pybind11_fail ("Invalid specified function to \"MessageManager::callFunctionOnMessageThread\"");

            {
                py::gil_scoped_release release;

                result = self.callFunctionOnMessageThread (+[](void* data) -> void*
                {
                    py::gil_scoped_acquire acquire;

                    auto func = *reinterpret_cast<py::function*> (data);

                    return func().ptr();
                }, std::addressof (func));
            }

            if (result == nullptr)
                return py::none();

            return py::reinterpret_borrow<py::object> (reinterpret_cast<PyObject*> (result));
        })
        .def ("isThisTheMessageThread", &MessageManager::isThisTheMessageThread)
        .def ("setCurrentThreadAsMessageThread", &MessageManager::setCurrentThreadAsMessageThread)
        .def ("getCurrentMessageThread", [](const MessageManager& self) { return PyThreadID (self.getCurrentMessageThread()); })
        .def ("currentThreadHasLockedMessageManager", &MessageManager::currentThreadHasLockedMessageManager)
        .def_static ("existsAndIsLockedByCurrentThread", &MessageManager::existsAndIsLockedByCurrentThread)
        .def_static ("existsAndIsCurrentThread", &MessageManager::existsAndIsCurrentThread)
        .def_static ("broadcastMessage", &MessageManager::broadcastMessage)
        .def ("registerBroadcastListener", &MessageManager::registerBroadcastListener)
        .def ("deregisterBroadcastListener", &MessageManager::deregisterBroadcastListener)
        .def ("deliverBroadcastMessage", &MessageManager::deliverBroadcastMessage)
    ;

    // ============================================================================================ juce::Message

    py::class_<Message, MessageManager::MessageBase, PyMessageBase<Message>> classMessage (m, "Message");

    classMessage
        .def (py::init<>())
    ;

    py::class_<CallbackMessage, MessageManager::MessageBase, PyCallbackMessage<>> classCallbackMessage (classMessageManager, "CallbackMessage");

    classCallbackMessage
        .def (py::init<>())
        .def ("messageCallback", &CallbackMessage::messageCallback)
    ;

    // ============================================================================================ juce::Message

    py::class_<MessageListener, PyMessageListener> classMessageListener (m, "MessageListener");

    classMessageListener
        .def (py::init<>())
        .def ("handleMessage", &MessageListener::handleMessage)
        .def ("postMessage", [](MessageListener& self, py::object message)
        {
            if (message.is_none() || ! py::isinstance<Message> (message))
                py::pybind11_fail ("Invalid specified message type in \"MessageListener::postMessage\"");

            return self.postMessage (message.release().cast<Message*>());
        }, "message"_a)
    ;

    // ============================================================================================ juce::MessageManagerLock

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
                py::pybind11_fail ("Invalid constructed \"MessageManagerLock\", either \"Thread\" or \"ThreadPoolJob\" must be provided");

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

    // ============================================================================================ juce::MultiTimer

    py::class_<MultiTimer, PyMultiTimer> classMultiTimer (m, "MultiTimer");

    classMultiTimer
        .def (py::init<>())
        .def ("timerCallback", &MultiTimer::timerCallback)
        .def ("startTimer", &MultiTimer::startTimer)
        .def ("stopTimer", &MultiTimer::stopTimer)
        .def ("isTimerRunning", &MultiTimer::isTimerRunning)
        .def ("getTimerInterval", &MultiTimer::getTimerInterval)
    ;

    // ============================================================================================ initialiseJuce_GUI

#if ! JUCE_PYTHON_EMBEDDED_INTERPRETER
    static std::atomic_int numScopedInitInstances = 0;

    if (numScopedInitInstances.fetch_add(1) == 0)
    {
        initialiseJuce_GUI();

        JUCEApplicationBase::createInstance = +[]() -> JUCEApplicationBase* { return nullptr; };

#if JUCE_MAC
        initialiseNSApplication();
#endif
    }

#if 1
    py::cpp_function cleanupCallback ([](py::handle weakref)
    {
        if (numScopedInitInstances.fetch_sub(1) == 1)
            shutdownJuce_GUI();

        weakref.dec_ref();
    });

    (void) py::weakref (m.attr ("MessageManager"), cleanupCallback).release();

#else
    auto atexit = py::module_::import ("atexit");
    atexit.attr ("register") (py::cpp_function([]
    {
        if (numScopedInitInstances.fetch_sub(1) == 1)
            shutdownJuce_GUI();
    }));

#endif
#endif
}

} // namespace popsicle::Bindings
