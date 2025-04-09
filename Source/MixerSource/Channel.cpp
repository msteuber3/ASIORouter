#include <Channel.h>

// Alternate approach: Make each channel a bus instead of a processor and let the user instantiate as many processors as they want. This way it does make a processor for each track. 
// Make one global processor(?) and route all audio through that (???) I just dont know if you can chain specific in channels to specific out channels
Channel::Channel(int index, int deviceIndex, juce::String name)
	: juce::Slider::Listener(), juce::Component(), juce::Timer(), index(index), deviceIndex(deviceIndex), rmsLevelSnapshot(0.f)
{
	startTimerHz(30);
	createSlider(name);
	addAndMakeVisible(verticalMeter);
	setSize(SLIDER_WIDTH, 200);
}

Channel::~Channel() 
{
	volumeSlider.removeListener(this);
}



void Channel::createSlider(juce::String labelName)
{

	volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
	volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	volumeSlider.setRange(-60.0f, +6.f, 0.1f);
	volumeSlider.setValue(0, juce::dontSendNotification);
	volumeSlider.setVelocityBasedMode(false);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);
	volume = juce::Decibels::decibelsToGain(volumeSlider.getValue());

	volumeLabel.setText(labelName, juce::dontSendNotification);
	volumeLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(volumeLabel);
}

void Channel::sliderValueChanged(juce::Slider* slider)
{
	volumeSlider.setValue(volumeSlider.getValue(), juce::dontSendNotification);
	volume = juce::Decibels::decibelsToGain(volumeSlider.getValue());
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
	volumeLabel.setBounds(0, 0, SLIDER_WIDTH, 50);
	volumeSlider.setBounds(0, 50, SLIDER_WIDTH, 150);

	auto meterWidth = SLIDER_WIDTH / 5;
	verticalMeter.setBounds((SLIDER_WIDTH / 2) - (meterWidth / 2), 50, meterWidth, 130);
	verticalMeter.toBack();
}

void Channel::process(float* channelData, int numSamples)
{
	for (int i = 0; i < numSamples; i++) {
		channelData[i] *= volume;
	}
	//rmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
}

void Channel::setRMSLevel(float rmsLevel) {
	rmsLevelSnapshot = rmsLevel;
}

void Channel::timerCallback()
{
	verticalMeter.setLevel(rmsLevelSnapshot);
}

