import time

import popsicle as juce

#==================================================================================================

class CustomTimer(juce.Timer):
    timesCalled = 0

    def timerCallback(self):
        self.timesCalled += 1

#==================================================================================================

def test_single_timer(juce_app):
    t = CustomTimer()
    assert not t.isTimerRunning()

    repeats = 2
    milliseconds = 100

    t.startTimer(milliseconds)
    assert t.getTimerInterval() == milliseconds
    assert t.isTimerRunning()
    assert t.timesCalled == 0

    while t.timesCalled < repeats:
        juce_app.processEvents(milliseconds)

    assert t.isTimerRunning()
    assert t.timesCalled >= repeats

    t.stopTimer()
    assert not t.isTimerRunning()

#==================================================================================================

def test_call_pending_timers_synchronously(juce_app):
    t = CustomTimer()

    t.startTimer(100)
    assert t.timesCalled == 0
    juce_app.processEvents(1)
    #assert t.timesCalled == 0

    time.sleep(0.2)

    juce.Timer.callPendingTimersSynchronously()
    #assert t.timesCalled >= 1

    t.stopTimer()

#==================================================================================================

def test_call_after_delay(juce_app):
    milliseconds = 100
    called = False

    def callback():
        nonlocal called
        called = True

    juce.Timer.callAfterDelay(milliseconds, callback)
    assert not called

    while not called:
        juce_app.processEvents(20)

    assert called
