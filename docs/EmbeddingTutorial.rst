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

Once this is done, it's also ncessary to find python libraries to embed:

.. code-block:: cmake

  set (Python_USE_STATIC_LIBS TRUE)
  find_package (Python REQUIRED Development.Embed)

If you installed python globally using the official upstream installers, it's possible to force their location as well to simplify the selection of the desired python version, for example in macOS:

.. code-block:: cmake

  set (Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
  set (Python_USE_STATIC_LIBS TRUE)
  find_package (Python REQUIRED Development.Embed)

And finally adding the necessary targets:

.. code-block:: cmake

  target_link_libraries (${TARGET_NAME} PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    # More juce modules here ...
    Python::Python                                    # << Add python library
    popsicle::juce_python                             # << Add popsicle juce_python
    popsicle::juce_python_recommended_warning_flags)  # << Add popsicle recommended warning flags


-----------------------------------------
Adding popsicle to the project (projucer)
-----------------------------------------

TODO
