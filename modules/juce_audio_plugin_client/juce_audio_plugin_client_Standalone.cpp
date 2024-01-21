/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include <juce_core/system/juce_TargetPlatform.h>

#if JucePlugin_Build_Standalone

#if ! JUCE_MODULE_AVAILABLE_juce_audio_utils
 #error To compile AudioUnitv3 and/or Standalone plug-ins, you need to add the juce_audio_utils and juce_audio_devices modules!
#endif

#include <juce_core/system/juce_TargetPlatform.h>
#include <juce_audio_plugin_client/detail/juce_CheckSettingMacros.h>

#include <juce_audio_plugin_client/detail/juce_IncludeSystemHeaders.h>
#include <juce_audio_plugin_client/detail/juce_IncludeModuleHeaders.h>
#include <juce_gui_basics/native/juce_WindowsHooks_windows.h>
#include <juce_audio_plugin_client/detail/juce_PluginUtilities.h>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

// You can set this flag in your build if you need to specify a different
// standalone JUCEApplication class for your app to use. If you don't
// set it then by default we'll just create a simple one as below.
#if ! JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP

#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace juce
{

//==============================================================================
class StandaloneFilterApp final : public JUCEApplication
{
public:
    StandaloneFilterApp()
    {
        PropertiesFile::Options options;

        options.applicationName     = appName;
        options.filenameSuffix      = ".settings";
        options.osxLibrarySubFolder = "Application Support";
       #if JUCE_LINUX || JUCE_BSD
        options.folderName          = "~/.config";
       #else
        options.folderName          = "";
       #endif

        appProperties.setStorageParameters (options);
    }

    const String getApplicationName() override              { return appName; }
    const String getApplicationVersion() override           { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override              { return true; }
    void anotherInstanceStarted (const String&) override    {}

    virtual StandaloneFilterWindow* createWindow()
    {
       #ifdef JucePlugin_PreferredChannelConfigurations
        StandalonePluginHolder::PluginInOuts channels[] = { JucePlugin_PreferredChannelConfigurations };
       #endif

        return new StandaloneFilterWindow (getApplicationName(),
                                           LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                                           appProperties.getUserSettings(),
                                           false, {}, nullptr
                                          #ifdef JucePlugin_PreferredChannelConfigurations
                                           , juce::Array<StandalonePluginHolder::PluginInOuts> (channels, juce::numElementsInArray (channels))
                                          #else
                                           , {}
                                          #endif
                                          #if JUCE_DONT_AUTO_OPEN_MIDI_DEVICES_ON_MOBILE
                                           , false
                                          #endif
                                           );
    }

    //==============================================================================
    void initialise (const String&) override
    {
        mainWindow.reset (createWindow());

       #if JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE
        Desktop::getInstance().setKioskModeComponent (mainWindow.get(), false);
       #endif

        mainWindow->setVisible (true);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        appProperties.saveIfNeeded();
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            mainWindow->pluginHolder->savePluginState();

        if (ModalComponentManager::getInstance()->cancelAllModalComponents())
        {
            Timer::callAfterDelay (100, []()
            {
                if (auto app = JUCEApplicationBase::getInstance())
                    app->systemRequestedQuit();
            });
        }
        else
        {
            quit();
        }
    }

protected:
    ApplicationProperties appProperties;
    std::unique_ptr<StandaloneFilterWindow> mainWindow;

private:
    const String appName { CharPointer_UTF8 (JucePlugin_Name) };
};

} // namespace juce

#if JucePlugin_Build_Standalone && JUCE_IOS

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wmissing-prototypes")

using namespace juce;

bool JUCE_CALLTYPE juce_isInterAppAudioConnected()
{
    if (auto holder = StandalonePluginHolder::getInstance())
        return holder->isInterAppAudioConnected();

    return false;
}

void JUCE_CALLTYPE juce_switchToHostApplication()
{
    if (auto holder = StandalonePluginHolder::getInstance())
        holder->switchToHostApplication();
}

Image JUCE_CALLTYPE juce_getIAAHostIcon (int size)
{
    if (auto holder = StandalonePluginHolder::getInstance())
        return holder->getIAAHostIcon (size);

    return Image();
}

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#endif

#endif

#if JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP
 extern juce::JUCEApplicationBase* juce_CreateApplication();

 #if JUCE_IOS
  extern void* juce_GetIOSCustomDelegateClass();
 #endif

#else
 JUCE_CREATE_APPLICATION_DEFINE (juce::StandaloneFilterApp)
#endif

#if ! JUCE_USE_CUSTOM_PLUGIN_STANDALONE_ENTRYPOINT
 JUCE_MAIN_FUNCTION_DEFINITION
#endif

#endif
