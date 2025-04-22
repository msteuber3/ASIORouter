#pragma once
#include <RouterHeader.h>
#include <VerticalMeter.h>
#include <JuceHeader.h>

class Channel : public juce::Component, public juce::Slider::Listener, private juce::Timer
{
public:
	Channel(int index, juce::String name);
	~Channel() override;

	void createSlider();

	void sliderValueChanged(juce::Slider* slider) override;

	void resized() override;

	const float* process(float* writePointer, int numSamples);

	void setRMSLevel(float rmsLevel);

	void timerCallback() override;

private:


	int index;
	
	juce::Slider volumeSlider;
	juce::Label volumeLabel;

	float volume = 0;

	juce::String name;

	float rmsLevelSnapshot;

	float levelSnapshot;

	VerticalMeter verticalMeter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Channel);
};