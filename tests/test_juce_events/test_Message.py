import popsicle as juce

#==================================================================================================

timesCalled = 0

class Message(juce.Message):
    def messageCallback(self):
        global timesCalled
        timesCalled += 1

#==================================================================================================

def test_construct_and_post(juce_app):
    global timesCalled

    m = Message()
    assert timesCalled == 0

    m.post()
    assert timesCalled == 0

    next(juce_app)
    assert timesCalled == 1
