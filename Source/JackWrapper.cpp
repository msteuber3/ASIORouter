/*
  ==============================================================================

    JackWrapper.cpp
    Created: 15 Apr 2025 2:20:54pm
    Author:  Michael

  ==============================================================================
*/

#include <JackWrapper.h>
#include <iostream>

JackWrapper::JackWrapper(const char* clientName) : juce::GroupComponent(), name(clientName)
{

    client = jack_client_open(clientName, JackNullOption, nullptr);
    if (!client)
    {
        DBG("Failed to open JACK client");
        return;
    }

    jackRouterOutputs.push_back(jack_port_register(client, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));

     createGUI();
}

JackWrapper::~JackWrapper()
{
    stop();
    if (client)
        jack_client_close(client);
}

void JackWrapper::createGUI()
{
    deviceLabel.setText(name, juce::dontSendNotification);
    deviceLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(deviceLabel);

    inputSectionLabel.setText("Inputs", juce::dontSendNotification);
    inputSectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    inputSectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inputSectionLabel);

    outputSectionLabel.setText("Outputs", juce::dontSendNotification);
    outputSectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    outputSectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputSectionLabel);

    for (int i = 0; i < jackRouterInputs.size(); i++) {
        inputChannels.add(std::make_unique<Channel>(i, "Input Channel " + juce::String(i)));
        addAndMakeVisible(inputChannels.getLast());
    }
    for (int i = 0; i < jackRouterOutputs.size(); i++) {
        outputChannels.add(std::make_unique<Channel>(i, "Output Channel " + juce::String(i)));
        addAndMakeVisible(outputChannels.getLast());
    }
}

bool JackWrapper::start()
{
    if (!client || jackRouterInputs.empty() || jackRouterOutputs.empty()) // Check for empty client
        return false;

    jack_set_process_callback(client, jackProcess, this); // Set the callback function

    if (jack_activate(client)) // activate client
    {
        DBG("Failed to activate JACK client");
        return false;
    }
/* Connect to system capture ports, we shall see. 
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
 */
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

void JackWrapper::setAudioCallback(JackAudioCallback* audioCallback) // Not sure what this is for atm
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

void JackWrapper::addChannel(std::shared_ptr<RingBuffer<float>> newBuffer, bool isInput, juce::String name)
{
    if (isInput) {
        inputBuffer.push_back(newBuffer);
        jackRouterInputs.push_back(jack_port_register(client, name.toRawUTF8(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0));
        inputChannels.add(std::make_unique<Channel>(0, name));
        addAndMakeVisible(inputChannels.getLast());
    
    }
    else {
        outputBuffer.push_back(newBuffer);
        jackRouterOutputs.push_back(jack_port_register(client, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
    }
}

jack_client_t* JackWrapper::getClient()
{
    if (client == nullptr) { jassert("Client does not exist yet"); }
    return client;
}



int JackWrapper::jackProcess(jack_nframes_t nframes, void* arg)
{
    auto* wrapper = static_cast<JackWrapper*>(arg);

    for (int i = 0; i < wrapper->jackRouterInputs.size(); i++) {
        {
            if (i >= wrapper->inputBuffer.size()) 
                break;

            auto* buffer = wrapper->inputBuffer[i].get();
            auto numSamples = buffer->getSize();

            auto processedData = wrapper->inputChannels[i]->process(buffer->getBuffer(), numSamples);

            float squaredSum = 0.0f;
            for (int i = 0; i < numSamples; ++i) {
                squaredSum += processedData[i] * processedData[i];
            }
            float rms = std::sqrt(squaredSum / numSamples);
            wrapper->inputChannels[i]->setRMSLevel(juce::Decibels::gainToDecibels(rms));

           buffer->write(processedData, numSamples);

            for (int j = 0; j < wrapper->jackRouterOutputs.size(); i++) {
                float* out = (float*)jack_port_get_buffer(wrapper->jackRouterOutputs[j], nframes);

                if (buffer)
                {
                    buffer->read(out, (int)nframes);
                }
                else
                {
                    std::memset(out, 0, sizeof(float) * nframes);
                }
                auto outData = wrapper->outputChannels[j]->process(out, nframes);
                float squaredSum = 0.0f;
                for (int i = 0; i < numSamples; ++i) {
                    squaredSum += outData[i] * outData[i];
                }
                float outRms = std::sqrt(squaredSum / numSamples);
                wrapper->outputChannels[i]->setRMSLevel(juce::Decibels::gainToDecibels(outRms));

                buffer->write(outData, numSamples);
            }
        }
    }
    return 0;
}
    /*
    auto* wrapper = static_cast<JackWrapper*>(arg);

    int numInChannels = wrapper->jackRouterInputs.size();
    int numOutChannels = wrapper->jackRouterOutputs.size(); // Thing to try: make ring buffers shared pointers that the channel->process block creates & updates
    float* inBuffer; // Alternativley: add channel method that adds shared pointers to ring buffers to a vector
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
*/

void JackWrapper::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    //JUCE GRIDS WOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    juce::Grid grid;

    // Define grid settings
    grid.rowGap = Px(10);
    grid.columnGap = Px(5);

    // Define some templates for our grid

    // Calculate number of columns needed: max of input and output channel counts
    const int numColumns = 2;

    // Setup rows (device name, inputs label, input channels, outputs label, output channels)
    grid.templateRows = {
        Track(Px(30)), // device name 
        Track(Px(20)), // Inputs label
        Track(Px(200)), //input channels
        Track(Px(20)), //outputs label
        Track(Px(200)) }; //output channels

    // Setup columns - one for each channel
    juce::Array<Track> columnTracks;
    for (int i = 0; i < numColumns; ++i)
        columnTracks.add(Track(Px(SLIDER_WIDTH)));

    grid.templateColumns = columnTracks;

    // Create a Grid Item for each component

    // Device label (spans all columns)
    auto deviceLabelItem = juce::GridItem(deviceLabel).withArea(1, 1, 2, numColumns + 1);
    grid.items.add(deviceLabelItem);

    // Input section label (spans all columns)
    auto inputLabelItem = juce::GridItem(inputSectionLabel).withArea(2, 1, 3, numColumns + 1);
    grid.items.add(inputLabelItem);

    // Input channels
    for (int i = 0; i < inputChannels.size(); i++) {
        auto channelItem = juce::GridItem(*inputChannels[i]).withArea(3, i + 1, 4, i + 2);
        grid.items.add(channelItem);
    }

    // Output section label (spans all columns)
    auto outputLabelItem = juce::GridItem(outputSectionLabel).withArea(4, 1, 5, numColumns + 1);
    grid.items.add(outputLabelItem);

    // Output channels
    for (int i = 0; i < outputChannels.size(); i++) {
        auto channelItem = juce::GridItem(*outputChannels[i]).withArea(5, i + 1, 6, i + 2);
        grid.items.add(channelItem);
    }

    // Perform layout
    grid.performLayout(bounds);
}