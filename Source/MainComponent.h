#pragma once

#include <JuceHeader.h>
#include "Channel.h"
#include "AudioInputDevice.h"


class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    // To this, add the following (maybe): AudioProcessorGraph of InputDevice channel nodes
    
    void resized() override;

    void createGuiElements();

private:
    void changeAudioDriver();

    void ScanCurrentDriver();

    void initializeMenu();

    
    std::vector<std::unique_ptr<Channel>> channels;

    juce::AudioDeviceManager mainDeviceManager;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetting;
    juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;
    juce::ComboBox audioDrivers;
    juce::StringArray inDeviceNames;
    juce::StringArray outDeviceNames;


    std::vector<std::unique_ptr<AudioInputDevice>> inputDevices;

    juce::AudioProcessorGraph outputGraph;
 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
