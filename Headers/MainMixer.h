#pragma once
#include <AudioInputDevice.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component
{
public:
	MainMixer(juce::AudioIODeviceType* deviceType);
	~MainMixer();

	void ScanCurrentDriver();

	void resized() override;

	//void audioDeviceIOCallbackWithContext(const float* const * inputChannelData, int numInputChannels,
	//	float* const * outputChannelData, int numOutputChannels,
	//	int numSamples, const juce::AudioIODeviceCallbackContext& context) override;

	void InitializeInputDevices();

	void RefreshInputDevices();


private:
	juce::FlexBox mixerBox;
	std::vector<std::unique_ptr<juce::AudioSource>> inputSourceList;
	juce::AudioIODeviceType* deviceType;
	juce::StringArray inDeviceNames;
	juce::StringArray outDeviceNames;
	std::vector<std::unique_ptr<AudioInputDevice>> inputDevices;
	std::unique_ptr<juce::AudioProcessorGraph> outputGraph;

};
