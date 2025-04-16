/*
  ==============================================================================

    PortManager.h
    Created: 15 Apr 2025 9:15:33pm
    Author:  Michael

  ==============================================================================
*/

#pragma once

#include <jack/jack.h>
#include <JuceHeader.h>
#include <RouterHeader.h>

class PortManager
{
public:
    PortManager(jack_client_t* client);

    void scanAsioDevicePorts();

    void refreshSystemPorts();
    void refreshAppPorts();

    juce::StringArray getSystemInputPorts() const;
    juce::StringArray getSystemOutputPorts() const;

    juce::StringArray getAppInputPorts() const;
    juce::StringArray getAppOutputPorts() const;

    bool connectPorts(const juce::String& source, const juce::String& destination);
    bool disconnectPorts(const juce::String& source, const juce::String& destination);
    bool isConnected(const juce::String& source, const juce::String& destination) const;

private:
    jack_client_t* client;

    juce::StringArray systemInputPorts;
    juce::StringArray systemOutputPorts;

    juce::StringArray appInputPorts;
    juce::StringArray appOutputPorts;

    juce::StringArray getPorts(unsigned long flags) const;
};