#pragma once
#include <RouterHeader.h>
#include <Channel.h>
#include <JuceAsioDevice.h>
#include <JuceWASAPIDeviceContainer.h>
#include <JackJuceBridge.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component //,  public juce::AudioIODeviceCallback    | Thought: It'd be pretty cool if I could make JackWrapper::AudioCallback a class that his inherits from
{
public:
	MainMixer();
	~MainMixer() override;

	bool generateDeviceType();

	void createWASAPIDevices();

	void createASIODevices();

	void createBridgeDevices();

	void createBridgeGUI();
	
	void calculatePreferredSize();

	void deleteDevice(int index);

	void handleDeviceNotFound();

	juce::Point<float> getPreferredSize() const;

	void paint(juce::Graphics& g) override;

	void resized() override;

private:
	juce::OwnedArray<JuceAsioDevice> juceDevices;

	juce::OwnedArray<JackDeviceBridge> inputDevices;
	juce::OwnedArray<JackDeviceBridge> outputDevices;

	juce::FlexBox mixerBox;

	juce::Point<float> preferredSize;

	std::unique_ptr<juce::AudioIODeviceType> deviceType;

	std::unique_ptr<JuceWASAPIDeviceContainer> WASAPIContainer;

	juce::Label deviceNotFoundLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMixer)
};
