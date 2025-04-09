#pragma once
#include <RouterHeader.h>
#include <AudioInputDevice.h>
#include <JuceHeader.h>

class MainMixer : public juce::Component, public juce::AudioProcessor
{
public:
	MainMixer(juce::AudioIODeviceType* deviceType, 
		std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses,
		std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses);
	static juce::AudioProcessor::BusesProperties buildBuses(
		std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses,
		std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses);
	~MainMixer();

	void ScanCurrentDriver();

	//void addChannelToBus(int busIndex);

	void resized() override;

	//void audioDeviceIOCallbackWithContext(const float* const * inputChannelData, int numInputChannels,
	//	float* const * outputChannelData, int numOutputChannels,
	//	int numSamples, const juce::AudioIODeviceCallbackContext& context) override;

	void InitializeInputDevices();

	void RefreshInputDevices();

	//

	bool isBusesLayoutSupported(const BusesLayout& layouts) const;

	const juce::String getName() const override;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;

	void releaseResources() override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	double getTailLengthSeconds() const override;

	bool acceptsMidi() const override;

	bool producesMidi() const override;

	juce::AudioProcessorEditor* createEditor() override;

	bool hasEditor() const override;

	int getNumPrograms() override;

	int getCurrentProgram() override;

	void setCurrentProgram(int index) override;

	const juce::String getProgramName(int index) override;

	void changeProgramName(int index, const juce::String& newName) override;

	void getStateInformation(juce::MemoryBlock& destData) override;

	void setStateInformation(const void* data, int sizeInBytes) override;

	//


private:
	juce::FlexBox mixerBox;
	juce::Component inputComponent;
	juce::FlexBox inputBox;
	juce::Component outputComponent;
	juce::FlexBox outputBox;

	std::vector<std::unique_ptr<juce::AudioSource>> inputSourceList;
	juce::AudioIODeviceType* deviceType;
	juce::StringArray inDeviceNames;
	juce::StringArray outDeviceNames;

	std::vector<std::unique_ptr<AudioInputDevice>> inputDevices;
	std::vector<std::unique_ptr<AudioInputDevice>> outputDevices;

	std::unique_ptr<juce::AudioProcessorGraph> outputGraph;

	std::vector<std::unique_ptr<Channel>> outChannels;
	std::vector<std::unique_ptr<Channel>> inChannels;

};
