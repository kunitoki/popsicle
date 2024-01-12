/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
 */

#pragma once

#include "JuceHeader.h"

//=================================================================================================

class PopsicleDemo : public juce::Component
{
public:
    PopsicleDemo();
    ~PopsicleDemo() override;

    // juce::Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::String text;

private:
    popsicle::ScriptEngine engine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopsicleDemo)
};
