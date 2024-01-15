.. image:: https://github.com/kunitoki/popsicle/raw/master/logo.png
    :alt: popsicle
    :target: https://github.com/kunitoki/popsicle

|linux_builds| |macos_builds| |windows_builds| |pypi_version| |pypi_downloads|

|pypi_status| |pypi_license| |commercial_license|

========
popsicle
========

Popsicle is a project that aims to give *JUCE* (https://juce.com/) a broader audience by allowing it to be used from python. Thanks to *pybind11* (https://pybind11.readthedocs.io/en/stable/) it exposes the JUCE framework api in a pythonic way, and the way it enables to write apps in python is very much similar to the way of writing them in C++ but without the overweight of managing project build, configurations and IDE solutions. it also allow embedding in exitsting JUCE apps and expose scripting to them via an embedded python interpreter.

--------
Features
--------

- Easy and quick to iterate over a JUCE application, no need to setup a build environment.
- The way it allows to write JUCE code is very similar to how you would write it in C++.
- Ships as wheel to be used in python scripts.
- Allows embedding in existing JUCE apps (ships as a JUCE module).

-------------
Example Usage
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
          g.fillAll(juce.Colours.black)

          random = juce.Random.getSystemRandom()
          rect = juce.Rectangle[int](0, 0, 20, 20)

          for _ in range(100):
              g.setColour(juce.Colour.fromRGBA(
                  random.nextInt(256),
                  random.nextInt(256),
                  random.nextInt(256),
                  255))

              rect.setCentre(random.nextInt(self.getWidth()), random.nextInt(self.getHeight()))
              g.drawRect(rect, 1)

      def timerCallback(self):
          if self.isVisible():
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
          if self.component:
              del self.component

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


  if __name__ == "__main__":
      juce.START_JUCE_APPLICATION(Application)

As easy as that ! You will find more example on JUCE usage in the *examples* folder.

-----------------
Supported Modules
-----------------

.. list-table:: List of popsicle supported JUCE modules
   :widths: 40 10 50
   :header-rows: 1

   * - Module
     - Support
     - Notes
   * - juce_analytics
     - ⛔️
     - Not Planned
   * - juce_audio_basics
     - ⛔️
     - Planned
   * - juce_audio_devices
     - ⛔️
     - Planned
   * - juce_audio_formats
     - ⛔️
     - Planned
   * - juce_audio_plugin_client
     - ⛔️
     - Not planned
   * - juce_audio_processors
     - ⛔️
     - Planned
   * - juce_audio_utils
     - ⛔️
     - Planned
   * - juce_box2d
     - ⛔️
     - Planned
   * - juce_core
     - ✅
     - Most Funcionality Available
   * - juce_cryptography
     - ⛔️
     - Planned
   * - juce_data_structures
     - ✅
     - Ready
   * - juce_dsp
     - ⛔️
     - Planned
   * - juce_events
     - ✅
     - Ready
   * - juce_graphics
     - ✅
     - In Progress
   * - juce_gui_basics
     - ✅
     - In Progress, Basic Support Available
   * - juce_gui_extra
     - ⛔️
     - Planned
   * - juce_midi_ci
     - ⛔️
     - Not Planned
   * - juce_opengl
     - ⛔️
     - Planned
   * - juce_osc
     - ⛔️
     - Not Planned
   * - juce_product_unlocking
     - ⛔️
     - Not Planned
   * - juce_video
     - ⛔️
     - Not planned

--------------------
Example Applications
--------------------

Some images of JUCE tutorials and other small apps ported to *popsicle*.

Super Simple Animated Graphics

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/juce_o_matic.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/juce_o_matic.py

Advanced GUI layout techniques (https://docs.juce.com/master/tutorial_rectangle_advanced.html)

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/layout_rectangles.png
    :target: https://github.com/kunitoki/popsicle/blob/master/examples/layout_rectangles.py

---------
Licensing
---------

It comes in two licensed flavours:

- **MIT License**: When used from python using the PyPi provided wheels or embedded in an OSS application
- **Commercial License**: When embedded in a closed source application

------------
Installation
------------

Installing popsicle is as easy as pulling from pypi (osx only for now):

.. code-block:: bash

  pip3 install popsicle

Make sure you have a recent *pip*.

-----------------
Build From Source
-----------------

Clone the repository recursively as JUCE is a submodule.

.. code-block:: bash

  git clone --recursive git@github.com:kunitoki/popsicle.git

Install python dependencies.

.. code-block:: bash

  # Build the binary distribution
  python -m build --wheel

  # Install the local wheel
  pip3 install dist/popsicle-*.whl


.. |linux_builds| image:: https://github.com/kunitoki/popsicle/workflows/Linux%20Builds/badge.svg
    :alt: Linux Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |macos_builds| image:: https://github.com/kunitoki/popsicle/workflows/macOS%20Builds/badge.svg
    :alt: macOS Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |windows_builds| image:: https://github.com/kunitoki/popsicle/workflows/Windows%20Builds/badge.svg
    :alt: Windows Builds Status
    :target: https://github.com/kunitoki/popsicle/actions

.. |commercial_license| image:: https://img.shields.io/badge/license-Commercial-blue
    :alt: Commercial License
    :target: https://github.com/kunitoki/popsicle/blob/master/COMMERCIAL

.. |pypi_license| image:: https://img.shields.io/pypi/l/popsicle
    :alt: PyPI - Open Source License
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
