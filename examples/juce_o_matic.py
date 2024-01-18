import juce_init
import popsicle as juce


class MainContentComponent(juce.Component, juce.Timer):
	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.setSize(600, 400)
		self.setOpaque(True)
		self.startTimerHz(60)

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.black)

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

	def mouseDown(self, event: juce.MouseEvent):
		print("mouseDown", event)

	def mouseMove(self, event: juce.MouseEvent):
		print("mouseMove", event.position.x, event.position.y)

	def mouseUp(self, event: juce.MouseEvent):
		print("mouseUp", event)

	def timerCallback(self):
		self.repaint()


class MainWindow(juce.DocumentWindow):
	component = None

	def __init__(self):
		super().__init__(
			juce.JUCEApplication.getInstance().getApplicationName(),
			juce.Desktop.getInstance().getDefaultLookAndFeel()
				.findColour(juce.ResizableWindow.backgroundColourId),
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

	def initialise(self, commandLineParameters: str):
		self.window = MainWindow()

		juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

	def shutdown(self):
		if self.window:
			del self.window

	def systemRequestedQuit(self):
		self.quit()


if __name__ == "__main__":
	juce.START_JUCE_APPLICATION(Application)
