import popsicle as juce

#==================================================================================================

class AsyncUpdater(juce.AsyncUpdater):
    timesCalled = 0

    def handleAsyncUpdate(self):
        self.timesCalled += 1

#==================================================================================================

def test_single_trigger(juce_app):
    a = AsyncUpdater()
    assert not a.isUpdatePending()

    a.triggerAsyncUpdate()
    assert a.isUpdatePending()
    assert a.timesCalled == 0
    next(juce_app)
    assert a.timesCalled == 1

#==================================================================================================

def test_multiple_trigger(juce_app):
    a = AsyncUpdater()

    a.triggerAsyncUpdate()
    a.triggerAsyncUpdate()
    a.triggerAsyncUpdate()
    a.triggerAsyncUpdate()
    assert a.isUpdatePending()
    assert a.timesCalled == 0
    next(juce_app)
    assert not a.isUpdatePending()
    assert a.timesCalled == 1

#==================================================================================================

def test_cancel_single_trigger(juce_app):
    a = AsyncUpdater()

    a.triggerAsyncUpdate()
    assert a.isUpdatePending()
    assert a.timesCalled == 0
    a.cancelPendingUpdate()
    assert not a.isUpdatePending()
    next(juce_app)
    assert not a.isUpdatePending()
    assert a.timesCalled == 0

#==================================================================================================

def test_handle_update_now(juce_app):
    a = AsyncUpdater()

    a.handleUpdateNowIfNeeded()
    assert not a.isUpdatePending()
    assert a.timesCalled == 0

    a.triggerAsyncUpdate()
    assert a.isUpdatePending()
    a.handleUpdateNowIfNeeded()
    assert not a.isUpdatePending()
    assert a.timesCalled == 1
