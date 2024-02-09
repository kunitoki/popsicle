import popsicle as juce

def createImageFromBuffer(image_data: bytes, width: int, height: int) -> juce.Image:
    img = juce.Image(juce.Image.ARGB, width, height, True)

    pixels = juce.Image.BitmapData(img, 0, 0, width, height, juce.Image.BitmapData.writeOnly)
    for y in range(height):
        line_stride = y * pixels.lineStride
        for x in range(width):
            pixel_stride = line_stride + x * pixels.pixelStride
            pixels.data[pixel_stride + 2] = image_data[pixel_stride + 0]
            pixels.data[pixel_stride + 1] = image_data[pixel_stride + 1]
            pixels.data[pixel_stride + 0] = image_data[pixel_stride + 2]
            pixels.data[pixel_stride + 3] = image_data[pixel_stride + 3]

    return img
