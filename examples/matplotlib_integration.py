import io
import multiprocessing
import queue

import numpy as np
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


def make_plot(fig):
	x = np.linspace(0, 10, 11)
	y = [3.9, 4.4, 10.8, 10.3, 11.2, 13.1, 14.1,  9.9, 13.9, 15.1, 12.5]

	a, b = np.polyfit(x, y, deg=1)
	y_est = a * x + b
	y_err = x.std() * np.sqrt(1/len(x) + (x - x.mean())**2 / np.sum((x - x.mean())**2))

	ax = fig.add_subplot(111)
	ax.plot(x, y_est, '-')
	ax.fill_between(x, y_est - y_err, y_est + y_err, alpha=0.2)
	ax.plot(x, y, 'o', color='tab:brown')


def generate_plot_png(q, width, height):
	fig = Figure(figsize=(width / 100, height / 100), dpi=100)
	canvas = FigureCanvasAgg(fig)

	make_plot(fig)
	canvas.draw()

	with io.BytesIO() as img_buf:
		canvas.print_png(img_buf)
		q.put(img_buf.getbuffer().tobytes())


class MainContentComponent(juce.Component, juce.Timer):
	img = None
	q = multiprocessing.Queue()
	process = None
	width = 600
	height = 400
	time = 0.0

	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.drawableImage = juce.DrawableImage()
		self.drawableImage.setOpaque(True)
		self.addChildComponent(self.drawableImage)

		self.setSize(int(self.width), int(self.height))
		self.setOpaque(True)

		self.process = multiprocessing.Process(target=generate_plot_png, args=[self.q, self.width, self.height])
		self.process.start()

		self.startTimerHz(24)

	def timerCallback(self):
		if not self.drawableImage.getImage().isValid():
			try:
				image_data = self.q.get_nowait()

				self.drawableImage.setImage(self.createImageFromBuffer(image_data))

				juce.Desktop.getInstance().getAnimator().fadeIn(self.drawableImage, 1000)

			except queue.Empty:
				pass

		else:
			if not juce.Desktop.getInstance().getAnimator().isAnimating(self.drawableImage):
				self.stopTimer()

		self.time += juce.degreesToRadians(6.0)
		self.repaint()

	def createImageFromBuffer(self, image_data) -> juce.Image:
		return juce.ImageCache.getFromMemory(image_data)

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.white)

		if not self.drawableImage.isVisible() or juce.Desktop.getInstance().getAnimator().isAnimating(self.drawableImage):
			b = self.getLocalBounds()
			center = juce.Point[float](b.getCentreX(), b.getCentreY())

			p = juce.Path()
			p.addStar(center, 5, 25, 60, self.time)

			g.setColour(juce.Colours.blueviolet)
			g.fillPath(p)

	def resized(self):
		self.drawableImage.setBounds(self.getLocalBounds())
		self.drawableImage.setBoundingBox(self.getLocalBounds().toFloat())


if __name__ == "__main__":
	START_JUCE_COMPONENT(MainContentComponent, name="Matplotlib Example")
