#include <Channel.h>

// Alternate approach: Make each channel a bus instead of a processor and let the user instantiate as many processors as they want. This way it does make a processor for each track. 
// Make one global processor(?) and route all audio through that (???) I just dont know if you can chain specific in channels to specific out channels
Channel::Channel(int index, int deviceIndex, juce::String name)
	: juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), juce::Slider::Listener(), juce::Component(), index(index), deviceIndex(deviceIndex)
{
	setBus();
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
	volumeSlider.setRange(-60.0f, +6.f, 0.1f);
	volumeSlider.setValue(0, juce::dontSendNotification);
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

void Channel::setBus()
{
	juce::AudioDeviceManager::AudioDeviceSetup config;
	deviceManager->getAudioDeviceSetup(config);

	juce::AudioProcessor::Bus* inputBus = getBus(true, 0);


	deviceManager->addAudioCallback(new juce::AudioProcessorPlayer());
	static_cast<juce::AudioProcessorPlayer*>(deviceManager->getAudioCallbackList()[0])->setProcessor(this);


}

void Channel::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	auto* channelData = buffer.getWritePointer(0);
	for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
		channelData[sample] *= volume;
	}
	rmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

}

