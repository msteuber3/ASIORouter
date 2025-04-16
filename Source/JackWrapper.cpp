/*
  ==============================================================================

    JackWrapper.cpp
    Created: 15 Apr 2025 2:20:54pm
    Author:  Michael

  ==============================================================================
*/

#include <JackWrapper.h>
#include <iostream>

JackWrapper::JackWrapper(const char* clientName)
{

    client = jack_client_open(clientName, JackNullOption, nullptr);
    if (!client)
    {
        std::cerr << "Failed to open JACK client" << std::endl;
        return;
    }

    const char** sysInputs = jack_get_ports(client, nullptr, nullptr, JackPortIsPhysical | JackPortIsInput);
    int systemInCount = 0;
    while (sysInputs && sysInputs[systemInCount]) ++systemInCount;

    const char** sysOutputs = jack_get_ports(client, nullptr, nullptr, JackPortIsPhysical | JackPortIsOutput);
    int systemOutCount = 0;
    while (sysOutputs && sysOutputs[systemOutCount]) ++systemOutCount;

    for (int i = 0; i < systemInCount; ++i)
    {
        std::string name = "input_" + std::to_string(i + 1);
        jackRouterInputs.push_back(jack_port_register(client, name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0));
    }
   
    // Register output ports
    for (int i = 0; i < systemOutCount; ++i)
    {
        std::string name = "output_" + std::to_string(i + 1);
        jackRouterOutputs.push_back(jack_port_register(client, name.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
    }
}

JackWrapper::~JackWrapper()
{
    stop();
    if (client)
        jack_client_close(client);
}

bool JackWrapper::start()
{
    if (!client || jackRouterInputs.empty() || jackRouterOutputs.empty())
        return false;

    jack_set_process_callback(client, jackProcess, this);

    if (jack_activate(client))
    {
        std::cerr << "Failed to activate JACK client" << std::endl;
        return false;
    }

    const char** systemCapturePorts = jack_get_ports(client, nullptr, nullptr,
        JackPortIsPhysical | JackPortIsOutput);
    const char** systemPlaybackPorts = jack_get_ports(client, nullptr, nullptr,
        JackPortIsPhysical | JackPortIsInput);

    for (size_t i = 0; systemCapturePorts[i] && i < jackRouterInputs.size(); ++i)
    {
        jack_connect(client, systemCapturePorts[i], jack_port_name(jackRouterInputs[i]));
    }

    for (size_t i = 0; systemPlaybackPorts[i] && i < jackRouterOutputs.size(); ++i)
    {
        jack_connect(client, jack_port_name(jackRouterOutputs[i]), systemPlaybackPorts[i]);
    }
 
    running = true;

    return true;
}

void JackWrapper::stop()
{
    if (running && client)
    {
        jack_deactivate(client);
        running = false;
    }
}

void JackWrapper::setAudioCallback(JackAudioCallback* audioCallback)
{
    audioCallbacks.clear(); // Maybe get rid of this, might want multiple callbacks in the future
    juce::StringArray inputNames;
    juce::StringArray outputNames;
    juce::Array<int> inputIds;
    juce::Array<int> outputIds;
    int id = 0;
    for (jack_port_t* inPort : jackRouterInputs) {
        inputNames.add(jack_port_name(inPort));
        inputIds.add(id + 100);
        id++;
    }
    id = 0;
    for (jack_port_t* outPort : jackRouterOutputs) {
        outputNames.add(jack_port_name(outPort));
        outputIds.add(id + 200);
        id++;
    }

    audioCallback->setupCallback(inputNames, outputNames, inputIds, outputIds);
    audioCallbacks.add(audioCallback);
}

jack_client_t* JackWrapper::getClient()
{
    if (client == nullptr) { jassert("Client does not exist yet"); }
    return client;
}


int JackWrapper::jackProcess(jack_nframes_t nframes, void* arg)
{
    auto* wrapper = static_cast<JackWrapper*>(arg);

    int numInChannels = wrapper->jackRouterInputs.size();
    int numOutChannels = wrapper->jackRouterOutputs.size();
    float* inBuffer;
    float* outBuffer;

    juce::AudioBuffer<float> inputBuffer;
    inputBuffer.setSize(numInChannels, (int)nframes);
    juce::AudioBuffer<float> outputBuffer;
    outputBuffer.setSize(numOutChannels, (int)nframes);

    for (size_t i = 0; i < wrapper->jackRouterInputs.size(); ++i) {
        inBuffer = static_cast<float*>(jack_port_get_buffer(wrapper->jackRouterInputs[i], nframes));
        inputBuffer.copyFrom((int)i, 0, inBuffer, (int)nframes);
    }
    for (size_t i = 0; i < wrapper->jackRouterOutputs.size(); ++i) {     // In the future, this will likely be retrieved from juce and i'll need to copy the juce buffer
        outBuffer = static_cast<float*>(jack_port_get_buffer(wrapper->jackRouterOutputs[i], nframes)); //onto the jack outputbuffer
        outputBuffer.copyFrom((int)i, 0, outBuffer, (int)nframes);
    }

    if (!(wrapper->audioCallbacks.isEmpty())) {
        wrapper->audioCallbacks[0]->audioCallback(inputBuffer, numInChannels, inputBuffer, numOutChannels, static_cast<int>(nframes));
    }
    return 0;
}

