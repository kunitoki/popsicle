=========================
Popsicle: Embedding Guide
=========================

TODO

--------------------------------------
Adding popsicle to the project (cmake)
--------------------------------------

When `cmake` is the preferred build system for a JUCE app, it's necessary to fetch the popsicle repository

- Adding an in-tree checked out repository of popsicle (as a non recursive submodule of your project):

   .. code-block:: cmake

     add_subdirectory (${CMAKE_CURRENT_LIST_DIR}/popsicle/modules)

- Adding an out-of-tree checked out repository of popsicle:

   .. code-block:: cmake

     add_subdirectory (${CMAKE_CURRENT_LIST_DIR}/../popsicle/modules popsicle)

- Using `FetchContent` to add popsicle to your project:

   .. code-block:: cmake

     include (FetchContent)
     set (FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
     FetchContent_Declare (popsicle
         GIT_REPOSITORY https://github.com/kunitoki/popsicle.git
         GIT_TAG origin/master # Eventually a tagged release
         GIT_SHALLOW TRUE
         GIT_PROGRESS TRUE
         SOURCE_SUBDIR modules
         SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/popsicle)
     FetchContent_MakeAvailable (popsicle)

Once this is done, it's also ncessary to find python interpreter and libraries to embed:

.. code-block:: cmake

  set (Python_USE_STATIC_LIBS TRUE)
  find_package (Python REQUIRED Interpreter Development.Embed)

If you installed python globally using the official upstream installers, it's possible to force their location as well to simplify the selection of the desired python version, for example in macOS:

.. code-block:: cmake

  set (Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
  set (Python_USE_STATIC_LIBS TRUE)
  find_package (Python REQUIRED Interpreter Development.Embed)

And add the necessary modules to the target (application or plugin):

.. code-block:: cmake

  target_link_libraries (${PROJECT_NAME} PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    # More juce modules here ...
    Python::Python                                    # << Add python library
    popsicle::juce_python                             # << Add popsicle juce_python
    popsicle::juce_python_recommended_warning_flags)  # << Add popsicle recommended warning flags

In order to be able to initialise correctly the embedded interpreter on the machine where the application will be deployed, we need to also zip and deploy the Python standard library or the interpreter bundled with the application will fail to initialise, requiring the final user to install the exact same python version.

In order to create a zip of the standard library we can add a new target which executes a script in `popsicle/cmake/ArchivePythonStdlib.py` (modify the path in case your location of popsicle is different):

.. code-block:: cmake

  set (ADDITIONAL_IGNORED_PYTHON_PATTERNS "lib2to3" "pydoc_data" "_xxtestfuzz*")
  set (PYTHON_STANDARD_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.zip")

  add_custom_target (
      ${PROJECT_NAME}_stdlib
      ${Python_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/popsicle/cmake/ArchivePythonStdlib.py
          -b ${Python_ROOT_DIR} -o ${CMAKE_CURRENT_BINARY_DIR} -M ${Python_VERSION_MAJOR} -m ${Python_VERSION_MINOR}
          -i "\"${ADDITIONAL_IGNORED_PYTHON_PATTERNS}\""
      BYPRODUCTS ${PYTHON_STANDARD_LIBRARY})
  add_dependencies (${PROJECT_NAME} ${PROJECT_NAME}_stdlib)

This will compile a zip file in `CMAKE_CURRENT_BINARY_DIR` with the Python's standard library that can be then copied when the app is installed.

The zip file can also be embedded in the binary by using the `juce_add_binary_data`:

.. code-block:: cmake

  juce_add_binary_data (BinaryData SOURCES ${PYTHON_STANDARD_LIBRARY})
  add_dependencies (BinaryData ${PROJECT_NAME}_stdlib)

  target_link_libraries (${PROJECT_NAME} PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    # More juce modules here ...
    BinaryData)

-----------------------------------------
Adding popsicle to the project (projucer)
-----------------------------------------

TODO


----------------------------------
Boostrapping the interpreter (C++)
----------------------------------

TODO
