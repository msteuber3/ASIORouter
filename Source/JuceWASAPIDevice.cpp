/*
  ==============================================================================

    JuceWASPIDevice.cpp
    Created: 17 Apr 2025 10:25:18pm
    Author:  Michael

  ==============================================================================
*/

#include "JuceWASAPIDevice.h"

JuceWASAPIDevice::JuceWASAPIDevice(juce::AudioIODevice* juceIODevice, bool isInput) : 
    juce::GroupComponent(), 
    juce::AudioIODeviceCallback(),
    isInput(isInput),
    juceAudioDevice(juceIODevice),
    name(juceIODevice->getName())
{
    createGUI();

    if (juceAudioDevice) { //TODO: add else
        auto sampleRates = juceAudioDevice->getAvailableSampleRates();
        auto bufferSizes = juceAudioDevice->getAvailableBufferSizes();

        auto sampleRate = sampleRates.isEmpty() ? 44100.0 : sampleRates[0];
        auto bufferSize = bufferSizes.isEmpty() ? 512 : bufferSizes[0];       // Check this later

        int requestedInChannels = juceAudioDevice->getInputChannelNames().size();
        int requestedOutChannels = juceAudioDevice->getOutputChannelNames().size();

        isInput ? juceAudioDevice->open(requestedInChannels, 0, sampleRate, bufferSize) : juceAudioDevice->open(0, requestedOutChannels, sampleRate, bufferSize);
        juceAudioDevice->start(this);
        
        if (isInput) {
            int actualChannels = juceAudioDevice->getActiveInputChannels().countNumberOfSetBits();
            DBG("Device: " + name + " requested " + juce::String(requestedInChannels) +
                " input channels, got " + juce::String(actualChannels));
        }
        else {
            int actualChannels = juceAudioDevice->getActiveOutputChannels().countNumberOfSetBits();
            DBG("Device: " + name + " requested " + juce::String(requestedOutChannels) +
                " input channels, got " + juce::String(actualChannels));
        }
        
        if (juceAudioDevice->isOpen()) { // TODO: add else
            auto channels = isInput ? juceAudioDevice->getInputChannelNames() : juceAudioDevice->getOutputChannelNames();
            if (!channels.isEmpty()) { //TODO: Add else
                createChannels(channels);
            }
        }
    }
}

JuceWASAPIDevice::~JuceWASAPIDevice()
{
    if (juceAudioDevice) {
        juceAudioDevice->stop();
        juceAudioDevice->close();
    }
}

void JuceWASAPIDevice::createGUI()
{
    deviceLabel.setText(juceAudioDevice->getName(), juce::dontSendNotification);
    deviceLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(deviceLabel);
}

juce::String JuceWASAPIDevice::getFullDeviceName()
{
    return juceAudioDevice->getName();
}

juce::String JuceWASAPIDevice::getShortDeviceName()
{
    return name;
}

void JuceWASAPIDevice::renameDevice(juce::String newName)
{
    name = newName;
    deviceLabel.setText(name, juce::dontSendNotification);
}

void JuceWASAPIDevice::createChannels(juce::StringArray channelNames)
{
    buffers.reserve(channelNames.size());

    for (int id = 0; id < channelNames.size(); ++id) {
        channelMap[id] = std::make_unique<Channel>(id, channelNames[id]);
        addAndMakeVisible(channelMap[id].get());
        buffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

std::unique_ptr<juce::AudioIODevice>& JuceWASAPIDevice::getAudioDevice()
{
    return juceAudioDevice;
}

void JuceWASAPIDevice::audioDeviceAboutToStart(juce::AudioIODevice* device) {
    // Have an active channel thing (maybe a little green dot?)
}

void JuceWASAPIDevice::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData, 
    int numInputChannels, 
    float* const* outputChannelData, 
    int numOutputChannels, 
    int numSamples, 
    const juce::AudioIODeviceCallbackContext& context)
{
    int numChannels = isInput ? numInputChannels : numOutputChannels;

    DBG("Processing " + juce::String(numChannels) + " channels for device: " + name);

    for (int ch = 0; ch < numChannels && ch < buffers.size(); ++ch) {

        const float* processedData = channelMap[ch]->process(
            const_cast<float*>((isInput ? inputChannelData : outputChannelData)[ch]), numSamples);

        float squaredSum = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            squaredSum += processedData[i] * processedData[i];
        }
        float rms = std::sqrt(squaredSum / numSamples);
        channelMap[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms));

        buffers[ch]->write(processedData, numSamples);
    }
}

void JuceWASAPIDevice::audioDeviceStopped() {
    // Have an inactive channel thing, gray it out or smthn

}

int JuceWASAPIDevice::calculatePreferredWidth()
{
    return channelMap.size() * SLIDER_WIDTH + 20;
}

int JuceWASAPIDevice::getNumChannels()
{
    if (this == nullptr)
        return 0;

    return channelMap.size();
}

juce::Point<float> JuceWASAPIDevice::getPreferredSize()
{
    return juce::Point<float>(calculatePreferredWidth(), 240.0f);
}

void JuceWASAPIDevice::resized()
{
    DBG("Inside the WASAPI resized method");
    auto bounds = getLocalBounds().reduced(10);

    //JUCE GRIDS WOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    juce::Grid grid;

    // Define grid settings
    grid.rowGap = Px(10);
    grid.columnGap = Px(5);

    const int numColumns = channelMap.size();

    grid.templateRows = {
        Track(Px(250)), //channels
    };

    // Setup columns - one for each channel
    juce::Array<Track> columnTracks;
    for (int i = 0; i < numColumns; ++i)
        columnTracks.add(Track(Px(SLIDER_WIDTH)));

    grid.templateColumns = columnTracks;



    for (auto& [id, channel] : channelMap) {
        auto channelItem = juce::GridItem(*channel).withArea(1, id + 1, 2, id + 2); // Add channels
        grid.items.add(channelItem);
    }

    grid.performLayout(bounds);
}
