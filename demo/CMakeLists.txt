cmake_minimum_required (VERSION 3.21)

set (TARGET_NAME popsicle_demo)
get_filename_component (ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
file (STRINGS "${ROOT_PATH}/modules/juce_python/juce_python.h" JUCE_PYTHON_MODULE)
string (REGEX REPLACE "(.*)([0-9]+\.[0-9]+\.[0-9]+)(.*)" "\\2" VERSION_NUMBER ${JUCE_PYTHON_MODULE})
project (${TARGET_NAME} VERSION ${VERSION_NUMBER})

# Set browsable modules in IDE
set_property (GLOBAL PROPERTY USE_FOLDERS YES)
option (JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

# Fetch juce
add_subdirectory (${ROOT_PATH}/JUCE JUCE)

#include (FetchContent)
#set (FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
#FetchContent_Declare (JUCE
#    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
#    GIT_TAG origin/master
#    GIT_SHALLOW TRUE
#    GIT_PROGRESS TRUE
#    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
#FetchContent_MakeAvailable (JUCE)

# Add the popsicle modules
get_filename_component (MODULES_PATH "${ROOT_PATH}/modules" ABSOLUTE)
add_subdirectory (${MODULES_PATH} ./modules)

# Configure python (APPLE only for now)
set (Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
set (Python_USE_STATIC_LIBS TRUE)
find_package (Python REQUIRED Interpreter Development.Embed)

# Setup the juce app
juce_add_gui_app (${TARGET_NAME}
    PRODUCT_NAME "PopsicleDemo"
    VERSION "${VERSION_NUMBER}"
    BUNDLE_ID "org.kunitoki.popsicledemo")
juce_generate_juce_header (${TARGET_NAME})

# Add the binary target for the python standard library
add_custom_target (
    ${TARGET_NAME}_stdlib
    ${Python_EXECUTABLE} ${ROOT_PATH}/cmake/ArchivePythonStdlib.py
        -b ${Python_ROOT_DIR} -o ${CMAKE_CURRENT_BINARY_DIR} -M ${Python_VERSION_MAJOR} -m ${Python_VERSION_MINOR}
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.zip)
add_dependencies (${TARGET_NAME} ${TARGET_NAME}_stdlib)

juce_add_binary_data (BinaryData SOURCES "${CMAKE_CURRENT_BINARY_DIR}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.zip")
add_dependencies (BinaryData ${TARGET_NAME}_stdlib)

target_sources (${TARGET_NAME} PRIVATE
    Main.cpp
    PopsicleDemo.cpp
    PopsicleDemo.h)

set_target_properties (${TARGET_NAME} PROPERTIES JUCE_TARGET_KIND_STRING "App")
set_target_properties (${TARGET_NAME} PROPERTIES CXX_STANDARD 17)

if (APPLE)
    set_target_properties(${TARGET_NAME} PROPERTIES OSX_ARCHITECTURES "arm64;x86_64")
    target_link_options(${TARGET_NAME} PRIVATE "-Wl,-weak_reference_mismatches,weak")
    #set (LTO_CONFIGURATION "juce::juce_recommended_lto_flags")
    set (LTO_CONFIGURATION "")
else()
    set (LTO_CONFIGURATION "")
endif()

target_compile_definitions (${TARGET_NAME} PRIVATE
    JUCE_STANDALONE_APPLICATION=1
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_CATCH_UNHANDLED_EXCEPTIONS=0
    JUCE_LOG_ASSERTIONS=1
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_WEB_BROWSER=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
    PYBIND11_DETAILED_ERROR_MESSAGES=1)

target_link_libraries (${TARGET_NAME} PRIVATE
    #juce::juce_analytics
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    #juce::juce_cryptography
    juce::juce_data_structures
    #juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    #juce::juce_opengl
    #juce::juce_osc
    #juce::juce_video
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    Python::Python
    popsicle::juce_python
    popsicle::juce_python_recommended_warning_flags
    BinaryData
    ${LTO_CONFIGURATION})
