/**
 * juce_python - Python bindings for the JUCE framework.
 *
 * This file is part of the popsicle project.
 *
 * Copyright (c) 2022 - kunitoki <kunitoki@gmail.com>
 *
 * popsicle is an open source library subject to commercial or open-source licensing.
 *
 * By using popsicle, you agree to the terms of the popsicle License Agreement, which can
 * be found at https://raw.githubusercontent.com/kunitoki/popsicle/master/LICENSE
 *
 * Or: You may also use this code under the terms of the GPL v3 (see www.gnu.org/licenses).
 *
 * POPSICLE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER EXPRESSED
 * OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE DISCLAIMED.
 */

#include "JuceHeader.h"
#include "PopsicleDemo.h"

//=================================================================================================

class PopsicleApplication : public juce::JUCEApplication
{
public:
    PopsicleApplication() = default;

    const juce::String getApplicationName() override
    {
        return "PopsicleDemo";
    }

    const juce::String getApplicationVersion() override
    {
        return "1.0.0";
    }

    void initialise (const String&) override
    {
        mainWindow.reset (new MainWindow ("PopsicleDemo", new PopsicleDemo()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name, juce::Component* c)
            : juce::DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                          .findColour (juce::ResizableWindow::backgroundColourId),
                                    juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            juce::MessageManager::callAsync ([this]
            {
                juce::Process::makeForegroundProcess();

                setVisible (true);
                toFront (true);
            });
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//=================================================================================================

START_JUCE_APPLICATION (PopsicleApplication)
