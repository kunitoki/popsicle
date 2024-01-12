/**
 * juce_python - Python bindings for the JUCE framework
 *
 * Copyright (c) 2024 - Lucio Asnaghi
 *
 * Licensed under the MIT License. Visit https://opensource.org/licenses/MIT for more information.
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
