#include "Channel.h"


Channel::Channel(int index, int deviceIndex, juce::String name)
	: juce::AudioProcessor(), juce::Slider::Listener(), juce::Component(), index(index), deviceIndex(deviceIndex)
{
	createSlider(name);
	addAndMakeVisible(verticalMeter);
	setSize(SLIDER_WIDTH, 200);
	startTimerHz(24);
}

Channel::~Channel() 
{

	releaseResources();
	volumeSlider.removeListener(this);
}



void Channel::createSlider(juce::String labelName)
{

	volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
	volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	volumeSlider.setRange(0, 2, 0.01);
	volumeSlider.setVelocityBasedMode(false);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);

	volumeLabel.setText(labelName, juce::dontSendNotification);
	volumeLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(volumeLabel);
}

void Channel::sliderValueChanged(juce::Slider* slider)
{
	volumeSlider.setValue(volumeSlider.getValue(), juce::dontSendNotification);
	volume = volumeSlider.getValue();
}

int Channel::getXCoord() {
	return index * 100;
 }
// Height is 150 for the slider, 50 for the label
// Width is 100 for both
int Channel::getYCoord() {
	return (200 * deviceIndex) + 50;
}

void Channel::resized()
{

	//int xPos = index * 75;
	//int yPos = (200 * deviceIndex) + 50;
	//int sliderWidth = 75; 
	//auto area = juce::Rectangle(xPos, yPos, 100, 200);
	//
	volumeLabel.setBounds(0, 0, SLIDER_WIDTH, 50);
	volumeSlider.setBounds(0, 50, SLIDER_WIDTH, 150);

	auto meterWidth = SLIDER_WIDTH / 5;
	verticalMeter.setBounds((SLIDER_WIDTH / 2) - (meterWidth / 2), 50, meterWidth, 150);
	verticalMeter.toBack();
	//	area.getWidth(),
	//	area.getHeight() - volumeLabel.getHeight());
	//toFront(false);
}

const juce::String Channel::getName() const
{
	return juce::String();
}

void Channel::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	juce::ignoreUnused(sampleRate, maximumExpectedSamplesPerBlock);
}

void Channel::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ignoreUnused(midiMessages);

	auto* channelData = buffer.getWritePointer(0);
	for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
		channelData[sample] *= volume;
	}
	rmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

}

void Channel::timerCallback() {
	verticalMeter.setLevel(rmsLevel);
	verticalMeter.repaint();
}

void Channel::releaseResources()
{
}

double Channel::getTailLengthSeconds() const
{
	return 0.0;
}

bool Channel::acceptsMidi() const
{
	return false;
}

bool Channel::producesMidi() const
{
	return false;
}

juce::AudioProcessorEditor* Channel::createEditor()
{
	return nullptr;
}

bool Channel::hasEditor() const
{
	return false;
}

int Channel::getNumPrograms()
{
	return 0;
}

int Channel::getCurrentProgram()
{
	return 0;
}

void Channel::setCurrentProgram(int index)
{
	juce::ignoreUnused(index);
}

const juce::String Channel::getProgramName(int index)
{
	return juce::String();
}

void Channel::changeProgramName(int index, const juce::String& newName)
{
	juce::ignoreUnused(index, newName);

}

void Channel::getStateInformation(juce::MemoryBlock& destData)
{
	juce::ignoreUnused(destData);

}

void Channel::setStateInformation(const void* data, int sizeInBytes)
{
	juce::ignoreUnused(data, sizeInBytes);
}

	//volumeLabel.setBounds(area.getX() + xPos, area.getY(), sliderWidth, 20);
	//volumeSlider.setBounds(area.getX() + xPos, volumeLabel.getBottom(), sliderWidth, area.getHeight() - volumeLabel.getHeight());