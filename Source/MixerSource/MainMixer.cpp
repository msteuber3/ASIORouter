#pragma once
#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

// Remove scanCurrentDriver from constructor

MainMixer::MainMixer() : 
    juce::Component(),
    JackAudioCallback()
{
    addAndMakeVisible(inputComponent);
    addAndMakeVisible(outputComponent);
}

MainMixer::MainMixer(int numInputChannels, int numOutputChannels) : 
    juce::Component(), 
    JackAudioCallback(),
    numInputChannels(numInputChannels),
    numOutputChannels(numInputChannels)
{
    addAndMakeVisible(inputComponent);
    addAndMakeVisible(outputComponent);
}

MainMixer::~MainMixer()
{
    inChannels.clear();
    outChannels.clear();

    removeAllChildren();
}

void MainMixer::createBridgeDevices(juce::AudioIODeviceType* deviceType) {

    auto inputDeviceNames = deviceType->getDeviceNames(true);
    auto outputDeviceNames = deviceType->getDeviceNames(false);

    for (const juce::String& name : inputDeviceNames) {
        inputDevices.add(std::make_unique<JackDeviceBridge>(deviceType->createDevice(name, name), name, this, this));
        addAndMakeVisible(inputDevices.getLast());
        mixerBox.items.add(juce::FlexItem(*inputDevices.getLast()).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
    }

    for (const juce::String& name : outputDeviceNames) {
        inputDevices.add(std::make_unique<JackDeviceBridge>(deviceType->createDevice(name, name), name, this, this));
        addAndMakeVisible(outputDevices.getLast());
        mixerBox.items.add(juce::FlexItem(*outputDevices.getLast()).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
    }
}

void MainMixer::resetChannelList()
{
    inputDevices.clear();
    outputDevices.clear();
    inChannels.clear();
    outChannels.clear();
}

void MainMixer::setupCallback(juce::StringArray inputChannelNames, juce::StringArray outputChannelNames, juce::Array<int> inputChannelIds, juce::Array<int> outputChannelIds)
{
   // if (inputChannelNames.size() != numInputChannels) { DBG("JACK CALLBACK SETUP INPUT DEVICE MISMATCH"); }
   // inChannelNames = inputChannelNames;
   //
   // if (outputChannelNames.size() != numOutputChannels) { DBG("JACK CALLBACK SETUP OUTPUT DEVICE MISMATCH"); }
   // outChannelNames = outputChannelNames;
   //
   // createChannels();
}

void MainMixer::audioCallback(juce::AudioBuffer<float> inputData, int numInputChannels, juce::AudioBuffer<float> outputData, int numOutputChannels, int numSamples)
{
    if (numInputChannels != inChannels.size()) {
        DBG("INPUT CHANNEL MISMATCH- IOCALLBACK INS: " + juce::String(numInputChannels) + "INCHANNELS INS : " + juce::String(inChannels.size()));
    }

    for (int channel = 0; channel < numInputChannels; ++channel)
    {
        float* inWritePointer = inputData.getWritePointer(channel);

        if (inChannels[channel] != nullptr)
            inChannels[channel]->process(inWritePointer, numSamples);
            float rms = inputData.getRMSLevel(channel, 0, numSamples);
            inChannels[channel]->setRMSLevel(juce::Decibels::gainToDecibels(rms));
    }

    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        float* outWritePointer = outputData.getWritePointer(channel);

        if (outChannels[channel] != nullptr)
            outChannels[channel]->process(outWritePointer, numSamples);
            float rms = outputData.getRMSLevel(channel, 0, numSamples);
            outChannels[channel]->setRMSLevel(juce::Decibels::gainToDecibels(rms));
    }
}

void MainMixer::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
}

void MainMixer::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    for (int ch = 0; ch < numInputChannels && ch < inputBuffers.size(); ++ch) {
        inputBuffers[ch].write(inputChannelData[ch], numSamples);
    }

    // Transfer output data from ring buffers to JUCE (written by JACK)
    for (int ch = 0; ch < numOutputChannels && ch < outputBuffers.size(); ++ch) {
        outputBuffers[ch].read(outputChannelData[ch], numSamples);
    }
}

void MainMixer::audioDeviceStopped()
{
}

   // juce::AudioBuffer<float> inputBuffer(numInputChannels, numSamples);
   // juce::AudioBuffer<float> outputBuffer(numInputChannels, numSamples);
   //
   // for (int channel = 0; channel < numInputChannels; ++channel)
   // {
   //     inputBuffer.copyFrom(channel, 0, inputChannelData[channel], numSamples);
   // }
   // for (int channel = 0; channel < numInputChannels; ++channel) {
   //     float* writePointer = inputBuffer.getWritePointer(channel);
   //     inChannels[channel]->process(writePointer, numSamples);
   //     float gain = inputBuffer.getRMSLevel(channel, 0, numSamples);
   //     float rmsLevel = juce::Decibels::gainToDecibels(gain);
   //     inChannels[channel]->setRMSLevel(rmsLevel);
   // }
   //

void MainMixer::resized() {
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 10, getLocalBounds().getWidth(), 500);

    mixerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mixerBox.flexDirection = juce::FlexBox::Direction::column;
    mixerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mixerBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mixerBox.performLayout(fbRect);
}