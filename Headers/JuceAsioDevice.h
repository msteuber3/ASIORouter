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


class JuceAsioDevice : public juce::GroupComponent, public juce::AudioIODeviceCallback {
public:
    JuceAsioDevice(juce::AudioIODevice* juceIODevice);

    ~JuceAsioDevice() override;

    void createGUI();

    void createInputChannels(juce::StringArray channelNames);

    void createOutputChannels(juce::StringArray channelNames);

    std::unique_ptr<juce::AudioIODevice>& getAudioDevice();

    std::shared_ptr<RingBuffer<float>> getBuffer(bool isInput, int index);

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceStopped() override;

    int calculatePreferredWidth();

    juce::Point<float> getPreferredSize();

    void resized() override;

private:
    std::unique_ptr<juce::AudioIODevice> juceAudioDevice;

    std::map<int, std::unique_ptr<Channel>> inputChannelMap;
    std::map<int, std::unique_ptr<Channel>> outputChannelMap;

    std::vector< std::shared_ptr<RingBuffer<float>>> inputBuffers;
    std::vector< std::shared_ptr<RingBuffer<float>>> outputBuffers;

    juce::Label deviceLabel;
    juce::Label inputSectionLabel;
    juce::Label outputSectionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceAsioDevice)

};
