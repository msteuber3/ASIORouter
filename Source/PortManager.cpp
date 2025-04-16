/*
  ==============================================================================

    PortManager.cpp
    Created: 15 Apr 2025 9:15:51pm
    Author:  Michael

  ==============================================================================
*/

#include "PortManager.h"

PortManager::PortManager(jack_client_t* client)
    : client(client)
{
    refreshSystemPorts();
    refreshAppPorts();
}

void PortManager::scanAsioDevicePorts()
{
    juce::StringArray availableDevices;
    auto asioDeviceType(juce::AudioIODeviceType::createAudioIODeviceType_ASIO());

    availableDevices = asioDeviceType->getDeviceNames();

}

juce::StringArray PortManager::getPorts(unsigned long flags) const
{
    juce::StringArray portNames;
    const char** ports = jack_get_ports(client, nullptr, nullptr, flags);
    if (ports)
    {
        for (int i = 0; ports[i]; ++i)
            portNames.add(ports[i]);

        jack_free(ports); // Don't leak!
    }
    return portNames;
}

void PortManager::refreshSystemPorts()
{
    systemInputPorts = getPorts(JackPortIsPhysical | JackPortIsOutput);  // System output = our input
    systemOutputPorts = getPorts(JackPortIsPhysical | JackPortIsInput);  // System input = our output
}

void PortManager::refreshAppPorts()
{
    appInputPorts = getPorts(JackPortIsInput);
    appOutputPorts = getPorts(JackPortIsOutput);
}

juce::StringArray PortManager::getSystemInputPorts() const 
{ 
    return systemInputPorts; 
}

juce::StringArray PortManager::getSystemOutputPorts() const 
{
    return systemOutputPorts; 
}

juce::StringArray PortManager::getAppInputPorts() const 
{
    return appInputPorts; 
}
juce::StringArray PortManager::getAppOutputPorts() const 
{
    return appOutputPorts; 
}

bool PortManager::connectPorts(const juce::String& source, const juce::String& destination)
{
    return (jack_connect(client, source.toRawUTF8(), destination.toRawUTF8()) == 0);
}

bool PortManager::disconnectPorts(const juce::String& source, const juce::String& destination)
{
    return (jack_disconnect(client, source.toRawUTF8(), destination.toRawUTF8()) == 0);
}

bool PortManager::isConnected(const juce::String& source, const juce::String& destination) const
{
    const char** connections = jack_port_get_all_connections(client, jack_port_by_name(client, source.toRawUTF8()));
    if (connections)
    {
        for (int i = 0; connections[i]; ++i)
        {
            if (destination == juce::String(connections[i]))
            {
                jack_free(connections);
                return true;
            }
        }
        jack_free(connections);
    }
    return false;
}