/*
  ==============================================================================

    JackRouterManager.h
    Created: 15 Apr 2025 9:47:21pm
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <jack/jack.h>
#include <JuceHeader.h>
#include <JackAudioCallback.h>
#include <RouterHeader.h>
#include <Channel.h>
#include <iostream>
#include <vector>
#include <string>

class JackDeviceBridge : public juce::Component, public juce::AudioIODeviceCallback{
public:
    // Initialize with a JUCE device
    JackDeviceBridge(juce::AudioIODevice* juceAudioDevice, const char* jackClientName, JackAudioCallback *jackCallback, juce::AudioIODeviceCallback *juceCallback);

    ~JackDeviceBridge() override;

    void createInputs(juce::StringArray channelNames);

    void createOutputs(juce::StringArray channelNames);

    static int jackProcessCallback(jack_nframes_t nframes, void* arg);

    int processJack(jack_nframes_t nframes);

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
    const char* clientName;
    juce::String deviceName;

    jack_client_t* jackClient;
    std::unique_ptr<juce::AudioIODevice> juceAudioDevice;
    
    std::unique_ptr<JackAudioCallback> jackPortCallback;
    std::unique_ptr<juce::AudioIODeviceCallback> juceDeviceCallback;

    std::map<int, jack_port_t*> inputPortMap;  // Maps JUCE channel to JACK port
    std::map<int, jack_port_t*> outputPortMap;

    std::map<int, Channel*> inputChannelMap;
    std::map<int, Channel*> outputChannelMap;

    // Ring buffers to transfer data between JACK and JUCE callbacks
    std::map<int, RingBuffer<float>> inputBuffers;
    std::map<int, RingBuffer<float>> outputBuffers;

    juce::Component inputComponent;
    juce::Component outputComponent;
    juce::FlexBox inputBox;
    juce::FlexBox outputBox;
    juce::FlexBox deviceBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JackDeviceBridge)

};
