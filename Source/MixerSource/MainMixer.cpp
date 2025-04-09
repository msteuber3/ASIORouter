#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

// Remove scanCurrentDriver from constructor

MainMixer::MainMixer(juce::AudioIODeviceType* deviceType, 
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses,
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses)
    : juce::Component(), 
    juce::AudioProcessor(buildBuses(inputBuses, outputBuses)),
    deviceType(deviceType)
{
    outputGraph = std::make_unique<juce::AudioProcessorGraph>();
    addAndMakeVisible(inputComponent);
    addAndMakeVisible(outputComponent);
    ScanCurrentDriver();
}   

juce::AudioProcessor::BusesProperties MainMixer::buildBuses(
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses,
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses) 
{
    juce::AudioProcessor::BusesProperties busesProps;
    for (std::tuple<juce::AudioChannelSet, juce::String> IOTuple : inputBuses) {
        busesProps.addBus(true, std::get<1>(IOTuple), std::get<0>(IOTuple), true);
    }
    for (std::tuple<juce::AudioChannelSet, juce::String> IOTuple : outputBuses) {
        busesProps.addBus(false, std::get<1>(IOTuple), std::get<0>(IOTuple), true);
    }
    return busesProps;

}

MainMixer::~MainMixer()
{
    outputGraph->clear();
    inputDevices.clear();
}

void MainMixer::ScanCurrentDriver() {

    if (!deviceType) {
        DBG("Error: No valid device type found.");
        return;
    }

    deviceType->scanForDevices();
    inDeviceNames = deviceType->getDeviceNames(true);
    outDeviceNames = deviceType->getDeviceNames(false);

    // Input device setup
    int ind = 0;
    int channelIndex = 0;
    for (const juce::String& name : inDeviceNames) {
        auto inDevice = std::make_unique<AudioInputDevice>(name, name, deviceType, ind, true);
        int maxIn = inDevice->createChannels(this);

        inputDevices.push_back(std::move(inDevice));
        ind++;

        for (int inChannel = 0; inChannel < maxIn; inChannel++) {
            if (inputDevices.back()->channels[inChannel]) {  
                inChannels.push_back(std::move(inputDevices.back()->channels[inChannel]));
                channelIndex++;
            }
        }
        inputComponent.addAndMakeVisible(*inputDevices.back());
        inputBox.items.add(juce::FlexItem(*inputDevices.back()).withMinHeight(250.0f).withMinWidth(DEVICE_CONTAINER_WIDTH));
    }

    // Output device setup
    ind = 0;
    for (const juce::String& name : outDeviceNames) {
        if(name.contains("Realtek Digital Output")) { 
            DBG("REALTEK NAME:" + name);
            break;
        }
        auto outDevice = std::make_unique<AudioInputDevice>(name, name, deviceType, ind, false);
        int maxOut = outDevice->createChannels(this);

        outputDevices.push_back(std::move(outDevice));
        ind++;

        for (int outChannel = 0; outChannel < maxOut; outChannel++) {
            if (outputDevices.back()->channels[outChannel]) {
                outChannels.push_back(std::move(outputDevices.back()->channels[outChannel]));

            }
        }
        outputComponent.addAndMakeVisible(*outputDevices.back());
        outputBox.items.add(juce::FlexItem(*outputDevices.back()).withMinHeight(250.0f).withMinWidth(DEVICE_CONTAINER_WIDTH));
    }
}

//void MainMixer::addChannelToBus(int busIndex)
//{
//    int numChannels = getBus(true, 0)->getNumberOfChannels();
//
//    auto layout = getBus(true, 0)->getCurrentLayout();
//
//    layout.addChannel(juce::AudioChannelSet::unknown);
//
//    getBus(true, 0)->setCurrentLayout(layout);
//
//    setBusesLayout(getBus(true, 0)->getBusesLayoutForLayoutChangeOfBus(layout));
//}

void MainMixer::resized() {
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 10, getLocalBounds().getWidth(), 500);
 
    inputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    inputBox.flexDirection = juce::FlexBox::Direction::row;
    inputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    inputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    outputBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    outputBox.flexDirection = juce::FlexBox::Direction::row;
    outputBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    outputBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mixerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mixerBox.flexDirection = juce::FlexBox::Direction::column;
    mixerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mixerBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    
    inputBox.performLayout(inputComponent.getLocalBounds());
    mixerBox.items.add(juce::FlexItem(inputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    outputBox.performLayout(outputComponent.getLocalBounds());
    mixerBox.items.add(juce::FlexItem(outputComponent).withMinWidth(getLocalBounds().getWidth()).withMinHeight(250));

    mixerBox.performLayout(fbRect);
}

void MainMixer::InitializeInputDevices()
{
	//auto* currentDevice = mixerDeviceManager->getCurrentAudioDevice();
}

void MainMixer::RefreshInputDevices()
{

}


//

bool MainMixer::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return true;
}

const juce::String MainMixer::getName() const
{
    return juce::String();
}

void MainMixer::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    juce::ignoreUnused(sampleRate, maximumExpectedSamplesPerBlock);
}

void MainMixer::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (int busIndex = 0; busIndex < getBusCount(true); busIndex++) {
        auto bus = getBus(true, busIndex);

        juce::AudioBuffer<float> busBuffer = bus->getBusBuffer(buffer);

        for (int channel = 0; channel < busBuffer.getNumChannels(); ++channel)
        {
            int numSamples = busBuffer.getNumSamples();
            float* data = busBuffer.getWritePointer(bus->getChannelIndexInProcessBlockBuffer(channel));
            inChannels[channel]->process(data, busBuffer.getNumSamples());
            float rmsLevel = juce::Decibels::gainToDecibels(busBuffer.getRMSLevel(bus->getChannelIndexInProcessBlockBuffer(channel), 0, busBuffer.getNumSamples()));
            inChannels[channel]->setRMSLevel(rmsLevel);
        }
    }
 //   rmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));

}

void MainMixer::releaseResources()
{
}

double MainMixer::getTailLengthSeconds() const
{
    return 0.0;
}

bool MainMixer::acceptsMidi() const
{
    return false;
}

bool MainMixer::producesMidi() const
{
    return false;
}

juce::AudioProcessorEditor* MainMixer::createEditor()
{
    return nullptr;
}

bool MainMixer::hasEditor() const
{
    return false;
}

int MainMixer::getNumPrograms()
{
    return 0;
}

int MainMixer::getCurrentProgram()
{
    return 0;
}

void MainMixer::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String MainMixer::getProgramName(int index)
{
    return juce::String();
}

void MainMixer::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);

}

void MainMixer::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);

}

void MainMixer::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

