#pragma once
#include <RouterHeader.h>
#include <Channel.h>
#include <JuceAsioDevice.h>
#include <JackJuceBridge.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component //,  public juce::AudioIODeviceCallback    | Thought: It'd be pretty cool if I could make JackWrapper::AudioCallback a class that his inherits from
{
public:
	MainMixer();
	~MainMixer() override;

	void createJuceDevices(juce::AudioIODeviceType* deviceType);

	void createBridgeDevices(juce::AudioIODeviceType* deviceType);

	void createGUI();

	void createBridgeGUI();

	void resized() override;

private:
	juce::OwnedArray<JuceAsioDevice> juceDevices;

	juce::OwnedArray<JackDeviceBridge> inputDevices;
	juce::OwnedArray<JackDeviceBridge> outputDevices;

	juce::FlexBox mixerBox;

	juce::OwnedArray<Channel> outChannels;
	juce::OwnedArray<Channel> inChannels;

};
