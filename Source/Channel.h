#pragma once
#include <JuceHeader.h>

class Channel : public juce::Component, public juce::Slider::Listener
{
public:
	Channel(int index, juce::String name);
	~Channel() override;

//	Channel(Channel&&) = default;
//	Channel& operator=(Channel&&) = default;

	void sliderValueChanged(juce::Slider* slider) override;

	void resized() override;

private:
	int index;
	juce::Slider volumeSlider;
	juce::Label volumeLabel;
};