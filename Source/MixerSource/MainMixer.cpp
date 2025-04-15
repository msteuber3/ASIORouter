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
        auto channel = new Channel(input, inChannelNames[input]);
        inputComponent.addAndMakeVisible(channel);
        inputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        inChannels.push_back(channel);
    }
    for (int output = 0; output < numOutputChannels; output++) {
        auto channel = new Channel(output, outChannelNames[output]);
        outputComponent.addAndMakeVisible(channel);
        outputBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        outChannels.push_back(channel);
    }
}

void MainMixer::resetChannelList()
{
    inChannelNames.clear();
    outChannelNames.clear();
    inChannels.clear();
    outChannels.clear();

    inputComponent.removeAllChildren();
    outputComponent.removeAllChildren();

    auto device = deviceManager->getCurrentAudioDevice();
    numInputChannels = device->getActiveInputChannels().countNumberOfSetBits();
    inChannelNames = device->getInputChannelNames();
    numOutputChannels = device->getActiveOutputChannels().countNumberOfSetBits();
    outChannelNames = device->getOutputChannelNames();

    createChannels();
}

void MainMixer::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    numInputChannels = device->getActiveInputChannels().countNumberOfSetBits();
    inChannelNames = device->getInputChannelNames();
    numOutputChannels = device->getActiveOutputChannels().countNumberOfSetBits();
    outChannelNames = device->getOutputChannelNames();
    createChannels();
}

void MainMixer::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels, float* const* outputChannelData, int numOutputChannels, int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
}

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