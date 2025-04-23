#pragma once
#include <RouterHeader.h>
#include <Channel.h>
#include <JuceAsioDevice.h>
#include <JuceWASAPIDeviceContainer.h>
#include <JackJuceBridge.h>
#include <JuceHeader.h>
#include <JackWrapper.h>

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

	void startJack();

	juce::Point<float> getPreferredSize() const;

	void paint(juce::Graphics& g) override;

	void resized() override;

private:
	juce::OwnedArray<JuceAsioDevice> juceDevices;

	juce::OwnedArray<JackDeviceBridge> inputDevices;
	juce::OwnedArray<JackDeviceBridge> outputDevices;

	juce::FlexBox mixerBox;

	juce::Point<float> preferredSize;

	bool activateJackVirtualDevice = true;

	std::unique_ptr<juce::AudioDeviceManager> audioDeviceManager;
	std::unique_ptr<juce::AudioIODeviceType> deviceType;

	std::unique_ptr<JuceWASAPIDeviceContainer> WASAPIContainer;
	std::unique_ptr<JackWrapper> jackWrapper;

	juce::Label deviceNotFoundLabel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMixer)
};
