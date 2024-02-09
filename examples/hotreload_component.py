import popsicle as juce

__all__ = ["TestComponent"]


class TestComponent(juce.Component, juce.Timer):
	time = 0.0

	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.setOpaque(True)
		self.startTimerHz(24)

	def timerCallback(self):
		self.time += juce.degreesToRadians(1)
		self.repaint()

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.red)

		b = self.getLocalBounds()
		center = juce.Point[float](b.getCentreX(), b.getCentreY())

		p = juce.Path()
		p.addStar(center, 20, 25, self.getWidth(), self.time)

		g.setColour(juce.Colours.green)
		g.fillPath(p)
