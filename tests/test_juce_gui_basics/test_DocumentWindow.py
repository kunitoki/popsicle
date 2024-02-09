import popsicle as juce

#==================================================================================================

class Window(juce.DocumentWindow):
    def closeButtonPressed(self):
        self.setVisible(False)
        self.removeFromDesktop()

    def minimiseButtonPressed(self):
        print("min")

    def maximiseButtonPressed(self):
        print("max")

#==================================================================================================

def test_display_visibility(juce_app):
    window = Window("abc", juce.Colours.white, juce.DocumentWindow.allButtons, True)
    assert window.isOnDesktop()
    assert window.getWidth() == 128
    assert window.getHeight() == 128
    assert not window.isVisible()
    assert not window.isShowing()

    window.centreWithSize(200, 200)
    assert window.getWidth() == 200
    assert window.getHeight() == 200

    window.setVisible(True)
    assert window.isVisible()
    assert window.isShowing()

    window.setVisible(False)
    assert not window.isVisible()
    assert not window.isShowing()

    window.setVisible(True)
    window.removeFromDesktop()
    assert window.isVisible()
    assert not window.isShowing()

#==================================================================================================

def test_display_not_on_desktop(juce_app):
    window = Window("abc", juce.Colours.white, juce.DocumentWindow.allButtons, False)
    assert not window.isOnDesktop()
    assert not window.isVisible()
    assert not window.isShowing()

    window.addToDesktop()
    assert window.isOnDesktop()
    assert not window.isVisible()
    assert not window.isShowing()

#==================================================================================================

def test_buttons(juce_app):
    buttons = juce.DocumentWindow.allButtons
    window = Window("abc", juce.Colours.white, buttons, True)
    assert isinstance (window.getCloseButton(), juce.Button)
    assert isinstance (window.getMinimiseButton(), juce.Button)
    assert isinstance (window.getMaximiseButton(), juce.Button)

    buttons = juce.DocumentWindow.minimiseButton | juce.DocumentWindow.maximiseButton
    window = Window("abc", juce.Colours.white, buttons, True)
    assert window.getCloseButton() is None
    assert isinstance (window.getMinimiseButton(), juce.Button)
    assert isinstance (window.getMaximiseButton(), juce.Button)

    buttons = 0
    window = Window("abc", juce.Colours.white, buttons, True)
    assert window.getCloseButton() is None
    assert window.getMinimiseButton() is None
    assert window.getMaximiseButton() is None
