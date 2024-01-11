import juce_init
import juce
import sys


class MainContentComponent(juce.Component, juce.Timer):
	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.setSize(600, 400)
		self.setOpaque(True)
		self.startTimerHz(60)

	def paint(self, g):
		g.fillAll(juce.Colour.fromRGBA(0, 0, 0, 255))

		random = juce.Random.getSystemRandom()
		rect = juce.Rectangle[int](0, 0, 20, 20)

		for _ in range(100):
			g.setColour(juce.Colour.fromRGBA(
				random.nextInt(255),
				random.nextInt(255),
				random.nextInt(255),
				255))

			rect.setCentre(random.nextInt(self.getWidth()), random.nextInt(self.getHeight()))
			g.drawRect(rect, 1)

	def mouseDown(self, event):
		print("mouseDown", event)

	def mouseMove(self, event):
		print("mouseMove", event.position.x, event.position.y)

	def mouseUp(self, event):
		print("mouseUp", event)

	def timerCallback(self):
		self.repaint()


class MainWindow(juce.DocumentWindow):
	component = None

	def __init__(self):
		super().__init__(
			juce.JUCEApplication.getInstance().getApplicationName(),
			juce.Colour(0, 0, 0, 255),
			juce.DocumentWindow.allButtons,
			True)

		self.component = MainContentComponent()

		self.setResizable(True, True)
		self.setContentNonOwned(self.component, True)
		self.centreWithSize(800, 600)
		self.setVisible(True)

	def __del__(self):
		self.clearContentComponent()

		if self.component:
			del self.component

	def closeButtonPressed(self):
		juce.JUCEApplication.getInstance().systemRequestedQuit()


class Application(juce.JUCEApplication):
	window = None

	def __init__(self):
		super().__init__()

	def getApplicationName(self):
		return "MyApp"

	def getApplicationVersion(self):
		return "1.0"

	def initialise(self, commandLineParameters):
		print("initialise", commandLineParameters)

		self.window = MainWindow()

	def shutdown(self):
		print("shutdown")

		del self.window

	def systemRequestedQuit(self):
		print("systemRequestedQuit")
		self.quit()

	def unhandledException(self, ex, file, line):
		print(">>", ex, type(ex), file, line)
		sys.exit(1)


if __name__ == "__main__":
	juce.START_JUCE_APPLICATION(Application)
