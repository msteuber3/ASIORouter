#pragma once
#include <RouterHeader.h>
#include <JackAudioCallback.h>
#include <Channel.h>
#include <JackJuceBridge.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component, public JackAudioCallback //,  public juce::AudioIODeviceCallback    | Thought: It'd be pretty cool if I could make JackWrapper::AudioCallback a class that his inherits from
{
public:
	MainMixer();
	MainMixer(int numInputChannels, int numOutputChannels);
	~MainMixer() override;

	void createBridgeDevices(juce::AudioIODeviceType* deviceType);

	void resetChannelList();

	void setupCallback(juce::StringArray inputChannelNames, juce::StringArray outputChannelNames, juce::Array<int> inputChannelIds, juce::Array<int> outputChannelIds) override;

	void audioCallback(juce::AudioBuffer<float> inputData, int numInputChannels, juce::AudioBuffer<float> outputData, int numOutputChannels, int numSamples) override;

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceStopped() override;

	void resized() override;

private:
	int numInputChannels = 0;
	juce::OwnedArray<JackDeviceBridge> inputDevices;

	int numOutputChannels = 0;
	juce::OwnedArray<JackDeviceBridge> outputDevices;

	juce::FlexBox mixerBox;

	juce::OwnedArray<Channel> outChannels;
	juce::OwnedArray<Channel> inChannels;

};
