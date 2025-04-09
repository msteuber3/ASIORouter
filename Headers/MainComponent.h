#pragma once

#include <MainMixer.h>
#include <RouterHeader.h>
#include <MenuBar.h>
#include <JuceHeader.h>

class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    // To this, add the following (maybe): AudioProcessorGraph of InputDevice channel nodes
    
    void resized() override;

    void createGuiElements();

private:
    void initializeMenu();

    juce::FlexBox mainFlexBox;
    
    juce::StringArray inDeviceNames;
    juce::StringArray outDeviceNames;

    MenuBar menuModel;
    std::unique_ptr<juce::MenuBarComponent> menuBar;

    std::vector<std::unique_ptr<AudioInputDevice>> inputDevices;

    std::unique_ptr<juce::AudioProcessorGraph> outputGraph;
    std::unique_ptr<MainMixer> mixer;
    
    juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
