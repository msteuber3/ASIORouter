/*
  ==============================================================================

    JackRouterManager.cpp
    Created: 15 Apr 2025 9:47:37pm
    Author:  Michael

  ==============================================================================
*/

#include "JackJuceBridge.h"

JackDeviceBridge::JackDeviceBridge(juce::AudioIODevice* juceAudioDevice, const char* jackClientName, JackAudioCallback* jackCallback, juce::AudioIODeviceCallback* juceCallback) : 
    juce::Component(),
    juce::AudioIODeviceCallback(),
    clientName(jackClientName),
    deviceName(juceAudioDevice->getName()),
    juceAudioDevice(juceAudioDevice),
    jackClient(jack_client_open(jackClientName, JackNullOption, NULL)),
    jackPortCallback(std::make_unique<JackAudioCallback>(jackCallback)),
    juceDeviceCallback(std::make_unique<juce::AudioIODeviceCallback>(juceCallback))
{ 
    if (juceAudioDevice) {
        auto inputChannels = juceAudioDevice->getInputChannelNames();
        auto outputChannels = juceAudioDevice->getOutputChannelNames();

        if(!inputChannels.isEmpty()) 
            createInputs(inputChannels);

        if (!inputChannels.isEmpty())
            createOutputs(outputChannels);
        
        // Set JACK process callback
        jack_set_process_callback(jackClient, jackProcessCallback, this);

        // Start JUCE device
        juceAudioDevice->open(inputChannels.size(), outputChannels.size(),
            juceAudioDevice->getCurrentSampleRate(),
            juceAudioDevice->getCurrentBufferSizeSamples());
        juceAudioDevice->start(this);

        // Activate JACK client
        jack_activate(jackClient);
    }
}

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

void JackDeviceBridge::createInputs(juce::StringArray channelNames)
{
    for (int inputId = 0; inputId < channelNames.size(); ++inputId) {
        juce::String portName = channelNames[inputId] + "_in_" + juce::String(inputId);

        inputPortMap.emplace(inputId, jack_port_register(jackClient,
            portName.toRawUTF8(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsOutput,  // Output from JACK's perspective
            0));

        inputChannelMap.emplace(inputId, new Channel(inputId, portName));

        // Create buffer for this channel
        inputBuffers.emplace(inputId, 8192);  // Buffer size should be power of 2

        inputComponent.addAndMakeVisible(inputChannelMap[inputId]);
        inputBox.items.add(juce::FlexItem(*inputChannelMap[inputId]).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
    }
}

void JackDeviceBridge::createOutputs(juce::StringArray channelNames)
{
    for (int outputId = 0; outputId < channelNames.size(); ++outputId) {
        juce::String portName = channelNames[outputId] + "_out_" + juce::String(outputId);

        outputPortMap.emplace(outputPortMap, jack_port_register(jackClient,
            portName.toRawUTF8(),
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput,  // Input from JACK's perspective 
            0));

        outputChannelMap.emplace(outputId, new Channel(outputId, portName));

        // Create buffer for this channel
        outputBuffers.emplace(outputId, 8192);  // Buffer size should be power of 2

        outputComponent.addAndMakeVisible(outputChannelMap[outputId]);
        outputBox.items.add(juce::FlexItem(*outputChannelMap[outputId]).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
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
            inputBuffers[channel].read(jackBuffer, nframes);
        }
    }

    // For each output channel (JACK input → JUCE output)
    for (auto& pair : outputPortMap) {
        int channel = pair.first;
        jack_port_t* port = pair.second;

        float* jackBuffer = (float*)jack_port_get_buffer(port, nframes);

        // Write to ring buffer (to be read by JUCE callback)
        if (channel < outputBuffers.size()) {
            outputBuffers[channel].write(jackBuffer, nframes);
        }
    }

    return 0;
}

void JackDeviceBridge::audioDeviceAboutToStart(juce::AudioIODevice* device) {}

void JackDeviceBridge::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    // Set up temporary busses so that we can process the data before passing it along to jack
    juce::AudioBuffer<float> tempInputBuffer(const_cast<float**>(inputChannelData), numInputChannels, numSamples);
    juce::AudioBuffer<float> tempOutputBuffer(outputChannelData, numOutputChannels, numSamples);

    for (int ch = 0; ch < numInputChannels && ch < inputBuffers.size(); ++ch) {

        float* inWritePointer = tempInputBuffer.getWritePointer(ch);
        const float* processedData = inputChannelMap[ch]->process(inWritePointer, numSamples);

        float rms = tempInputBuffer.getRMSLevel(ch, 0, numSamples);
        inputChannelMap[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms)); // It's probably wise to do this in the JACK process block cause that's what's going to be shooting the data out elsewhere

        inputBuffers[ch].write(processedData, numSamples); //write to ring buffer for JACK to see
    }


    // Transfer output data from ring buffers to JUCE (written by JACK)
    for (int ch = 0; ch < numOutputChannels && ch < outputBuffers.size(); ++ch) {
        outputBuffers[ch].read(outputChannelData[ch], numSamples);
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

