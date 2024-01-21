# ==============================================================================
#
#  This file is part of the JUCE library.
#  Copyright (c) 2022 - Raw Material Software Limited
#
#  JUCE is an open source library subject to commercial or open-source
#  licensing.
#
#  By using JUCE, you agree to the terms of both the JUCE 7 End-User License
#  Agreement and JUCE Privacy Policy.
#
#  End User License Agreement: www.juce.com/juce-7-licence
#  Privacy Policy: www.juce.com/juce-privacy-policy
#
#  Or: You may also use this code under the terms of the GPL v3 (see
#  www.gnu.org/licenses).
#
#  JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
#  EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
#  DISCLAIMED.
#
# ==============================================================================

add_library(juce_recommended_warning_flags INTERFACE)
add_library(juce::juce_recommended_warning_flags ALIAS juce_recommended_warning_flags)

function(_juce_get_debug_config_genex result)
    get_property(debug_configs GLOBAL PROPERTY DEBUG_CONFIGURATIONS)
    if(NOT debug_configs)
        set(debug_configs Debug)
    endif()
    list(TRANSFORM debug_configs REPLACE [[^.+$]] [[$<CONFIG:\0>]])
    list(JOIN debug_configs "," debug_configs)
    # $<CONFIG> doesn't accept multiple configurations until CMake 3.19
    set(${result} "$<OR:${debug_configs}>" PARENT_SCOPE)
endfunction()

# ==================================================================================================

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    target_compile_options(juce_recommended_warning_flags INTERFACE "/W4")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(juce_recommended_warning_flags INTERFACE
        -Wall
        -Wshadow-all
        -Wshorten-64-to-32
        -Wstrict-aliasing
        -Wuninitialized
        -Wunused-parameter
        -Wconversion
        -Wsign-compare
        -Wint-conversion
        -Wconditional-uninitialized
        -Wconstant-conversion
        -Wsign-conversion
        -Wbool-conversion
        -Wextra-semi
        -Wunreachable-code
        -Wcast-align
        -Wshift-sign-overflow
        -Wmissing-prototypes
        -Wnullable-to-nonnull-conversion
        -Wno-ignored-qualifiers
        -Wswitch-enum
        -Wpedantic
        -Wdeprecated
        -Wfloat-equal
        -Wmissing-field-initializers
        $<$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:OBJCXX>>:
            -Wzero-as-null-pointer-constant
            -Wunused-private-field
            -Woverloaded-virtual
            -Wreorder
            -Winconsistent-missing-destructor-override>
        $<$<OR:$<COMPILE_LANGUAGE:OBJC>,$<COMPILE_LANGUAGE:OBJCXX>>:
            -Wunguarded-availability
            -Wunguarded-availability-new>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(juce_recommended_warning_flags INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wstrict-aliasing
        -Wuninitialized
        -Wunused-parameter
        -Wsign-compare
        -Wsign-conversion
        -Wunreachable-code
        -Wcast-align
        -Wno-implicit-fallthrough
        -Wno-maybe-uninitialized
        -Wno-ignored-qualifiers
        -Wswitch-enum
        -Wredundant-decls
        -Wno-strict-overflow
        -Wshadow
        -Wfloat-equal
        -Wmissing-field-initializers
        $<$<COMPILE_LANGUAGE:CXX>:
            -Woverloaded-virtual
            -Wreorder
            -Wzero-as-null-pointer-constant>)
endif()

# ==================================================================================================

add_library(juce_recommended_config_flags INTERFACE)
add_library(juce::juce_recommended_config_flags ALIAS juce_recommended_config_flags)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    _juce_get_debug_config_genex(debug_config)
    target_compile_options(juce_recommended_config_flags INTERFACE
        $<IF:${debug_config},/Od /Zi,/Ox> $<$<STREQUAL:"${CMAKE_CXX_COMPILER_ID}","MSVC">:/MP> /EHsc)
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
       OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
       OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    _juce_get_debug_config_genex(debug_config)
    target_compile_options(juce_recommended_config_flags INTERFACE
        $<${debug_config}:-g -O0>
        $<$<CONFIG:Release>:-O3>)
endif()

# ==================================================================================================

add_library(juce_recommended_lto_flags INTERFACE)
add_library(juce::juce_recommended_lto_flags ALIAS juce_recommended_lto_flags)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    target_compile_options(juce_recommended_lto_flags INTERFACE
        $<$<CONFIG:Release>:$<IF:$<STREQUAL:"${CMAKE_CXX_COMPILER_ID}","MSVC">,-GL,-flto>>)
    target_link_libraries(juce_recommended_lto_flags INTERFACE
        $<$<CONFIG:Release>:$<$<STREQUAL:"${CMAKE_CXX_COMPILER_ID}","MSVC">:-LTCG>>)
elseif((NOT MINGW) AND ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                     OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
                     OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")))
    target_compile_options(juce_recommended_lto_flags INTERFACE $<$<CONFIG:Release>:-flto>)
    target_link_libraries(juce_recommended_lto_flags INTERFACE $<$<CONFIG:Release>:-flto>)
    # Xcode 15.0 requires this flag to avoid a compiler bug
    target_link_libraries(juce_recommended_lto_flags INTERFACE
        $<$<CONFIG:Release>:$<$<STREQUAL:"${CMAKE_CXX_COMPILER_ID}","AppleClang">:-Wl,-weak_reference_mismatches,weak>>)
endif()
