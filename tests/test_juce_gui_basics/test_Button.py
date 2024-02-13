from ..utilities import equal_images

import popsicle as juce

#==================================================================================================

class CustomButton(juce.Button):
    timesClicked = 0
    timesStateChanged = 0

    def paintButton(self, g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown):
        if self.getToggleState():
            g.fillAll(juce.Colours.red)
        else:
            g.fillAll(juce.Colours.green)

    def clicked(self):
        self.timesClicked += 1

    def buttonStateChanged(self):
        self.timesStateChanged += 1

class CustomButtonListener(juce.Button.Listener):
    lastClickedButton = None
    lastChangedButton = None
    timesClicked = 0
    timesStateChanged = 0

    def buttonClicked(self, btn):
        self.lastClickedButton = btn
        self.timesClicked += 1

    def buttonStateChanged(self, btn):
        self.lastChangedButton = btn
        self.timesStateChanged += 1

#==================================================================================================

def test_state_properties(juce_app):
    b = CustomButton("abc")
    assert not b.isDown()
    assert not b.isOver()

#==================================================================================================

def test_button_text(juce_app):
    b = CustomButton("abc")
    assert b.getButtonText() == "abc"

    b.setButtonText("123")
    assert b.getButtonText() == "123"

#==================================================================================================

def test_connected_edges(juce_app):
    b = CustomButton("abc")
    assert b.getConnectedEdgeFlags() == 0
    assert not b.isConnectedOnLeft()
    assert not b.isConnectedOnRight()
    assert not b.isConnectedOnTop()
    assert not b.isConnectedOnBottom()

    b.setConnectedEdges(juce.Button.ConnectedOnLeft)
    assert b.getConnectedEdgeFlags() == juce.Button.ConnectedOnLeft
    assert b.isConnectedOnLeft()
    assert not b.isConnectedOnRight()
    assert not b.isConnectedOnTop()
    assert not b.isConnectedOnBottom()

    b.setConnectedEdges(juce.Button.ConnectedOnLeft | juce.Button.ConnectedOnRight)
    assert b.getConnectedEdgeFlags() == juce.Button.ConnectedOnLeft | juce.Button.ConnectedOnRight
    assert b.isConnectedOnLeft()
    assert b.isConnectedOnRight()
    assert not b.isConnectedOnTop()
    assert not b.isConnectedOnBottom()

#==================================================================================================

def test_button_states(juce_app):
    b = CustomButton("abc")
    assert b.getState() == juce.Button.buttonNormal

    b.setState(juce.Button.buttonOver)
    assert b.getState() == juce.Button.buttonOver

    b.setState(juce.Button.buttonDown)
    assert b.getState() == juce.Button.buttonDown

    b.setState(juce.Button.buttonNormal)
    assert b.getState() == juce.Button.buttonNormal

#==================================================================================================

def test_shortcut(juce_app):
    b = CustomButton("abc")

    key = juce.KeyPress(juce.KeyPress.F1Key)
    key_shift = juce.KeyPress('a', juce.ModifierKeys.shiftModifier, 'A')

    assert not b.isRegisteredForShortcut(key)
    assert not b.isRegisteredForShortcut(key_shift)

    b.addShortcut(key)
    b.addShortcut(key_shift)

    b.clearShortcuts()

    assert not b.isRegisteredForShortcut(key)
    assert not b.isRegisteredForShortcut(key_shift)

#==================================================================================================

def test_toggleability(juce_app):
    b = CustomButton("abc")
    assert not b.isToggleable()
    assert not b.getToggleState()

    b.setToggleable(True)
    assert b.isToggleable()

    listener = CustomButtonListener()
    b.addListener(listener)

    assert b.timesStateChanged == 0

    b.setToggleState(True, juce.dontSendNotification)
    assert listener.lastChangedButton is None
    assert b.getToggleState()
    assert b.timesStateChanged == 1

    b.setToggleState(False, juce.dontSendNotification)
    assert listener.lastChangedButton is None
    assert not b.getToggleState()
    assert b.timesStateChanged == 2

    b.setToggleState(True, juce.sendNotification)
    assert listener.lastChangedButton == b
    assert b.getToggleState()

    b.removeListener(listener)
    listener.lastChangedButton = None

    b.setToggleState(False, juce.sendNotification)
    assert listener.lastChangedButton is None
    assert not b.getToggleState()

#==================================================================================================

def test_toggleability_by_clicking(juce_app):
    b = CustomButton("abc")
    assert not b.getClickingTogglesState()
    assert not b.getToggleState()

    listener = CustomButtonListener()
    b.addListener(listener)

    b.triggerClick()
    next(juce_app)
    assert not b.getToggleState()
    assert b.timesStateChanged == 1
    assert listener.timesClicked == 1
    assert listener.timesStateChanged == 1

    b.setClickingTogglesState(True)

    b.triggerClick()
    next(juce_app)
    assert b.getToggleState()
    assert b.timesStateChanged == 2
    assert listener.timesClicked == 2
    assert listener.timesStateChanged == 2

#==================================================================================================

def test_clicking_callback(juce_app):
    b = CustomButton("abc")
    b.setSize(100, 20)

    b.triggerClick()
    assert b.timesClicked == 0
    next(juce_app)
    assert b.timesClicked == 1

    b.triggerClick()
    assert b.timesClicked == 1
    next(juce_app)
    assert b.timesClicked == 2

    timesClicked = 0
    def clickHandler():
        nonlocal timesClicked
        timesClicked += 1
    b.onClick = clickHandler

    b.triggerClick()
    assert timesClicked == 0
    next(juce_app)
    assert timesClicked == 1

#==================================================================================================

def test_state_change(juce_app):
    b = CustomButton("abc")
    b.setSize(100, 20)

#==================================================================================================

def test_rendering(juce_app):
    b = CustomButton("abc")
    b.setSize(100, 20)

    b.setToggleState(False, juce.dontSendNotification)
    snapshot_off = b.createComponentSnapshot(b.getLocalBounds())

    b.setToggleState(True, juce.dontSendNotification)
    snapshot_on = b.createComponentSnapshot(b.getLocalBounds())

    assert equal_images(snapshot_off, snapshot_off)
    assert equal_images(snapshot_on, snapshot_on)
    assert not equal_images(snapshot_off, snapshot_on)
