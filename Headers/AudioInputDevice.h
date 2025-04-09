#pragma once

#include <Channel.h>
#include <RouterHeader.h>
#include <JuceHeader.h>

// I know I want to recieve an IO device from the main component and extract it's channels and put those channels in a 
// graph which will in turn be mapped onto a larger graph

class AudioInputDevice : public juce::Component
{
public:
	AudioInputDevice(const juce::String& inDevice, const juce::String& outDevice, juce::AudioIODeviceType* deviceType, int deviceInd, bool isInput);
	~AudioInputDevice();

	void createGuiElements();

	bool createInputDevice();

	int createChannels(juce::Component* mainComponent);

	void resized() override;

	std::vector<std::unique_ptr<Channel>> channels;

private:
	juce::AudioIODeviceType* deviceType;
	juce::String inDeviceName;
	juce::String outDeviceName;

	juce::Label deviceLabel;

	std::unique_ptr<juce::AudioIODevice> inputDevice;
	juce::Component sliderComponent;
	juce::FlexBox sliderBox;
	juce::FlexBox deviceBox;

	int deviceIndex;

	bool isInput;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInputDevice);

};
