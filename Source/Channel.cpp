#include "Channel.h"


Channel::Channel(int i, juce::String name) {
	index = i;

	volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
	volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	volumeSlider.setRange(0, 100, 1);
	volumeSlider.setVelocityBasedMode(false);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);

	volumeLabel.setText(name, juce::dontSendNotification);
	volumeLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(volumeLabel);

	setSize(100, 100);
}

Channel::~Channel() 
{
	volumeSlider.removeListener(this);
}

void Channel::sliderValueChanged(juce::Slider* slider)
{
	volumeSlider.setValue(volumeSlider.getValue(), juce::dontSendNotification);
}

void Channel::resized()
{
	auto area = getLocalBounds();
	int xPos = index * 75;
	int sliderWidth = 75; 

	volumeLabel.setBounds(0, 0, area.getWidth(), 50);
	volumeSlider.setBounds(0, volumeLabel.getBottom(),
		area.getWidth(),
		area.getHeight() - volumeLabel.getHeight());
}

	//volumeLabel.setBounds(area.getX() + xPos, area.getY(), sliderWidth, 20);
	//volumeSlider.setBounds(area.getX() + xPos, volumeLabel.getBottom(), sliderWidth, area.getHeight() - volumeLabel.getHeight());