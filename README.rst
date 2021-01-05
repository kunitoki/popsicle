.. image:: https://github.com/kunitoki/popsicle/raw/master/logo.png
   :alt: popsicle
   :target: https://github.com/kunitoki/popsicle

========
popsicle
========

Popsicle is a project that aims to give JUCE a broader audience by allowing it to be used from python. By using *cppyy* (http://cppyy.readthedocs.io/en/latest/) it exposes the full JUCE api in a pythonic way, and the way it enables to write JUCE apps in python is very much similar to the way of writing them in C++.

.. image:: https://github.com/kunitoki/popsicle/workflows/Build%20Wheels/badge.svg
   :alt: status
   :target: https://github.com/kunitoki/popsicle/actions

-------------
Example usage
-------------

.. code-block:: python

  from popsicle import juce_gui_basics
  from popsicle import juce, juce_multi, START_JUCE_APPLICATION


  class MainContentComponent(juce_multi(juce.Component, juce.Timer)):
      def __init__(self):
          super().__init__((), ())
          self.setSize(600, 400)
          self.startTimerHz(60)

      def __del__(self):
          self.stopTimer()

      def paint(self, g):
          g.fillAll(juce.Colours.black)

          random = juce.Random.getSystemRandom()
          rect = juce.Rectangle[int](0, 0, 20, 20)

          for _ in range(100):
              g.setColour(juce.Colour(
                  random.nextInt(256),
                  random.nextInt(256),
                  random.nextInt(256)))

              rect.setCentre(random.nextInt(self.getWidth()), random.nextInt(self.getHeight()))
              g.drawRect(rect)

      def timerCallback(self):
          if self.isVisible():
              self.repaint()


  class MainWindow(juce.DocumentWindow):
      def __init__(self):
          super().__init__(
              juce.JUCEApplication.getInstance().getApplicationName(),
              juce.Colours.black,
              juce.DocumentWindow.allButtons,
              True)

          self.component = MainContentComponent()

          self.setResizable(True, True)
          self.setContentNonOwned(self.component, True)
          self.centreWithSize(800, 600)
          self.setVisible(True)

      def __del__(self):
          if hasattr(self, "component"):
              del self.component

      def closeButtonPressed(self):
          juce.JUCEApplication.getInstance().systemRequestedQuit()


  class Application(juce.JUCEApplication):
      def getApplicationName(self):
          return "JUCE-o-matic"

      def getApplicationVersion(self):
          return "1.0"

      def initialise(self, commandLine):
          self.window = MainWindow()

      def shutdown(self):
          if hasattr(self, "window"):
              del self.window


  if __name__ == "__main__":
      START_JUCE_APPLICATION(Application)

As easy as that ! You will find more example on JUCE usage in the *examples* folder.

--------------------
Example Applications
--------------------

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/animated_component.png
   :target: https://github.com/kunitoki/popsicle/blob/master/examples/animated_component.py

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/juce_o_matic.png
   :target: https://github.com/kunitoki/popsicle/blob/master/examples/juce_o_matic.py

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/audio_player_waveform.png
   :target: https://github.com/kunitoki/popsicle/blob/master/examples/audio_player_wave_cpp.py

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/layout_flexgrid.png
   :target: https://github.com/kunitoki/popsicle/blob/master/examples/layout_flexgrid.py

.. image:: https://github.com/kunitoki/popsicle/raw/master/images/layout_rectangles.png
   :target: https://github.com/kunitoki/popsicle/blob/master/examples/layout_rectangles.py

------------
Installation
------------

At the moment the wheels are not published to pypi so you will need to indepently install the dependencies.

.. code-block:: bash

  pip install cppyy>=1.9.1

Then build popsicle from source.

-----------------
Build From Source
-----------------

First step is to build JUCE. Step into the popsicle directory, then issue the following commands (or you can use the *build_juce.sh* script):

.. code-block:: bash

  rm -Rf cxxbuild
  mkdir -p cxxbuild

  pushd cxxbuild
  cmake -G "Ninja Multi-Config"
  cmake --build . --config Release
  popd

Then it's possible to package a wheel and install it (currently this is only tested on macOS and Linux):

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
