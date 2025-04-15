#pragma once
#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

// Remove scanCurrentDriver from constructor

MainMixer::MainMixer()
    : juce::Component(), juce::AudioIODeviceCallback()
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

void MainMixer::createChannels() {
    for (int input = 0; input < numInputChannels; input++) {
        auto channel =inChannels.add(new Channel(input, inChannelNames[input]));
        inputComponent.addAndMakeVisible(channel);
        inputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
    }
    for (int output = 0; output < numOutputChannels; output++) {
        auto channel = outChannels.add(new Channel(output, outChannelNames[output]));
        outputComponent.addAndMakeVisible(channel);
        outputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
    }
   // setSize(DEVICE_CONTAINER_WIDTH, 500);
}

void MainMixer::resetChannelList()
{
    inChannelNames.clear();
    outChannelNames.clear();
    inChannels.clear();
    outChannels.clear();

    inputComponent.removeAllChildren();
    outputComponent.removeAllChildren();
}

void MainMixer::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (!inChannels.isEmpty()) { 
        inChannels.clear();
        inputComponent.removeAllChildren();
    }
    if (!outChannels.isEmpty()) { 
        outChannels.clear(); 
        outputComponent.removeAllChildren();

    }

    numInputChannels = device->getActiveInputChannels().countNumberOfSetBits();
    inChannelNames = device->getInputChannelNames();

    numOutputChannels = device->getActiveOutputChannels().countNumberOfSetBits();
    outChannelNames = device->getOutputChannelNames();

    createChannels();
}

void MainMixer::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData, 
    int numInputChannels, 
    float* const* outputChannelData, 
    int numOutputChannels, 
    int numSamples, 
    const juce::AudioIODeviceCallbackContext& context )
{
    if (numInputChannels != inChannels.size()) {
        DBG("INPUT CHANNEL MISMATCH- IOCALLBACK INS: " + juce::String(numInputChannels) + "INCHANNELS INS : " + juce::String(inChannels.size()));
    }

    juce::AudioBuffer<float> inputBuffer(const_cast<float**>(inputChannelData), numInputChannels, numSamples);

    // Wrap output
    juce::AudioBuffer<float> outputBuffer(outputChannelData, numOutputChannels, numSamples);

    // For each input channel
    for (int ch = 0; ch < numInputChannels; ++ch)
    {
        float* inputData = inputBuffer.getWritePointer(ch); // or getReadPointer if you want const

        // Send to your processor
        if (inChannels[ch] != nullptr)
            inChannels[ch]->process(inputData, numSamples); // or pass rms, too
            float rms = inputBuffer.getRMSLevel(ch, 0, numSamples);
            inChannels[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms));

    }

    // For each output channel
    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        float* outputData = outputBuffer.getWritePointer(ch);

        if (outChannels[ch] != nullptr)
            outChannels[ch]->process(outputData, numSamples);
            float rms = outputBuffer.getRMSLevel(ch, 0, numSamples);
            outChannels[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms));
    }
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

void MainMixer::audioDeviceStopped()
{
}

void MainMixer::resized() {
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 10, getLocalBounds().getWidth(), 500);
 
    inputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    inputBox.flexDirection = juce::FlexBox::Direction::row;
    inputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    inputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    outputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    outputBox.flexDirection = juce::FlexBox::Direction::row;
    outputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    outputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mixerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mixerBox.flexDirection = juce::FlexBox::Direction::column;
    mixerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mixerBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    
    inputBox.performLayout(inputComponent.getLocalBounds());
    mixerBox.items.add(juce::FlexItem(inputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    outputBox.performLayout(outputComponent.getLocalBounds());
    mixerBox.items.add(juce::FlexItem(outputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    mixerBox.performLayout(fbRect);
}