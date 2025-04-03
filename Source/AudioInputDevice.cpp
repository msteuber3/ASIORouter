#include "AudioInputDevice.h"

AudioInputDevice::AudioInputDevice(const juce::String& inDevice, const juce::String& outDevice, juce::AudioIODeviceType* deviceType, int deviceInd) 
	: inDeviceName(inDevice), outDeviceName(outDevice), deviceType(deviceType), deviceIndex(deviceInd)
{
	juce::AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType NodeType = juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode;

	audioProcessor = new juce::AudioProcessorGraph::AudioGraphIOProcessor(NodeType);

    createInputDevice();
}



AudioInputDevice::~AudioInputDevice() {}

bool AudioInputDevice::createInputDevice()
{
	//Might break with mics
	inputDevice = deviceType->createDevice(inDeviceName, outDeviceName);
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
        mainComponent->addAndMakeVisible(*channel);
        channels.push_back(std::move(channel));
    }
    return maxInputChannels;
}

