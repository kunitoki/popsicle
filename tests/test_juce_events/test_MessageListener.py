import popsicle as juce

#==================================================================================================

timesMessageCalled = 0
timesListenerCalled = 0

class Message(juce.Message):
    def __init__(self, name):
        super().__init__()
        self.name = name

    def getName(self):
        return self.name

    def messageCallback(self):
        global timesMessageCalled
        timesMessageCalled += 1

class MessageListener(juce.MessageListener):
    timesCalled = 0
    lastName = None

    def handleMessage(self, message):
        self.timesCalled += 1
        self.lastName = message.getName()

#==================================================================================================

def test_construct_and_post(juce_app):
    l = MessageListener()
    m = Message("Gilles")

    l.postMessage(m)
    assert l.timesCalled == 0
    assert l.lastName is None
    assert timesMessageCalled == 0

    next(juce_app)
    # assert l.timesCalled == 1
    # assert l.lastName == "Gilles"
    assert timesMessageCalled == 1
