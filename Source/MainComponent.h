#pragma once

#include <JuceHeader.h>
#include "Channel.h"


class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int, double) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void resized() override;


   // void paint (juce::Graphics&) override;

private:

    void initializeMenu();
    void createChannels();
    std::vector<std::unique_ptr<Channel>> channels;

    juce::AudioDeviceManager uniqueDeviceManager;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetting;

 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
