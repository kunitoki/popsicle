import pytest

import popsicle as juce

#==================================================================================================

class Thread(juce.Thread):
    counter = 0
    threadID = None

    def run(self):
        self.setCurrentThreadName("123")
        self.threadID = juce.Thread.getCurrentThreadId()

        self.notify()

        while not self.threadShouldExit():
            self.counter += 1

        assert juce.Thread.currentThreadShouldExit()

        self.notify()

class ThreadListener(juce.Thread.Listener):
    receivedSignal = False

    def exitSignalSent(self):
        self.receivedSignal = True

#==================================================================================================

def test_construct_and_start_stop():
    threadID = juce.Thread.getCurrentThreadId()
    assert threadID == threadID

    a = Thread("abc")
    assert a.getThreadName() == "abc"
    assert not a.isThreadRunning()

    listener = ThreadListener()
    a.addListener(listener)

    a.startThread()
    assert a.isThreadRunning()
    assert not a.isRealtime()

    if not a.wait(10000):
        assert False

    assert a.threadID is not None and threadID != a.threadID
    assert a.getThreadName() == "abc"

    a.stopThread(10000)
    assert not a.isThreadRunning()
    assert a.counter > 0
    assert listener.receivedSignal == True

#==================================================================================================

def test_construct_and_start_signal_stop():
    a = Thread("abc")
    assert not a.isThreadRunning()

    a.startRealtimeThread(juce.Thread.RealtimeOptions()
        .withProcessingTimeMs(100)
        .withPriority(juce.Thread.Priority.highest))
    assert a.isThreadRunning()
    assert a.isRealtime()

    listener = ThreadListener()
    a.addListener(listener)

    if not a.wait(10000):
        assert False

    a.removeListener(listener)

    a.signalThreadShouldExit()

    if not a.waitForThreadToExit(10000):
        assert False

    assert not a.isThreadRunning()
    assert a.counter > 0
    assert listener.receivedSignal == False

#==================================================================================================

@pytest.mark.skip(reason="This deadlocks, most likely on the GIL")
def test_launch():
    launched = False
    start_event = juce.WaitableEvent()
    stop_event = juce.WaitableEvent()

    def runThread():
        nonlocal launched, start_event, stop_event

        start_event.signal()
        launched = True
        stop_event.signal()

    assert juce.Thread.launch(runThread)

    start_event.wait()
    stop_event.wait()

    assert launched
