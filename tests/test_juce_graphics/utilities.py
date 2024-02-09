import os
import inspect
import numpy
import imageio.v2 as imageio

import popsicle as juce

#==================================================================================================

this_file = juce.File(os.path.abspath(__file__))

def get_logo_image() -> juce.Image:
    return juce.ImageCache.getFromFile(this_file.getSiblingFile("data").getChildFile(f"logo.png"))

def get_original_image_path(test_name: str) -> juce.File:
    return this_file.getParentDirectory().getSiblingFile("runtime_data").getChildFile(f"{test_name}.png")

def get_reference_image_path(test_name: str) -> juce.File:
    return this_file.getSiblingFile("reference_images").getChildFile(f"{test_name}.png")

def get_failed_folder_path() -> juce.File:
    return this_file.getParentDirectory().getSiblingFile("compare_data")

def get_failed_generated_image_path(test_name: str) -> juce.File:
    return get_failed_folder_path().getChildFile(f"{test_name}_generated.png")

def get_failed_reference_image_path(test_name: str) -> juce.File:
    return get_failed_folder_path().getChildFile(f"{test_name}_reference.png")

def get_failed_diff_image_path(test_name: str) -> juce.File:
    return get_failed_folder_path().getChildFile(f"{test_name}_diff.png")

#==================================================================================================

def load_image(file: juce.File) -> juce.Image:
    return juce.ImageCache.getFromFile(file)

def store_image(file: juce.File, img: juce.Image):
    file.deleteFile()
    assert juce.PNGImageFormat().writeImageToStream(img, juce.FileOutputStream(file))

#==================================================================================================

def mean_squared_error(image1, image2):
    err = numpy.sum((image1 - image2) ** 2)
    err /= float(image1.shape[0] * image1.shape[1])
    return err

def is_image_equal_reference(update_rendering: bool, original_image: juce.Image, threshold = 0.0) -> bool:
    test_name = inspect.getouterframes(inspect.currentframe())[1].function

    reference_path = get_reference_image_path(test_name)
    if not reference_path.existsAsFile() or update_rendering:
        store_image(reference_path, original_image)
        return True

    original_path = get_original_image_path(test_name)
    store_image(original_path, original_image)

    pre_image1 = imageio.imread(original_path.getFullPathName())
    pre_image2 = imageio.imread(reference_path.getFullPathName())
    image1 = pre_image1.astype("float") / 255
    image2 = pre_image2.astype("float") / 255
    mse = mean_squared_error(image1, image2)

    match = mse <= threshold
    if not match:
        result_image_diff = numpy.abs(image1 - image2) / mse
        result_image_diff = (numpy.clip(result_image_diff, 0, 1) * 255).astype("uint8")
        for row in result_image_diff:
            for pixel in row:
                pixel[3] = 255

        imageio.imwrite(get_failed_diff_image_path(test_name).getFullPathName(), result_image_diff)
        imageio.imwrite(get_failed_generated_image_path(test_name).getFullPathName(), pre_image1)
        imageio.imwrite(get_failed_reference_image_path(test_name).getFullPathName(), pre_image2)

    original_path.deleteFile()
    return match
