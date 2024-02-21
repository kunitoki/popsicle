import threading

import popsicle as juce

#==================================================================================================

def test_constructor():
    c = juce.CriticalSection()
    assert c.tryEnter()

#==================================================================================================

def test_double_entering():
    c = juce.CriticalSection()
    assert c.tryEnter()
    assert c.tryEnter()

#==================================================================================================

def test_manual_enter_exit():
    c = juce.CriticalSection()
    c.enter()
    assert c.tryEnter()
    c.exit()

#==================================================================================================

def test_scoped_enter_exit():
    c = juce.CriticalSection()
    data = 1

    def thread_callback():
        nonlocal data
        c.enter()
        data = 2
        c.exit()

    t = threading.Thread(target=thread_callback)
    t.start()

    with juce.CriticalSection.ScopedLockType(c):
        data = 3

    t.join()

    assert data == 2 or data == 3
