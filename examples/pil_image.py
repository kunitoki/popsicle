
import popsicle as juce


class ImageJuce(juce.Image):
    def __init__(self, im):
        """
        An PIL image wrapper for JUCE. This is a subclass of JUCE's Image class.

        :param im: A PIL Image object.
        """
        if im.mode == "RGB":
            im = im.convert("RGBA")
            data = im.tobytes("raw", "BGRA")
            format = juce.Image.ARGB
        elif im.mode == "RGBA":
            data = im.tobytes("raw", "BGRA")
            format = juce.Image.ARGB
        else:
            raise ValueError(f"Unsupported image format: {im.mode}")

        juce.Image.__init__(self, format, im.size[0], im.size[1], False)

        pixels = juce.Image.BitmapData(self, 0, 0, im.size[0], im.size[1], juce.Image.BitmapData.writeOnly)
        pixels.data = data
