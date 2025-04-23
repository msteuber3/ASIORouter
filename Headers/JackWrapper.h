/*
  ==============================================================================

    JackWrapper.h
    Created: 15 Apr 2025 2:20:36pm
    Author:  Michael

  ==============================================================================
*/

#pragma once

#include <jack/jack.h>
#include <JuceHeader.h>
#include <JackAudioCallback.h>
#include <RouterHeader.h>
#include <RingBuffer.h>
#include <Channel.h>
#include <atomic>
#include <functional>
#include <iostream>
#include <string>

class JackWrapper : public juce::GroupComponent
{
public:
    JackWrapper(const char* clientName);
    ~JackWrapper();

    void createGUI();

    bool start();
    void stop();

    void setAudioCallback(JackAudioCallback* audioCallback);

    void addChannel(std::shared_ptr<RingBuffer<float>> newBuffer, bool isInput, juce::String name);

    int getNumInputChannels() const { return jackRouterInputs.size(); }
    int getNumOutputChannels() const { return jackRouterOutputs.size(); }

    jack_client_t* getClient();

    void resized() override;

private:
    static int jackProcess(jack_nframes_t nframes, void* arg);

    const char* name;

    std::vector<jack_port_t*> jackRouterInputs;
    std::vector<jack_port_t*> jackRouterOutputs;

    juce::OwnedArray<Channel>  inputChannels;
    juce::OwnedArray<Channel>  outputChannels;

    jack_client_t* client = nullptr;
    jack_port_t* inputPort = nullptr;
    jack_port_t* outputPort = nullptr;

    std::vector<std::shared_ptr<RingBuffer<float>>> inputBuffer;
    std::vector<std::shared_ptr<RingBuffer<float>>> outputBuffer;

    juce::Label deviceLabel;
    juce::Label inputSectionLabel;
    juce::Label outputSectionLabel;

    juce::Array<JackAudioCallback*> audioCallbacks;
    std::atomic<bool> running{ false };
};
