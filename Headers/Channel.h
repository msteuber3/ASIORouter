#pragma once
#include "RouterHeader.h"
#include "VerticalMeter.h"
#include <JuceHeader.h>

class Channel : public juce::Component, public juce::Slider::Listener, private juce::Timer
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

	void process(float* channelData, int numSamples);

	void setRMSLevel(float rmsLevel);

	void timerCallback() override;

	//

private:
	int index;
	int deviceIndex;
	juce::Slider volumeSlider;
	juce::Label volumeLabel;
	float volume;

	float rmsLevelSnapshot;

	float levelSnapshot;

	VerticalMeter verticalMeter;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Channel);
};