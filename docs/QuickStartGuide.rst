
===========================
Popsicle: Quick Start Guide
===========================

Popsicle is a groundbreaking project designed to extend the accessibility of `JUCE <https://juce.com/>`_ by seamlessly integrating it with Python. Leveraging the power of `pybind11 <https://pybind11.readthedocs.io/en/stable/>`_, Popsicle offers a Pythonic interface to the JUCE framework. This integration allows developers to utilize JUCE in a manner similar to using Qt with PySide, offering a simplified yet robust approach.


--------------------------
Using the wheels in Python
--------------------------

In order to use popsicle is necessary to install the wheels from pypi.

.. code-block:: bash

  pip3 install popsicle


-----------------------------
Importing the popsicle module
-----------------------------

To make sure everything is setup correctly, execute the following command:

.. code-block:: bash

  python3 -c "from popsicle import juce; print(juce.SystemStats.getJUCEVersion())"

Which should print something like:

.. code-block:: bash

  JUCE v7.0.9

Now you are ready to rock !


------------------------------
First hands on of JUCE classes
------------------------------

Let's grab a more convoluted example:

.. code-block:: python

  import popsicle as juce

  el = juce.XmlElement("TEST")
  el.setAttribute("property1", "100")
  el.setAttribute("property2", "hallo")
  el.setAttribute("property3", "1")

  os = juce.MemoryOutputStream()

  el.writeTo(os)

  print(os.toString())

Which invoked should print:

.. code-block:: bash

  <?xml version="1.0" encoding="UTF-8"?>

  <TEST property1="100" property2="hallo" property3="1"/>


--------------------------
Our first JUCE application
--------------------------

Now we would like to start a proper JUCE application with the Message Thread running and all the JUCE bell and whistles, but let's start with having our first JUCEApplication subclass:

.. code-block:: python

  import popsicle as juce

  class MyFirstPopsicleApp(juce.JUCEApplication):
    def __init__(self):
      juce.JUCEApplication.__init__(self)

    def getApplicationName(self):
      return "My First Popsicle App"

    def getApplicationVersion(self):
      return "1.0"

    def initialise(self, commandLineParameters):
      print("We were called with:", commandLineParameters)
      juce.MessageManager.callAsync(lambda: self.systemRequestedQuit())

    def shutdown(self):
      print("We were told to shutdown")

Now that we have our application, we need to tell just to use it, let's add this at the end of the file:

.. code-block:: python
  if __name__ == "__main__":
    juce.START_JUCE_APPLICATION(MyFirstPopsicleApp)

At this point, after saving the file (called `first_app.py` or whatever it suit best), when executed like this:

.. code-block:: bash

  python first_app.py 1 2 3 "test"

Will start and immediately quit, producing this output:

.. code-block:: bash

  We were called with: 1 2 3 test
  We were told to shutdown

---------------
Adding a window
---------------

In order to show something on screen, we need to build a window with an empty component into it and show it.

.. code-block:: python

  import popsicle as juce

  class MyFirstPopsicleComponent(juce.Component):
    def __init__(self):
      juce.Component.__init__(self)
      self.setSize(600, 400)
      self.setVisible(True)

    def paint(self, g):
      g.fillAll(juce.Colours.red)

  class MyFirstPopsicleWindow(juce.DocumentWindow):
    component = None

    def __init__(self):
      juce.DocumentWindow.__init__(
        self,
        juce.JUCEApplication.getInstance().getApplicationName(),
        juce.Desktop.getInstance().getDefaultLookAndFeel()
          .findColour(juce.ResizableWindow.backgroundColourId),
        juce.DocumentWindow.allButtons,
        True)

        self.component = MyFirstPopsicleComponent()

        self.setResizable(True, True)
        self.setContentNonOwned(self.component, True)
        self.centreWithSize(self.component.getWidth(), self.component.getHeight() + self.getTitleBarHeight())
        self.setVisible(True)

    def __del__(self):
      self.clearContentComponent()

      if self.component:
        self.component.setVisible(False)
        del self.component

    def closeButtonPressed(self):
      juce.JUCEApplication.getInstance().systemRequestedQuit()

Then let's add it to the application:

.. code-block:: python

  class MyFirstPopsicleApp(juce.JUCEApplication):
    window = None

    def __init__(self):
      juce.JUCEApplication.__init__(self)

    def getApplicationName(self):
      return "My First Popsicle App"

    def getApplicationVersion(self):
      return "1.0"

    def initialise(self, commandLineParameters):
      self.window = MyFirstPopsicleWindow()

      juce.MessageManager.callAsync(lambda: juce.Process.makeForegroundProcess())

    def shutdown(self):
      if self.window:
        del self.window

At this point launching the app will show a window with a red component inside. Great progress !
