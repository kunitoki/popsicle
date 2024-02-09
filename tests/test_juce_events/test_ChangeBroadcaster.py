import popsicle as juce

#==================================================================================================

class ChangeListener(juce.ChangeListener):
    timesCalled = 0
    lastBroadcaster = None

    def changeListenerCallback(self, broadcaster):
        self.timesCalled += 1
        self.lastBroadcaster = broadcaster

#==================================================================================================

def test_single_send(juce_app):
    b = juce.ChangeBroadcaster()

    l = ChangeListener()
    b.addChangeListener(l)

    b.sendChangeMessage()
    assert l.timesCalled == 0
    assert l.lastBroadcaster is None
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b

#==================================================================================================

def test_multi_send(juce_app):
    b = juce.ChangeBroadcaster()

    l = ChangeListener()
    b.addChangeListener(l)

    b.sendChangeMessage()
    b.sendChangeMessage()
    b.sendChangeMessage()
    assert l.timesCalled == 0
    assert l.lastBroadcaster is None
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b

#==================================================================================================

def test_multi_send_separate_broadcasters(juce_app):
    a = juce.ChangeBroadcaster()
    b = juce.ChangeBroadcaster()
    c = juce.ChangeBroadcaster()

    l = ChangeListener()
    a.addChangeListener(l)
    b.addChangeListener(l)
    c.addChangeListener(l)

    a.sendChangeMessage()
    b.sendChangeMessage()
    c.sendChangeMessage()
    assert l.timesCalled == 0
    assert l.lastBroadcaster is None
    next(juce_app)
    assert l.timesCalled == 3
    assert l.lastBroadcaster == c

#==================================================================================================

def test_remove_listener(juce_app):
    a = juce.ChangeBroadcaster()
    b = juce.ChangeBroadcaster()

    l1 = ChangeListener()
    l2 = ChangeListener()
    a.addChangeListener(l1)
    a.addChangeListener(l2)
    b.addChangeListener(l1)
    b.addChangeListener(l2)

    b.sendChangeMessage()
    next(juce_app)
    assert l1.timesCalled == 1
    assert l1.lastBroadcaster == b
    assert l2.timesCalled == 1
    assert l2.lastBroadcaster == b

    a.removeChangeListener(l2)
    a.sendChangeMessage()
    next(juce_app)
    assert l1.timesCalled == 2
    assert l1.lastBroadcaster == a
    assert l2.timesCalled == 1
    assert l2.lastBroadcaster == b

#==================================================================================================

def test_remove_all_listeners(juce_app):
    b = juce.ChangeBroadcaster()

    l1 = ChangeListener()
    l2 = ChangeListener()
    l3 = ChangeListener()
    b.addChangeListener(l1)
    b.addChangeListener(l2)
    b.addChangeListener(l3)

    b.sendChangeMessage()
    next(juce_app)
    assert l1.timesCalled == 1
    assert l2.timesCalled == 1
    assert l3.timesCalled == 1

    b.removeAllChangeListeners()
    b.sendChangeMessage()
    next(juce_app)
    assert l1.timesCalled == 1
    assert l2.timesCalled == 1
    assert l3.timesCalled == 1

#==================================================================================================

def test_synchronous_send(juce_app):
    b = juce.ChangeBroadcaster()

    l = ChangeListener()
    b.addChangeListener(l)

    b.sendSynchronousChangeMessage()
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b

    b.sendChangeMessage()
    b.sendSynchronousChangeMessage()
    assert l.timesCalled == 2
    assert l.lastBroadcaster == b
    next(juce_app)
    assert l.timesCalled == 2
    assert l.lastBroadcaster == b

#==================================================================================================

def test_dispatch_pending_messages(juce_app):
    b = juce.ChangeBroadcaster()

    l = ChangeListener()
    b.addChangeListener(l)

    assert l.timesCalled == 0
    assert l.lastBroadcaster is None
    b.dispatchPendingMessages()
    assert l.timesCalled == 0
    assert l.lastBroadcaster is None
    next(juce_app)
    assert l.timesCalled == 0
    assert l.lastBroadcaster is None

    b.sendChangeMessage()
    b.sendChangeMessage()
    b.dispatchPendingMessages()
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastBroadcaster == b
