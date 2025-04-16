/*
  ==============================================================================

    JuceAsioDevice.h
    Created: 16 Apr 2025 7:41:16am
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <RouterHeader.h>
#include <RingBuffer.h>
#include <Channel.h>


class JuceAsioDevice : public juce::Component, public juce::AudioIODeviceCallback {
public:
    JuceAsioDevice(juce::AudioIODevice* juceIODevice);

    ~JuceAsioDevice() override;

    void createInputChannels(juce::StringArray channelNames);

    void createOutputChannels(juce::StringArray channelNames);

    void createGUI();

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceStopped() override;

    void resized() override;

private:
    juce::String deviceName;

    std::unique_ptr<juce::AudioIODevice> juceAudioDevice;

    std::map<int, std::unique_ptr<Channel>> inputChannelMap;
    std::map<int, std::unique_ptr<Channel>> outputChannelMap;

    std::vector< std::unique_ptr<RingBuffer<float>>> inputBuffers;
    std::vector< std::unique_ptr<RingBuffer<float>>> outputBuffers;

    juce::Component inputComponent;
    juce::Component outputComponent;
    juce::FlexBox inputBox;
    juce::FlexBox outputBox;
    juce::FlexBox deviceBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceAsioDevice)

};
