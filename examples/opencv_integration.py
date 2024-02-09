import os
import cv2

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


class MainContentComponent(juce.Component):
	juceImage = None

	def __init__(self):
		juce.Component.__init__(self)

		self.blurLabel = juce.Label()
		self.blurLabel.setText("Blur", juce.dontSendNotification)
		self.addAndMakeVisible(self.blurLabel)

		self.blurSlider = juce.Slider()
		self.blurSlider.setRange(0.0, 255.0, 1.0)
		self.blurSlider.setValue(1.0, juce.dontSendNotification)
		self.blurSlider.onValueChange = lambda: self.updateImage()
		self.addAndMakeVisible(self.blurSlider)

		self.brightnessLabel = juce.Label()
		self.brightnessLabel.setText("Brightness", juce.dontSendNotification)
		self.addAndMakeVisible(self.brightnessLabel)

		self.brightnessSlider = juce.Slider()
		self.brightnessSlider.setRange(0.0, 255.0, 1.0)
		self.brightnessSlider.setValue(20.0, juce.dontSendNotification)
		self.brightnessSlider.onValueChange = lambda: self.updateImage()
		self.addAndMakeVisible(self.brightnessSlider)

		self.cvImage = cv2.imread(juce.File(os.path.abspath(__file__)).withFileExtension(".jpg").getFullPathName())
		self.updateImage()

		self.setSize(self.juceImage.getWidth(), self.juceImage.getHeight() + 44 + 40)
		self.setOpaque(True)

	def processBrightness(self, img, brightness):
		hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
		h, s, v = cv2.split(hsv)
		lim = 255 - brightness
		v[v > lim] = 255
		v[v <= lim] += brightness
		final_hsv = cv2.merge((h, s, v))
		return cv2.cvtColor(final_hsv, cv2.COLOR_HSV2BGR)

	def processBlur(self, img, blur):
		kernel_size = (blur + 1, blur + 1)
		return cv2.blur(img, kernel_size)

	def updateImage(self):
		img = self.processBrightness(self.cvImage, int(self.brightnessSlider.getValue()))
		img = self.processBlur(img, int(self.blurSlider.getValue()))

		self.juceImage = self.convertCvImageToJuce(img, self.juceImage)

		self.repaint()

	def convertCvImageToJuce(self, cvImage, juceImage = None):
		height, width, _ = cvImage.shape

		if juceImage is None or width != juceImage.getWidth() or height != juceImage.getHeight():
			juceImage = juce.Image(juce.Image.ARGB, width, height, False)

		pixels = juce.Image.BitmapData(juceImage, 0, 0, width, height, juce.Image.BitmapData.writeOnly)
		cvImage = cv2.cvtColor(cvImage, cv2.COLOR_BGR2BGRA)

		pixels.data = cvImage.data

		return juceImage

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.black)

		if self.juceImage:
			g.drawImageWithin(self.juceImage, 0, 0, self.getWidth(), self.getHeight() - 44 - 40, juce.RectanglePlacement.onlyReduceInSize)

	def resized(self):
		bounds = self.getLocalBounds()

		brightnessBounds = bounds.removeFromBottom(40).reduced(4)
		self.brightnessLabel.setBounds(brightnessBounds.removeFromLeft(100))
		self.brightnessSlider.setBounds(brightnessBounds)

		blurBounds = bounds.removeFromBottom(40).reduced(4)
		self.blurLabel.setBounds(blurBounds.removeFromLeft(100))
		self.blurSlider.setBounds(blurBounds)


if __name__ == "__main__":
	START_JUCE_COMPONENT(MainContentComponent, name="OpenCV Example")
