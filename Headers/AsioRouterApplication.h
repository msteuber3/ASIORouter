//
//  AsioRouterApplication.h
//  AsioRouter - App
//
//  Created by Michael Steuber on 4/8/25.
//
#pragma once
#include <JuceHeader.h>
#include <MainComponent.h>

class AsioRouterApplication : public juce::JUCEApplication
{
public:
    AsioRouterApplication();
    
    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;
    
    void initialise(const juce::String& commandLine) override;
    void shutdown() override;
    
    static JUCEApplication* getApp();
        
    void anotherInstanceStarted (const juce::String& commandLine) override;
    
    void systemRequestedQuit() override;
        
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name);
           

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};
