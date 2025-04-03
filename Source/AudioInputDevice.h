#pragma once

#include <JuceHeader.h>
#include "Channel.h"

// I know I want to recieve an IO device from the main component and extract it's channels and put those channels in a 
// graph which will in turn be mapped onto a larger graph

class AudioInputDevice
{
public:
	AudioInputDevice(const juce::String& inDevice, const juce::String& outDevice, juce::AudioIODeviceType* deviceType, int deviceInd);
	~AudioInputDevice();

	bool createInputDevice();

	int createChannels(juce::Component* mainComponent);

	std::vector<std::unique_ptr<Channel>> channels;

private:
	juce::AudioIODeviceType* deviceType;
	juce::String inDeviceName;
	juce::String outDeviceName;

	juce::AudioIODevice* inputDevice;
	juce::AudioDeviceManager deviceManager;

	juce::AudioProcessorGraph::AudioGraphIOProcessor* audioProcessor;

	int deviceIndex;


};