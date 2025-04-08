/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <MainComponent.h>
#include <AsioRouterApplication.h>


AsioRouterApplication::AsioRouterApplication() {}

const juce::String AsioRouterApplication::getApplicationName()
    { return ProjectInfo::projectName; }
const juce::String AsioRouterApplication::getApplicationVersion()
    { return ProjectInfo::versionString; }
bool AsioRouterApplication::moreThanOneInstanceAllowed()
    { return true; }

juce::JUCEApplication* AsioRouterApplication::getApp()
    { return juce::JUCEApplication::getInstance(); }


juce::AudioDeviceManager& AsioRouterApplication::getAudioDeviceManager()
    { return deviceManager; };
    //==============================================================================
void AsioRouterApplication::initialise (const juce::String& commandLine)
    {
        deviceManager.initialise(20, 2, nullptr, true);
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

void AsioRouterApplication::shutdown()
    {
        mainWindow = nullptr;
    }

    //==============================================================================
void AsioRouterApplication::systemRequestedQuit()
    {
        quit();
    }

void AsioRouterApplication::anotherInstanceStarted (const juce::String& commandLine) {}

    

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AsioRouterApplication)
