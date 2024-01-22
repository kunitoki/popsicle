from ..fixtures import juce_app
import popsicle as juce


#==================================================================================================

def test_message_manager_instance():
    mm = juce.MessageManager.getInstance()
    assert mm.isThisTheMessageThread()
    assert not mm.hasStopMessageBeenSent()
    assert mm.currentThreadHasLockedMessageManager()
    assert juce.MessageManager.existsAndIsCurrentThread()
    assert juce.MessageManager.existsAndIsLockedByCurrentThread()
    assert mm.getCurrentMessageThread() is not None

#==================================================================================================

def test_message_manager_run_dispatch_loop_until():
    mm = juce.MessageManager.getInstance()

    x = 10
    def testAsync():
        nonlocal x
        x = 11

    mm.callAsync(testAsync)
    assert x == 10
    mm.runDispatchLoopUntil(10)
    assert x == 11

#==================================================================================================

class ActionListener(juce.ActionListener):
    message = None

    def actionListenerCallback(self, message: str):
        self.message = message

def test_message_manager_broadcast_listener():
    mm = juce.MessageManager.getInstance()
    listener = ActionListener()

    mm.registerBroadcastListener(listener)
    mm.deliverBroadcastMessage("123")
    assert listener.message is None
    mm.runDispatchLoopUntil(10)
    assert listener.message == "123"

    mm.deregisterBroadcastListener(listener)
    mm.deliverBroadcastMessage("abc")
    mm.runDispatchLoopUntil(10)
    assert listener.message == "123"

