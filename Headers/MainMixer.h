#pragma once
#include <RouterHeader.h>
#include <Channel.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component, public juce::AudioIODeviceCallback
{
public:
	MainMixer();
	~MainMixer();

	void createChannels();

	void resetChannelList();

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int 	numInputChannels,
		float* const* outputChannelData,
		int 	numOutputChannels,
		int 	numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceStopped() override;

	void resized() override;

private:
	int numInputChannels = 0;
	juce::StringArray inChannelNames;
	int numOutputChannels = 0;
	juce::StringArray outChannelNames;

	juce::FlexBox mixerBox;
	juce::Component inputComponent;
	juce::FlexBox inputBox;
	juce::Component outputComponent;
	juce::FlexBox outputBox;

	std::vector<Channel*> outChannels;
	std::vector<Channel*> inChannels;

};
