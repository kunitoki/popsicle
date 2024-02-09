import popsicle as juce

#==================================================================================================

class ActionListener(juce.ActionListener):
    timesCalled = 0
    lastMessage = None

    def actionListenerCallback(self, message):
        self.timesCalled += 1
        self.lastMessage = message

#==================================================================================================

def test_single_send(juce_app):
    b = juce.ActionBroadcaster()

    l = ActionListener()
    b.addActionListener(l)

    b.sendActionMessage("abc")
    assert l.timesCalled == 0
    assert l.lastMessage is None
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastMessage == "abc"

#==================================================================================================

def test_multi_send(juce_app):
    b = juce.ActionBroadcaster()

    l = ActionListener()
    b.addActionListener(l)

    b.sendActionMessage("1")
    b.sendActionMessage("2")
    b.sendActionMessage("3")
    assert l.timesCalled == 0
    assert l.lastMessage is None
    next(juce_app)
    assert l.timesCalled == 3
    assert l.lastMessage == "3"

#==================================================================================================

def test_remove_listener(juce_app):
    b = juce.ActionBroadcaster()

    l = ActionListener()
    b.addActionListener(l)

    b.sendActionMessage("1")
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastMessage == "1"

    b.removeActionListener(l)
    b.sendActionMessage("2")
    next(juce_app)
    assert l.timesCalled == 1
    assert l.lastMessage == "1"

#==================================================================================================

def test_remove_all_listeners(juce_app):
    b = juce.ActionBroadcaster()

    l1 = ActionListener()
    l2 = ActionListener()
    l3 = ActionListener()
    b.addActionListener(l1)
    b.addActionListener(l2)
    b.addActionListener(l3)

    b.sendActionMessage("bark")
    next(juce_app)
    assert l1.timesCalled == 1
    assert l2.timesCalled == 1
    assert l3.timesCalled == 1

    b.removeAllActionListeners()
    b.sendActionMessage("bork")
    next(juce_app)
    assert l1.timesCalled == 1
    assert l2.timesCalled == 1
    assert l3.timesCalled == 1
