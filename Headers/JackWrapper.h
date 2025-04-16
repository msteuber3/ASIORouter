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
#include <atomic>
#include <functional>
#include <iostream>
#include <string>

class JackWrapper
{
public:
    JackWrapper(const char* clientName);
    ~JackWrapper();

    bool start();
    void stop();

    void setAudioCallback(JackAudioCallback* audioCallback);

    int getNumInputChannels() const { return jackRouterInputs.size(); }
    int getNumOutputChannels() const { return jackRouterOutputs.size(); }

    jack_client_t* getClient();

private:
    static int jackProcess(jack_nframes_t nframes, void* arg);

    std::vector<jack_port_t*> jackRouterInputs;
    std::vector<jack_port_t*> jackRouterOutputs;

    jack_client_t* client = nullptr;
    jack_port_t* inputPort = nullptr;
    jack_port_t* outputPort = nullptr;

    juce::Array<JackAudioCallback*> audioCallbacks;
    std::atomic<bool> running{ false };
};
