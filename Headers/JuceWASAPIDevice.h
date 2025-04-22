/*
  ==============================================================================

    JuceWASPIDevice.h
    Created: 17 Apr 2025 10:24:19pm
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <RouterHeader.h>
#include <RingBuffer.h>
#include <Channel.h>


class JuceWASAPIDevice : public juce::GroupComponent, public juce::AudioIODeviceCallback {
public:
    JuceWASAPIDevice(juce::AudioIODevice* juceIODevice, bool isInput);
    ~JuceWASAPIDevice() override;

    void createGUI();

    juce::String getFullDeviceName();

    juce::String getShortDeviceName();

    void renameDevice(juce::String newName);

    void createChannels(juce::StringArray channelNames);

    std::unique_ptr<juce::AudioIODevice>& getAudioDevice();

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
        int numInputChannels,
        float* const* outputChannelData,
        int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceStopped() override;

    int calculatePreferredWidth();

    int getNumChannels();

    juce::Point<float> getPreferredSize();

    void resized() override;

private:
    bool isInput;

    juce::String name;

    std::unique_ptr<juce::AudioIODevice> juceAudioDevice;

    std::map<int, std::unique_ptr<Channel>> channelMap;

    std::vector< std::unique_ptr<RingBuffer<float>>> buffers;

    juce::Label deviceLabel;
 
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceWASAPIDevice)

};
