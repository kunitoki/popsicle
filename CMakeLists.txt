# ==============================================================================
#
#  This file is part of the popsicle project.
#  Copyright (c) 2020 - Lucio Asnaghi aka kRAkEn/gORe
#
#  You may use this code under the terms of the MIT (see www.gnu.org/licenses).
#
#  POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
#  WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE,
#  ARE DISCLAIMED.
#
# ==============================================================================

cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME popsicle)
project(${PROJECT_NAME})

add_subdirectory("JUCE")

function(juce_add_shared_library target)
    add_library(${target} SHARED)

    target_compile_definitions(${target} PRIVATE JUCE_STANDALONE_APPLICATION=1)
    _juce_initialise_target(${target} ${ARGN})
    _juce_set_output_name(${target} "$<TARGET_PROPERTY:${target},JUCE_PRODUCT_NAME>.so")
    set_target_properties(${target} PROPERTIES JUCE_TARGET_KIND_STRING "App")
endfunction()

juce_add_shared_library(${PROJECT_NAME}
    VERSION     1.0.0)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1)

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_analytics
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_cryptography
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_osc
    juce::juce_video
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
