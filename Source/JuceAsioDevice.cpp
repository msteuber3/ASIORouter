/*
  ==============================================================================

    JuceAsioDevice.cpp
    Created: 16 Apr 2025 7:41:16am
    Author:  Michael

  ==============================================================================
*/

#include <JuceAsioDevice.h>

JuceAsioDevice::JuceAsioDevice(juce::AudioIODevice* juceIODevice) : 
    juce::GroupComponent(juce::String(juceIODevice->getName() + " Control Panel"), juceIODevice->getName()),
    juce::AudioIODeviceCallback(),
    juceAudioDevice(juceIODevice)
{
    createGUI();

    if (juceAudioDevice) {
        auto sampleRates = juceAudioDevice->getAvailableSampleRates();
        auto bufferSizes = juceAudioDevice->getAvailableBufferSizes();

        auto sampleRate = sampleRates.isEmpty() ? 44100.0 : sampleRates[0];
        auto bufferSize = bufferSizes.isEmpty() ? 512 : bufferSizes[0];

        int numInputChannels = juceAudioDevice->getInputChannelNames().size();
        int numOutputChannels = juceAudioDevice->getOutputChannelNames().size();

        juce::BigInteger activeInputChannels;
        juce::BigInteger activeOutputChannels;

        activeInputChannels.setRange(0, 6, true);
        activeOutputChannels.setRange(0, 4, true);

        juceAudioDevice->open(activeInputChannels, activeOutputChannels, sampleRate, bufferSize);
        juceAudioDevice->start(this);
       
        auto inputChannels = juceAudioDevice->getInputChannelNames();
        auto outputChannels = juceAudioDevice->getOutputChannelNames();

        int actualInChannels = juceAudioDevice->getActiveInputChannels().countNumberOfSetBits();
        DBG("Device: " + juceAudioDevice->getName() + " requested " + juce::String(numInputChannels) +
          " input channels, got " + juce::String(actualInChannels));
 
        int actualOutChannels = juceAudioDevice->getActiveOutputChannels().countNumberOfSetBits();
        DBG("Device: " + juceAudioDevice->getName() + " requested " + juce::String(numOutputChannels) +
            " output channels, got " + juce::String(actualOutChannels));
       

        if (!inputChannels.isEmpty())
            createInputChannels(inputChannels);

        if (!outputChannels.isEmpty())
            createOutputChannels(outputChannels);
    }
    setSize(calculatePreferredWidth(), 500);

    DBG("Opened ASIO4ALL with "
        << juceAudioDevice->getActiveInputChannels().countNumberOfSetBits()
        << " input channels and "
        << juceAudioDevice->getActiveOutputChannels().countNumberOfSetBits()
        << " output channels.");

}

JuceAsioDevice::~JuceAsioDevice()
{
    if (juceAudioDevice) {
        juceAudioDevice->stop();
        juceAudioDevice->close();
    }
}

void JuceAsioDevice::createGUI()
{
    deviceLabel.setText(juceAudioDevice->getName(), juce::dontSendNotification);
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
}

void JuceAsioDevice::createInputChannels(juce::StringArray channelNames)
{
    inputBuffers.reserve(channelNames.size());

    for (int inputId = 0; inputId < channelNames.size(); ++inputId) {
        inputChannelMap[inputId] = std::make_unique<Channel>(inputId, channelNames[inputId]);
        addAndMakeVisible(inputChannelMap[inputId].get());
        inputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

void JuceAsioDevice::createOutputChannels(juce::StringArray channelNames)
{
    outputBuffers.reserve(channelNames.size());

    for (int outputId = 0; outputId < channelNames.size(); ++outputId) {
        outputChannelMap[outputId] = std::make_unique<Channel>(outputId, channelNames[outputId]);
        addAndMakeVisible(outputChannelMap[outputId].get());
        outputBuffers.push_back(std::make_unique<RingBuffer<float>>(4096));
    }
}

std::unique_ptr<juce::AudioIODevice>& JuceAsioDevice::getAudioDevice()
{
    return juceAudioDevice;
}

std::shared_ptr<RingBuffer<float>> JuceAsioDevice::getBuffer(bool isInput, int index)
{
    return (isInput ? inputBuffers[index] : outputBuffers[index]);
}

void JuceAsioDevice::audioDeviceAboutToStart(juce::AudioIODevice* device) {}

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
        if (outputChannelData[ch] != nullptr)
            std::memcpy(outputChannelData[ch], inputChannelData[ch], sizeof(float) * numSamples);
        else
            std::fill(outputChannelData[ch], outputChannelData[ch] + numSamples, 0.0f);

        const float* processedData = outputChannelMap[ch]->process(outputChannelData[ch], numSamples);
        float squaredSum = 0.0f;
        for (int i = 0; i < numSamples; ++i) {
            squaredSum += processedData[i] * processedData[i];
        }
        float rms = std::sqrt(squaredSum / numSamples);
        outputChannelMap[ch]->setRMSLevel(juce::Decibels::gainToDecibels(rms));

        outputBuffers[ch]->read(outputChannelData[ch], numSamples);
    }
}

void JuceAsioDevice::audioDeviceStopped() {}

int JuceAsioDevice::calculatePreferredWidth()
{
    return juce::jmax((int)inputChannelMap.size(), (int)outputChannelMap.size()) * SLIDER_WIDTH + 20;
}

juce::Point<float> JuceAsioDevice::getPreferredSize() 
{
    float width = (float)calculatePreferredWidth();
    float height = 300; // Fixed height or calculate based on needs
    return { width, height };
}

void JuceAsioDevice::resized()
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
    const int numColumns = juce::jmax(inputChannelMap.size(), outputChannelMap.size());

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
    for (auto& [id, channel] : inputChannelMap) {
        auto channelItem = juce::GridItem(*channel).withArea(3, id + 1, 4, id + 2);
        grid.items.add(channelItem);
    }

    // Output section label (spans all columns)
    auto outputLabelItem = juce::GridItem(outputSectionLabel).withArea(4, 1, 5, numColumns + 1);
    grid.items.add(outputLabelItem);

    // Output channels
    for (auto& [id, channel] : outputChannelMap) {
        auto channelItem = juce::GridItem(*channel).withArea(5, id + 1, 6, id + 2);
        grid.items.add(channelItem);
    }

    // Perform layout
    grid.performLayout(bounds);
}
