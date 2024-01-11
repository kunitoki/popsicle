/**
 * popsicle - Copyright (c) 2024, Lucio Asnaghi. All rights reserved.
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

private:
    jucepy::ScriptEngine engine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopsicleDemo)
};
