#pragma once
#include "RouterHeader.h"
#include "VerticalMeter.h"
#include <JuceHeader.h>

class Channel : public juce::Component, public juce::Slider::Listener, public juce::AudioProcessor, private juce::Timer
{
public:
	Channel(int index, int deviceIndex, juce::String name);
	~Channel() override;

	void createSlider(juce::String labelName);

	void sliderValueChanged(juce::Slider* slider) override;

	int getXCoord();

	int getYCoord();

	void resized() override;

	//

	const juce::String getName() const override;

	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;

	void releaseResources() override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	void timerCallback() override;

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
	int index;
	int deviceIndex;
	juce::Slider volumeSlider;
	juce::Label volumeLabel;
	float volume;

	float rmsLevel;
	std::atomic<float> level{ 0.0f };

	VerticalMeter verticalMeter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Channel);
};