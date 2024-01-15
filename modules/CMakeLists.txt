juce_add_modules(juce_python)

add_library(juce_python_recommended_warning_flags INTERFACE)
if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
    #target_compile_options(juce_python_recommended_warning_flags INTERFACE "/W4")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(juce_python_recommended_warning_flags INTERFACE
        -Wno-shadow-field-in-constructor
        -Wno-shadow-field
        -Wno-shadow-uncaptured-local
        -Wno-shadow
        -Wno-self-assign-overloaded
        -Wno-zero-as-null-pointer-constant
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-float-conversion
        -Wno-float-equal)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(juce_python_recommended_warning_flags INTERFACE
        -Wno-shadow
        -Wno-zero-as-null-pointer-constant
        -Wno-redundant-decls
        -Wno-float-equal)
endif()

add_library(popsicle::juce_python ALIAS juce_python)
add_library(popsicle::juce_python_recommended_warning_flags ALIAS juce_python_recommended_warning_flags)
