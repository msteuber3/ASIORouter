#pragma once

#include <MainMixer.h>
#include <RouterHeader.h>
#include <MenuBar.h>
#include <CustomListener.h>
#include <JuceHeader.h>

class MainComponent  : public juce::Component, public MainComponentListener
{
public:
    MainComponent();
    ~MainComponent() override;

    void initializeDeviceManager();

    void createMixer();
    
    void resized() override;

    void createGuiElements();
    
    void resetMixer();

    void mainComponentEventTriggered() override;

private:

    juce::FlexBox mainFlexBox;

    MenuBar menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;

    std::unique_ptr<MainMixer> mixer;
    
    juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
