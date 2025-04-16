/*
  ==============================================================================

    JackRouterManager.cpp
    Created: 15 Apr 2025 9:47:37pm
    Author:  Michael

  ==============================================================================
*/

#include "JackJuceBridge.h"

JackDeviceBridge::JackDeviceBridge(juce::AudioIODevice* juceAudioDevice, jack_client_t* jackClient, const char* jackClientName) :
    juce::Component(),
    juce::AudioIODeviceCallback(),
    clientName(jackClientName),
    deviceName(juceAudioDevice->getName()),
    juceAudioDevice(juceAudioDevice),
    jackClient(jackClient)
{
    if (jackClient == nullptr) {
        DBG("Failed to create JACK client " + juce::String(jackClientName));
        return;
    }

    if (juceAudioDevice) {
        auto inputChannels = juceAudioDevice->getInputChannelNames();
        auto outputChannels = juceAudioDevice->getOutputChannelNames();

        if(!inputChannels.isEmpty()) 
            createInputBridges(inputChannels);

        if (!outputChannels.isEmpty())
            createOutputs(outputChannels);
        
        // Set JACK process callback
        jack_set_process_callback(jackClient, jackProcessCallback, this);

        // Start JUCE device
        juceAudioDevice->open(inputChannels.size(), outputChannels.size(),
            juceAudioDevice->getCurrentSampleRate(),
            juceAudioDevice->getCurrentBufferSizeSamples());
        juceAudioDevice->start(this);

        // Activate JACK client
        if (jack_activate(jackClient) == 0) {
            DBG("JACK client " + juce::String(jackClientName) + " activated successfully");
        }
    }
}

JackDeviceBridge::JackDeviceBridge(juce::AudioIODevice* juceAudioDevice)
{}

JackDeviceBridge::~JackDeviceBridge() {
    if (juceAudioDevice) {
        juceAudioDevice->stop();
        juceAudioDevice->close();
    }

    if (jackClient) {
        jack_deactivate(jackClient);
        jack_client_close(jackClient);
    }
}

void JackDeviceBridge::createInputBridges(juce::StringArray channelNames)
{
    inputBuffers.reserve(channelNames.size());

    for (int inputId = 0; inputId < channelNames.size(); ++inputId) {
        juce::String portName = channelNames[inputId] + "_in_" + juce::String(inputId);
        auto port = jack_port_register(jackClient,
            portName.toRawUTF8(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput,
            0);
        inputPortMap[inputId] = port;

        inputChannelMap[inputId] = std::make_unique<Channel>(inputId, portName);

        inputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

void JackDeviceBridge::createOutputs(juce::StringArray channelNames)
{
    outputBuffers.reserve(channelNames.size());

    for (int outputId = 0; outputId < channelNames.size(); ++outputId) {
        juce::String portName = channelNames[outputId] + "_in_" + juce::String(outputId);
        auto port = jack_port_register(jackClient,
            portName.toRawUTF8(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput,
            0);
        outputPortMap[outputId] = port;

        outputChannelMap[outputId] = std::make_unique<Channel>(outputId, portName);

        outputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

void JackDeviceBridge::createGUI()
{
    if (!inputChannelMap.empty()) {
        for (auto& [id, channel] : inputChannelMap) {
            channel->createGUI();
            inputComponent.addAndMakeVisible(*channel);
            inputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        }
    }
    if (!outputChannelMap.empty()) {
        for (auto& [id, channel] : outputChannelMap) {
            channel->createGUI();
            outputComponent.addAndMakeVisible(*channel);
            outputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        }
    }
}

// JACK process callback (static wrapper)
int JackDeviceBridge::jackProcessCallback(jack_nframes_t nframes, void* arg) {
    return static_cast<JackDeviceBridge*>(arg)->processJack(nframes);
}

// JACK process implementation
int JackDeviceBridge::processJack(jack_nframes_t nframes) {

    // For each input channel (JUCE input → JACK output)
    for (auto& pair : inputPortMap) {
        int channel = pair.first;
        jack_port_t* port = pair.second;

        float* jackBuffer = (float*)jack_port_get_buffer(port, nframes);

        // Get data from ring buffer (filled by JUCE callback)
        if (channel < inputBuffers.size()) {
            inputBuffers[channel]->read(jackBuffer, nframes);
        }
    }

    // For each output channel (JACK input → JUCE output)
    for (auto& pair : outputPortMap) {
        int channel = pair.first;
        jack_port_t* port = pair.second;

        float* jackBuffer = (float*)jack_port_get_buffer(port, nframes);

        // Write to ring buffer (to be read by JUCE callback)
        if (channel < outputBuffers.size()) {
            outputBuffers[channel]->write(jackBuffer, nframes);
        }
    }

    return 0;
}

void JackDeviceBridge::audioDeviceAboutToStart(juce::AudioIODevice* device) {}

void JackDeviceBridge::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    for (int ch = 0; ch < numInputChannels && ch < inputBuffers.size(); ++ch) {

        const float* processedData = inputChannelMap[ch]->process(
            const_cast<float*>(inputChannelData[ch]), numSamples);

        float squaredSum = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            squaredSum += processedData[i] * processedData[i];
        }
        float rms = std::sqrt(squaredSum / numSamples);
        inputChannelMap[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms));

        inputBuffers[ch]->write(processedData, numSamples);
    }

    // Transfer output data from ring buffers to JUCE (written by JACK)
    for (int ch = 0; ch < numOutputChannels && ch < outputBuffers.size(); ++ch) {
        outputBuffers[ch]->read(outputChannelData[ch], numSamples);
    }
}

void JackDeviceBridge::audioDeviceStopped() {}

void JackDeviceBridge::resized()
{
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 10, getLocalBounds().getWidth(), 500);

    inputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    inputBox.flexDirection = juce::FlexBox::Direction::row;
    inputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    inputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    outputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    outputBox.flexDirection = juce::FlexBox::Direction::row;
    outputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    outputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    deviceBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    deviceBox.flexDirection = juce::FlexBox::Direction::column;
    deviceBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    deviceBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    inputBox.performLayout(inputComponent.getLocalBounds());
    deviceBox.items.add(juce::FlexItem(inputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    outputBox.performLayout(outputComponent.getLocalBounds());
    deviceBox.items.add(juce::FlexItem(outputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    deviceBox.performLayout(fbRect);
}

