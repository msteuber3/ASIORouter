/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/
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

    //==============================================================================
void AsioRouterApplication::initialise (const juce::String& commandLine)
    {
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

AsioRouterApplication::MainWindow::MainWindow (juce::String name)
    : DocumentWindow (name,
                      juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                  .findColour (juce::ResizableWindow::backgroundColourId),
                      DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (new MainComponent(), true);

   #if JUCE_IOS || JUCE_ANDROID
    setFullScreen (true);
   #else
    setResizable (true, true);
    centreWithSize (getWidth(), getHeight());
   #endif

    setVisible (true);
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AsioRouterApplication)
