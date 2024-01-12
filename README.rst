.. image:: https://github.com/kunitoki/popsicle/raw/master/logo.png
    :alt: popsicle
    :target: https://github.com/kunitoki/popsicle

========
popsicle
========

Popsicle is a project that aims to give *JUCE* (https://juce.com/) a broader audience by allowing it to be used from python. Thanks to *pybind11* (https://pybind11.readthedocs.io/en/stable/) it exposes the JUCE framework api in a pythonic way, and the way it enables to write apps in python is very much similar to the way of writing them in C++ but without the overweight of managing project build, configurations and IDE solutions.

|linux_builds| |macos_builds| |windows_builds| |pypi_status| |pypi_license| |pypi_version| |pypi_downloads|

.. |linux_builds| image:: https://github.com/kunitoki/popsicle/workflows/Linux%20Builds/badge.svg
    :alt: Linux Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |macos_builds| image:: https://github.com/kunitoki/popsicle/workflows/macOS%20Builds/badge.svg
    :alt: macOS Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |windows_builds| image:: https://github.com/kunitoki/popsicle/workflows/Windows%20Builds/badge.svg
    :alt: Windows Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |pypi_license| image:: https://img.shields.io/pypi/l/popsicle
    :alt: PyPI - License
    :target: https://github.com/kunitoki/popsicle/blob/master/LICENSE

.. |pypi_status| image:: https://img.shields.io/pypi/status/popsicle
    :alt: PyPI - Status
    :target: https://pypi.org/project/popsicle/

.. |pypi_version| image:: https://img.shields.io/pypi/pyversions/popsicle
    :alt: PyPI - Python Version
    :target: https://pypi.org/project/popsicle/

.. |pypi_downloads| image:: https://img.shields.io/pypi/dm/popsicle
    :alt: PyPI - Downloads
    :target: https://pypi.org/project/popsicle/

--------
Features
--------

- Easy and quick to iterate over a JUCE application, no need to setup a build environment.
- The way it allows to write JUCE code is very similar to how you would write it in C++.
- Ships as wheel to be used in python scripts
- Allows embedding in existing JUCE apps (ships as a JUCE module)

-------------
Example usage
-------------

A single 80 lines script is better than thousand of words:

.. code-block:: python

  import popsicle as juce


  class MainContentComponent(juce.Component, juce.Timer):
      def __init__(self):
          juce.Component.__init__(self)
          juce.Timer.__init__(self)

          self.setSize(600, 400)
          self.startTimerHz(60)

      def __del__(self):
          self.stopTimer()

      def paint(self, g):
          g.fillAll(juce.Colour.fromRGBA(0, 0, 0, 255))

          random = juce.Random.getSystemRandom()
          rect = juce.Rectangle[int](0, 0, 20, 20)

          for _ in range(100):
              g.setColour(juce.Colour.fromRGBA(
                  random.nextInt(256),
                  random.nextInt(256),
                  random.nextInt(256),
                  255))

              rect.setCentre(random.nextInt(self.getWidth()), random.nextInt(self.getHeight()))
              g.drawRect(rect)

      def timerCallback(self):
          if self.isVisible():
              self.repaint()


  class MainWindow(juce.DocumentWindow):
      component = None

      def __init__(self):
          super().__init__(
              juce.JUCEApplication.getInstance().getApplicationName(),
              juce.Colour.fromRGBA(0, 0, 0, 255), #juce.Desktop.getInstance().getDefaultLookAndFeel().findColour(juce.ResizableWindow.backgroundColourId),
              juce.DocumentWindow.allButtons,
              True)

          self.component = MainContentComponent()

          self.setResizable(True, True)
          self.setContentNonOwned(self.component, True)
          self.centreWithSize(800, 600)
          self.setVisible(True)

      def __del__(self):
          if self.component:
              del self.component
              self.component = None

      def closeButtonPressed(self):
          juce.JUCEApplication.getInstance().systemRequestedQuit()


  class Application(juce.JUCEApplication):
      window = None

      def getApplicationName(self):
          return "JUCE-o-matic"

      def getApplicationVersion(self):
          return "1.0"

      def initialise(self, commandLine):
          self.window = MainWindow()

      def shutdown(self):
          if self.window:
              del self.window
              self.window = None


  if __name__ == "__main__":
      juce.START_JUCE_APPLICATION(Application)

As easy as that ! You will find more example on JUCE usage in the *examples* folder.

--------------------
Example Applications
--------------------

Some images of JUCE tutorials and other small apps ported to *popsicle*.

Animated Component (https://docs.juce.com/master/tutorial_animation.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/animated_component.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/animated_component.py

Audio Player with waveforms (https://docs.juce.com/master/tutorial_audio_thumbnail.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/audio_player_waveform.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/audio_player_wave_cpp.py

Slider decibels (https://docs.juce.com/master/tutorial_synth_db_level_control.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/slider_decibels.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/slider_decibels.py

Slider values example (https://docs.juce.com/master/tutorial_slider_values.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/slider_values_example.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/slider_values_example.py

Wavetable oscillator (https://docs.juce.com/master/tutorial_wavetable_synth.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/wavetable_oscillator.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/wavetable_oscillator.py

Responsive GUI layouts using FlexBox and Grid (https://docs.juce.com/master/tutorial_flex_box_grid.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/layout_flexgrid.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/layout_flexgrid.py

Advanced GUI layout techniques (https://docs.juce.com/master/tutorial_rectangle_advanced.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/layout_rectangles.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/layout_rectangles.py

Table listbox (https://docs.juce.com/master/tutorial_table_list_box.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/table_list_box.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/table_list_box.py

Super Simple Animated Graphics

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/juce_o_matic.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/juce_o_matic.py

------------
Installation
------------

Installing popsicle is as easy as pulling from pypi (osx only for now):

.. code-block:: bash

  pip3 install juce

Make sure you have a recent *pip* if you are on BigSur intel.

-----------------
Build From Source
-----------------

Clone the repository recursively as JUCE is a submodule

.. code-block:: bash

  git clone --recursive git@github.com:kunitoki/popsicle.git

Install python dependencies.

.. code-block:: bash

  # Cleanup the temporary folders
  python3 setup.py clean --all

  # Build the binary distribution
  python3 setup.py bdist_wheel

  # Install the local wheel
  pip3 install dist/popsicle-*.whl

Eventually uploading to PyPI:

.. code-block:: bash

  python3 -m twine upload --repository popsicle dist/popsicle-*.whl
