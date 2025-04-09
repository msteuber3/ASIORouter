#include <AudioInputDevice.h>

AudioInputDevice::AudioInputDevice(const juce::String& inDevice, const juce::String& outDevice, juce::AudioIODeviceType* deviceType, int deviceInd, bool isInput)
	: Component(), inDeviceName(inDevice), outDeviceName(outDevice), deviceType(deviceType), deviceIndex(deviceInd), inputDevice(deviceType->createDevice(inDevice, outDevice)), isInput(isInput)
{
    deviceLabel.setText(inDeviceName, juce::dontSendNotification);
    deviceLabel.setJustificationType(juce::Justification::left);
    createGuiElements();
    createInputDevice();
}

AudioInputDevice::~AudioInputDevice() 
{
    inputDevice->close();
    channels.clear();
    removeAllChildren();
}

void AudioInputDevice::createGuiElements()
{
   // setSize(DEVICE_CONTAINER_WIDTH, 250);

    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(sliderComponent);

    juce::Rectangle<int> sbRect = juce::Rectangle<int>(0, 25, DEVICE_CONTAINER_WIDTH, 200);
    sliderComponent.setBounds(sbRect);
   

}

 bool AudioInputDevice::createInputDevice()
{
	//Might break with mics
    if (inputDevice == nullptr)
    {
        juce::Logger::writeToLog("Failed to create input device: " + inDeviceName);
        return false;
    }

    const juce::BigInteger& numInputs = inputDevice->getInputChannelNames().size();
    const juce::BigInteger& numOutputs = inputDevice->getOutputChannelNames().size();
    if (inputDevice->open(numInputs, numOutputs, 44100, 512) != "") {
        DBG("Failed to open device: " + inputDevice->getName());
        return false;
    }

    inputDevice->start(nullptr);
    if (!inputDevice->isOpen())
    {
        juce::Logger::writeToLog("Failed to start input device: " + inDeviceName);
		return false;
    }
    else
    {
        juce::Logger::writeToLog("Input device started successfully: " + inDeviceName);
    }
	return true;
}

int AudioInputDevice::createChannels(juce::Component* mainComponent)
{
    auto activeInputChannels = inputDevice->getActiveInputChannels();
    auto activeOutputChannels = inputDevice->getActiveOutputChannels();

    auto maxChannels = isInput ? activeInputChannels.getHighestBit() + 1 : activeOutputChannels.getHighestBit() + 1;
    DBG("Device: " + outDeviceName);
    for (int i = 0; i < maxChannels; i++) {
        juce::String name = isInput ? inputDevice->getInputChannelNames()[i] : inputDevice->getOutputChannelNames()[i];
        auto channel = std::make_unique<Channel>(i, deviceIndex, name);
        
        sliderComponent.addAndMakeVisible(*channel);
        
        sliderBox.items.add(juce::FlexItem(*channel).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));

        channels.push_back(std::move(channel));
    }

    return maxChannels;
}

void AudioInputDevice::resized()
{
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 0, DEVICE_CONTAINER_WIDTH, 250);
    addAndMakeVisible(sliderComponent);

    sliderBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    sliderBox.flexDirection = juce::FlexBox::Direction::row;
    sliderBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    sliderBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    deviceBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    deviceBox.flexDirection = juce::FlexBox::Direction::column;
    deviceBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    deviceBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    deviceBox.items.add(juce::FlexItem(deviceLabel).withMinWidth(DEVICE_CONTAINER_WIDTH).withMinHeight(20));

    sliderBox.performLayout(sliderComponent.getLocalBounds());

    deviceBox.items.add(juce::FlexItem(sliderComponent).withMinHeight(200).withMinWidth(DEVICE_CONTAINER_WIDTH));

    deviceBox.performLayout(fbRect);




}

