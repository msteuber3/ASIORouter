#include "AudioInputDevice.h"

AudioInputDevice::AudioInputDevice(const juce::String& inDevice, const juce::String& outDevice, juce::AudioIODeviceType* deviceType, int deviceInd)
	: Component(), inDeviceName(inDevice), outDeviceName(outDevice), deviceType(deviceType), deviceIndex(deviceInd), inputDevice(deviceType->createDevice(inDevice, outDevice))
{
    createInputDevice();
}

AudioInputDevice::~AudioInputDevice() 
{
    inputDevice->close();
    channels.clear();
    removeAllChildren();
}

void AudioInputDevice::createGuiElements()
{
    sliderBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 0, DEVICE_CONTAINER_WIDTH, 200);
    sliderBox.performLayout(fbRect);
}

 

bool AudioInputDevice::createInputDevice()
{
	//Might break with mics
    if (inputDevice == nullptr)
    {
        juce::Logger::writeToLog("Failed to create input device: " + inDeviceName);
        return false;
    }

    const juce::BigInteger& numInputs = inputDevice->getInputChannelNames().size();
    const juce::BigInteger& numOutputs = inputDevice->getOutputChannelNames().size();
    if (inputDevice->open(numInputs, numOutputs, 44100, 512) != "") {
        DBG("Failed to open device: " + inputDevice->getName());
        return false;
    }

    inputDevice->start(nullptr);
    if (!inputDevice->isOpen())
    {
        juce::Logger::writeToLog("Failed to start input device: " + inDeviceName);
		return false;
    }
    else
    {
        juce::Logger::writeToLog("Input device started successfully: " + inDeviceName);
    }
	return true;
}

int AudioInputDevice::createChannels(juce::Component* mainComponent)
{
    auto activeInputChannels = inputDevice->getActiveInputChannels();
    auto activeOutputChannels = inputDevice->getActiveOutputChannels();

    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

    for (int i = 0; i < maxInputChannels; i++) {
		juce::String name = inputDevice->getInputChannelNames()[i];
        auto channel = std::make_unique<Channel>(i, deviceIndex, name);
        
        addAndMakeVisible(*channel);
        
        sliderBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));

        channels.push_back(std::move(channel));
    }
    return maxInputChannels;
}

void AudioInputDevice::resized()
{
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 0, DEVICE_CONTAINER_WIDTH, 200);

    sliderBox.flexWrap = juce::FlexBox::Wrap::noWrap; 
    sliderBox.flexDirection = juce::FlexBox::Direction::row;
    sliderBox.justifyContent = juce::FlexBox::JustifyContent::flexStart; 
    sliderBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    sliderBox.performLayout(fbRect);
}

