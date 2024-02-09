
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

Now you are ready to rock ! Let's grab a more convoluted example:

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

