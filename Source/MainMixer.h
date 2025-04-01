#pragma once
#include <JuceHeader.h>

class MainMixer : public juce::MixerAudioSource, public juce::AudioIODeviceCallback
{
public:
	MainMixer(const juce::AudioDeviceManager &mixerManager);
	~MainMixer();

	void audioDeviceIOCallbackWithContext(const float* const * inputChannelData, int numInputChannels,
		float* const * outputChannelData, int numOutputChannels,
		int numSamples, const juce::AudioIODeviceCallbackContext& context) override;

	void InitializeInputDevices();

	void RefreshInputDevices();


private:
	std::vector<std::unique_ptr<juce::AudioSource>> inputSourceList;
	juce::AudioDeviceManager &mixerDeviceManager;
	juce::MixerAudioSource mixer;
};