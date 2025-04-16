/*
  ==============================================================================

    JuceAsioDevice.cpp
    Created: 16 Apr 2025 7:41:16am
    Author:  Michael

  ==============================================================================
*/

#include <JuceAsioDevice.h>

JuceAsioDevice::JuceAsioDevice(juce::AudioIODevice* juceIODevice) : 
    juce::Component(),
    juce::AudioIODeviceCallback(),
    juceAudioDevice(juceIODevice)
{
    if (juceAudioDevice) {
       
        auto inputChannels = juceAudioDevice->getInputChannelNames();
        auto outputChannels = juceAudioDevice->getOutputChannelNames();

        if (!inputChannels.isEmpty())
            createInputChannels(inputChannels);

        if (!inputChannels.isEmpty())
            createOutputChannels(outputChannels);

        juceAudioDevice->open(inputChannels.size(), outputChannels.size(),
            juceAudioDevice->getCurrentSampleRate(),
            juceAudioDevice->getCurrentBufferSizeSamples());
        juceAudioDevice->start(this);
    }
}

JuceAsioDevice::~JuceAsioDevice()
{
    if (juceAudioDevice) {
        juceAudioDevice->stop();
        juceAudioDevice->close();
    }
}

void JuceAsioDevice::createInputChannels(juce::StringArray channelNames)
{
    inputBuffers.reserve(channelNames.size());

    for (int inputId = 0; inputId < channelNames.size(); ++inputId) {
        inputChannelMap[inputId] = std::make_unique<Channel>(inputId, channelNames[inputId]);
        inputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

void JuceAsioDevice::createOutputChannels(juce::StringArray channelNames)
{
    outputBuffers.reserve(channelNames.size());

    for (int outputId = 0; outputId < channelNames.size(); ++outputId) {
        outputChannelMap[outputId] = std::make_unique<Channel>(outputId, channelNames[outputId]);
        outputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

void JuceAsioDevice::createGUI()
{
    addAndMakeVisible(inputComponent);
    addAndMakeVisible(outputComponent);

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

void JuceAsioDevice::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
}

void JuceAsioDevice::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
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
    for (int ch = 0; ch < numOutputChannels && ch < outputBuffers.size(); ++ch) {
        outputBuffers[ch]->read(outputChannelData[ch], numSamples);
    }
}

void JuceAsioDevice::audioDeviceStopped()
{
}

void JuceAsioDevice::resized()
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
