from matplotlib.backends.backend_agg import FigureCanvasAgg
from matplotlib.figure import Figure

import io
import matplotlib.pyplot as plt

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


def generate_image(width, height):
	fig = Figure(figsize=(width / 100, height / 100), dpi=100)
	canvas = FigureCanvasAgg(fig)

	ax = fig.add_subplot(111)
	ax.plot([1, 2, 3])

	canvas.draw()
	raw_data = bytearray(canvas.buffer_rgba())

	img = juce.Image(juce.Image.ARGB, width, height, True)

	data = juce.Image.BitmapData(img, 0, 0, width, height, juce.Image.BitmapData.writeOnly)
	for y in range(height):
		line_stride = y * width * 4
		for x in range(width):
			pixel_stride = line_stride + x * 4
			data.data[pixel_stride+ 0] = 255
			data.data[pixel_stride + 1] = raw_data[pixel_stride + 0]
			data.data[pixel_stride + 2] = raw_data[pixel_stride + 1]
			data.data[pixel_stride + 3] = raw_data[pixel_stride + 2]

	return img


def generate_image_2(width, height):
	plt.rcParams["figure.figsize"] = [width / 100, height / 100]
	plt.rcParams["figure.autolayout"] = True

	plt.figure()
	plt.plot([1, 2])

	img_buf = io.BytesIO()
	plt.savefig(img_buf, format='png')
	img = juce.ImageCache.getFromMemory(img_buf.getbuffer())
	img_buf.close()

	return img


class MainContentComponent(juce.Component):
	def __init__(self):
		super().__init__()

		width = 600
		height = 400

		self.img = generate_image_2(width, height)

		self.setSize(int(width), int(height))
		self.setOpaque(True)

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.black)
		g.drawImageAt(self.img, 0, 0, False)


if __name__ == "__main__":
	START_JUCE_COMPONENT(MainContentComponent, name="Matplotlib Example")
