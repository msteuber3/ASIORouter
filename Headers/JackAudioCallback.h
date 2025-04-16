/*
  ==============================================================================

    JackAudioCallback.h
    Created: 15 Apr 2025 5:33:13pm
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class JackAudioCallback : juce::AudioIODeviceCallback {
public:
    virtual ~JackAudioCallback() = default;
    virtual void setupCallback(juce::StringArray inputChannelNames, juce::StringArray outputChannelNames, juce::Array<int> inputChannelIds, juce::Array<int> outputChannelIds) = 0;
    virtual void audioCallback(juce::AudioBuffer<float> in, int numInChannels, juce::AudioBuffer<float> out, int numOutChannels, int numSamples) = 0;
};