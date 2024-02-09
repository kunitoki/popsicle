
from juce_init import START_JUCE_COMPONENT
import popsicle as juce



class MainContentComponent(juce.Component):
	def __init__(self):
		super().__init__()

		width = 600
		height = 400

		self.text = juce.DrawableText()
		self.text.setText("Ciao")
		self.text.setColour(juce.Colours.white)
		self.text.setJustification(juce.Justification.centred)
		self.addAndMakeVisible(self.text)

		self.button = juce.TextButton("abc")
		self.addAndMakeVisible(self.button)
		self.button.onClick = lambda: print("clicked")

		self.setSize(int(width), int(height))
		self.setOpaque(True)

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.black)

	def resized(self):
		bounds = self.getLocalBounds()

		self.text.setBounds(bounds.removeFromLeft(self.proportionOfWidth(0.5)))

		self.button.setBounds(bounds)

if __name__ == "__main__":
	START_JUCE_COMPONENT(MainContentComponent, name="Drawables Example")
