from collections import defaultdict

import popsicle as juce

#==================================================================================================

class CustomMultiTimer(juce.MultiTimer):
    timesCalled = defaultdict(int)

    def timerCallback(self, timerID):
        self.timesCalled[timerID] += 1

#==================================================================================================

def test_multi_timer(juce_app):
    t = CustomMultiTimer()
    assert not t.isTimerRunning(1)
    assert not t.isTimerRunning(2)

    repeats = 2
    milliseconds1 = 100
    milliseconds2 = 50

    t.startTimer(1, milliseconds1)
    t.startTimer(2, milliseconds2)
    assert t.getTimerInterval(1) == milliseconds1
    assert t.getTimerInterval(2) == milliseconds2
    assert t.isTimerRunning(1)
    assert t.isTimerRunning(2)
    assert t.timesCalled[1] == 0
    assert t.timesCalled[2] == 0

    while t.timesCalled[1] < repeats or t.timesCalled[2] < repeats:
        juce_app.processEvents(min(milliseconds1, milliseconds2))

    assert t.isTimerRunning(1)
    assert t.isTimerRunning(2)
    assert t.timesCalled[1] >= repeats
    assert t.timesCalled[2] >= repeats

    t.stopTimer(1)
    assert not t.isTimerRunning(1)
    assert t.isTimerRunning(2)

    t.stopTimer(2)
    assert not t.isTimerRunning(2)
