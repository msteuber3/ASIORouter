#pragma once

#include <MainMixer.h>
#include <RouterHeader.h>
#include <MenuBar.h>
#include <JuceHeader.h>

class MainComponent  : public juce::Component, public MenuBar::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void initializeDeviceManager();

    void createMixer();

    void enableDeviceSelectorComponent();
    
    void resized() override;

    void createGuiElements();

    void menuBarPropertiesWindowClosed() override;

private:
    bool deviceSelectorComponentActive = false;

    juce::AudioDeviceSelectorComponent *audioSettingsComp;

    juce::FlexBox mainFlexBox;

    MenuBar menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;

    MainMixer *mixer;
    
    juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
