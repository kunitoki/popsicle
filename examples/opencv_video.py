import os
import cv2
import threading
import queue

from juce_init import START_JUCE_COMPONENT
import popsicle as juce


def captureCameraCallback(imageQ, paramsQ, faceDetector, shouldStop, scaleFactor, minNeighbors):
	cap = cv2.VideoCapture(0)

	cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
	cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

	while cap.isOpened() and not shouldStop.is_set():
		# Capture camera frames
		ret, frame = cap.read()
		if not ret:
			break

		# Get parameters
		try:
			while True:
				scaleFactor, minNeighbors = paramsQ.get_nowait()
		except queue.Empty:
			pass

		# Detect the face
		frame = cv2.flip(frame, 1)
		gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

		faces = faceDetector.detectMultiScale(
			gray,
			scaleFactor=scaleFactor,
			minNeighbors=minNeighbors,
			minSize=(50, 50)
		)

		# Process effects
		blur = 20
		frame = cv2.blur(frame, (blur + 1, blur + 1))

		# Provide rects to juce
		rects = []
		for (x, y, w, h) in faces:
			rects.append(juce.Rectangle[int](x, y, w, h))

		imageQ.put((cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA), rects))

	cap.release()


class MainContentComponent(juce.Component, juce.Timer):
	juceImage = None
	imageQ = queue.Queue()
	paramsQ = queue.Queue()
	shouldStop = threading.Event()
	rects = []
	time = 0.0

	def __init__(self):
		juce.Component.__init__(self)
		juce.Timer.__init__(self)

		self.blurLabel = juce.Label()
		self.blurLabel.setText("Scale Factor", juce.dontSendNotification)
		self.addAndMakeVisible(self.blurLabel)

		self.blurSlider = juce.Slider()
		self.blurSlider.setRange(1.01, 5.0, 0.01)
		self.blurSlider.setValue(1.2, juce.dontSendNotification)
		self.blurSlider.onValueChange = lambda: self.updateParameters()
		self.addAndMakeVisible(self.blurSlider)

		self.brightnessLabel = juce.Label()
		self.brightnessLabel.setText("Min Neighbors", juce.dontSendNotification)
		self.addAndMakeVisible(self.brightnessLabel)

		self.brightnessSlider = juce.Slider()
		self.brightnessSlider.setRange(1.0, 100.0, 1.0)
		self.brightnessSlider.setValue(10.0, juce.dontSendNotification)
		self.brightnessSlider.onValueChange = lambda: self.updateParameters()
		self.addAndMakeVisible(self.brightnessSlider)

		self.updateParameters()

		self.faceCascade = cv2.CascadeClassifier(juce.File(os.path.abspath(__file__)).withFileExtension(".xml").getFullPathName())

		self.thread = threading.Thread(target=captureCameraCallback, args=(
			self.imageQ, self.paramsQ, self.faceCascade, self.shouldStop, 1.2, 10))
		self.thread.start()

		self.startTimerHz(30)

		self.setSize(640, 480 + 44 + 40)
		self.setOpaque(True)

	def updateParameters(self):
		self.paramsQ.put((
			self.blurSlider.getValue(),
			int(self.brightnessSlider.getValue()),
		))

	def convertCvImageToJuce(self, cvImage, juceImage = None):
		height, width, _ = cvImage.shape

		if juceImage is None or width != juceImage.getWidth() or height != juceImage.getHeight():
			juceImage = juce.Image(juce.Image.ARGB, width, height, False)

		pixels = juce.Image.BitmapData(juceImage, 0, 0, width, height, juce.Image.BitmapData.writeOnly)
		pixels.data = cvImage.data

		return juceImage

	def visibilityChanged(self):
		if not self.isVisible():
			self.shouldStop.set()
			self.thread.join()

	def timerCallback(self):
		img = None
		rects = []

		try:
			while True:
				img, rects = self.imageQ.get_nowait()

		except queue.Empty:
			pass

		if img is not None:
			self.juceImage = self.convertCvImageToJuce(img, self.juceImage)
			self.rects = rects

		self.time += juce.degreesToRadians(2.0)
		self.repaint()

	def paint(self, g: juce.Graphics):
		g.fillAll(juce.Colours.black)

		if self.juceImage:
			g.drawImageWithin(self.juceImage, 0, 0, self.getWidth(), self.getHeight() - 44 - 40, juce.RectanglePlacement.onlyReduceInSize)

		for r in self.rects:
			center = juce.Point[float](r.getCentreX(), r.getCentreY())

			p = juce.Path()
			p.addStar(center, 6, 70, 160, self.time)

			gradient = juce.ColourGradient(
				juce.Colours.greenyellow, r.getCentreX(), r.getCentreY(),
				juce.Colours.cornflowerblue, r.getBottomRight().x, r.getBottomRight().y,
				isRadial=True)
			g.setGradientFill(gradient)
			g.fillPath(p)

			g.setColour(juce.Colours.black)
			g.strokePath(p, juce.PathStrokeType(2.0))

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
