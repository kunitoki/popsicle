cmake_minimum_required (VERSION 3.21)

set (PROJECT_NAME popsicle)
get_filename_component (ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
file (STRINGS "${ROOT_PATH}/VERSION" VERSION_NUMBER)
project (${PROJECT_NAME} VERSION ${VERSION_NUMBER})

include (FetchContent)
set (FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Set browsable modules in IDE
set_property (GLOBAL PROPERTY USE_FOLDERS YES)
option (JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

# Fetch juce
FetchContent_Declare (JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
FetchContent_MakeAvailable (JUCE)

# Find python
if ("${Python_INCLUDE_DIRS}" STREQUAL "")
    set (Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
    set (Python_USE_STATIC_LIBS TRUE)
    find_package (Python REQUIRED Development)
else()
    message(STATUS "${PROJECT_NAME} - Using python includes (provided): ${Python_INCLUDE_DIRS}")
endif()

# Add the modules
get_filename_component (MODULES_PATH "${ROOT_PATH}/modules" ABSOLUTE)
add_subdirectory (${MODULES_PATH} ./modules)

# Configure the library
add_library (${PROJECT_NAME} SHARED)
set_target_properties (${PROJECT_NAME} PROPERTIES JUCE_TARGET_KIND_STRING "App")
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties (${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET "hidden")
set_target_properties (${PROJECT_NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN TRUE)
set_target_properties (${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties (${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties (${PROJECT_NAME} PROPERTIES PREFIX "")

if (APPLE)
    set_target_properties (${PROJECT_NAME} PROPERTIES OSX_ARCHITECTURES "arm64;x86_64")
    set_target_properties (${PROJECT_NAME} PROPERTIES OSX_DEPLOYMENT_TARGET "10.15")
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    set_target_properties (${PROJECT_NAME} PROPERTIES SUFFIX ".so")
    target_link_options (${PROJECT_NAME} PRIVATE "-Wl,-weak_reference_mismatches,weak")
    set (LTO_CONFIGURATION "juce::juce_recommended_lto_flags")
elseif (WIN32)
    target_compile_definitions (${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:Py_DEBUG>)
    set_target_properties (${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties (${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_d")
    endif()
    set (LTO_CONFIGURATION "")
endif()

target_include_directories (${PROJECT_NAME} PRIVATE "${Python_INCLUDE_DIRS}")

if (NOT "${Python_LIBRARY_DIRS}" STREQUAL "")
    target_link_directories (${PROJECT_NAME} PRIVATE "${Python_LIBRARY_DIRS}")
endif()

target_compile_definitions (${PROJECT_NAME} PRIVATE
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
    JUCE_PYTHON_EMBEDDED_INTERPRETER=0
    PYBIND11_DETAILED_ERROR_MESSAGES=1)

target_link_libraries (${PROJECT_NAME} PRIVATE
    #juce::juce_analytics
    #juce::juce_audio_basics
    #juce::juce_audio_devices
    #juce::juce_audio_formats
    #juce::juce_audio_processors
    #juce::juce_audio_utils
    juce::juce_core
    #juce::juce_cryptography
    juce::juce_data_structures
    #juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    #juce::juce_gui_extra
    #juce::juce_opengl
    #juce::juce_osc
    #juce::juce_video
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    popsicle::juce_python
    popsicle::juce_python_recommended_warning_flags
    ${LTO_CONFIGURATION})

_juce_initialise_target (${PROJECT_NAME} VERSION "${VERSION_NUMBER}")
_juce_set_output_name (${PROJECT_NAME} $<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>)

#if (lto_supported)
#    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
#endif()
