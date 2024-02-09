from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class MainContentComponent(juce.Component):
    genderLabel = juce.Label("", "I identify my gender as...")
    maleButton = juce.ToggleButton("Male")
    femaleButton = juce.ToggleButton("Female")

    hobbiesLabel = juce.Label("", "My hobbies are...")
    sportButton = juce.ToggleButton("Sport")
    artButton = juce.ToggleButton("Art")
    filmButton = juce.ToggleButton("Film")

    GenderButtons = 1001

    def __init__(self):
        juce.Component.__init__(self)

        self.addAndMakeVisible(self.genderLabel)
        self.addAndMakeVisible(self.maleButton)
        self.addAndMakeVisible(self.femaleButton)
        self.maleButton.onClick = lambda: self.updateToggleState(self.maleButton, "Male")
        self.femaleButton.onClick = lambda: self.updateToggleState(self.femaleButton, "Female")
        self.maleButton.setRadioGroupId(self.GenderButtons, juce.dontSendNotification)
        self.femaleButton.setRadioGroupId(self.GenderButtons, juce.dontSendNotification)

        self.addAndMakeVisible(self.hobbiesLabel)
        self.addAndMakeVisible(self.sportButton)
        self.addAndMakeVisible(self.artButton)
        self.addAndMakeVisible(self.filmButton)
        self.sportButton.onClick = lambda: self.updateToggleState(self.sportButton, "Sport")
        self.artButton.onClick = lambda: self.updateToggleState(self.artButton, "Art")
        self.filmButton.onClick = lambda: self.updateToggleState(self.filmButton, "Film")

        self.maleButton.setClickingTogglesState(True)
        self.femaleButton.setClickingTogglesState(True)
        self.sportButton.setClickingTogglesState(True)
        self.artButton.setClickingTogglesState(True)
        self.filmButton.setClickingTogglesState(True)

        self.setSize(400, 300)

    def resized(self):
        self.genderLabel.setBounds (10, 10, self.getWidth() - 20, 20)
        self.maleButton.setBounds (20, 40, self.getWidth() - 30, 20)
        self.femaleButton.setBounds (20, 70, self.getWidth() - 30, 20)

        self.hobbiesLabel.setBounds (10, 110, self.getWidth() - 20, 20)
        self.sportButton.setBounds (20, 140, self.getWidth() - 30, 20)
        self.artButton.setBounds (20, 170, self.getWidth() - 30, 20)
        self.filmButton.setBounds (20, 200, self.getWidth() - 30, 20)

    def updateToggleState(self, button, name):
        state = button.getToggleState()
        stateString = "ON" if state else "OFF"
        selectedString = " (selected)" if state else ""

        print(name, "Button changed to", stateString)
        button.setButtonText(name + selectedString)


if __name__ == "__main__":
	START_JUCE_COMPONENT(MainContentComponent, name="Radio Button And Checkboxes")
