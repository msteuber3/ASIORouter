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
    setBusOrder(inputBuses, outputBuses);
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

void MainMixer::setBusOrder(
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses,
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses) 
{
    std::map<int, juce::String> inputs;
    std::map<int, juce::String> outputs;
    int totalChannelIndex = 0;
    int deviceIndex = 0;
    for (std::tuple<juce::AudioChannelSet, juce::String> IOTuple : inputBuses) {
        if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::centre)
        { 
            inputs[totalChannelIndex] = std::get<1>(IOTuple) + " Input Channel 1"; 
            totalChannelIndex++;
            deviceIndex++;
        }
        else if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::left)
        {
            inputs[totalChannelIndex] = std::get<1>(IOTuple) + " Input Channel 1";
            totalChannelIndex++;
            inputs[totalChannelIndex] = std::get<1>(IOTuple) + " Input Channel 2";
            totalChannelIndex++;
            deviceIndex++;
        }
        else if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::discreteChannel0)
        {
            DBG("Sorry future Michael, have fun with this; I'm feeling quite lazy");
        }
    }
    for (std::tuple<juce::AudioChannelSet, juce::String> IOTuple : outputBuses) {
        if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::centre)
        {
            outputs[totalChannelIndex] = std::get<1>(IOTuple) + " Output Channel 1";
            totalChannelIndex++;
            deviceIndex++;
        }
        else if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::left)
        {
            outputs[totalChannelIndex] = std::get<1>(IOTuple) + " Output Channel 1";
            totalChannelIndex++;
            outputs[totalChannelIndex] = std::get<1>(IOTuple) + " Output Channel 2";
            totalChannelIndex++;
            deviceIndex++;
        }
        else if (std::get<0>(IOTuple).getTypeOfChannel(0) == juce::AudioChannelSet::ChannelType::discreteChannel0)
        {
            DBG("Sorry future Michael, have fun with this; I'm feeling quite lazy");
        }
    }

}

// Create devices from the current selected driver
void MainMixer::ScanCurrentDriver() {

    // If no driver is selected, print das debug statement and return
    if (!deviceType) {
        DBG("Error: No valid device type found.");
        return;
    }

    // Get the input and ouput device names as juce::StringArrays 
    deviceType->scanForDevices();
    inDeviceNames = deviceType->getDeviceNames(true);
    outDeviceNames = deviceType->getDeviceNames(false);

    // Device index
    int ind = 0;
    // Channel index
 //   int channelIndex = 0;
    // for input device name in list of input devices from deviceType's getDeviceName()
    for (const juce::String& name : inDeviceNames) {
        // Create new AudioInputDevice (name = name, name; deviceType = current device type, ind = device index, input = true
        auto inDevice = std::make_unique<AudioInputDevice>(name, name, deviceType, ind, true);

        // Create device channels, return number of input devices
        int maxIn = inDevice->createChannels(this);

        // move the new device to the inputDeviecs vector
        inputDevices.push_back(std::move(inDevice));

        // Increment input devices
        ind++;

        // for channel in channels in the input device
      /////////////  for (int inChannel = 0; inChannel < maxIn; inChannel++) {
      /////////////      // InputDevices is a vector of AudioInputDevices, which each contain a vector of Channels
      /////////////      // This checks if the channel exists
      /////////////      if (inputDevices.back()->channels[inChannel]) {  
      /////////////          // Then it moves that channel to the inChannels vector, REMOVING IT from the inputDevice->channels vector
      /////////////          // AFAIK, this works fine
      /////////////
      /////////////          // Can we remove inChannels and make this reliant on the AudioInputChannel object?
      /////////////          inChannels.push_back(std::move(inputDevices.back()->channels[inChannel]));
      /////////////          //Increment the channel index
      /////////////          channelIndex++;
      /////////////      }
      /////////////  }
        // Then add the input device to the flexbox. This could be problematic, as we aren't copying the pointer; we're moving it.
        // So inputDevices.back will not contain the slider we need (I think)
        // inputDevices.back is the most recent input device; the component we are adding relies on the channels vector that we remove items from; 
        // therefore, we should handle everything in AudioInputDevice here OR make sure we are creating the GUI components here at least. 
        // NEXT STEP: Check all instances of AudioInputDevice creation and see how much we can add here
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

     ////////////   for (int outChannel = 0; outChannel < maxOut; outChannel++) {
     ////////////       if (outputDevices.back()->channels[outChannel]) {
     ////////////           outChannels.push_back(std::move(outputDevices.back()->channels[outChannel]));
     ////////////
     ////////////       }
     ////////////   }
        outputComponent.addAndMakeVisible(*outputDevices.back());
        outputBox.items.add(juce::FlexItem(*outputDevices.back()).withMinHeight(250.0f).withMinWidth(DEVICE_CONTAINER_WIDTH));
    }
}

std::unique_ptr<Channel> MainMixer::getChannelFromBusBuffer() {
    return nullptr;
}

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

void MainMixer::InitializeInputDevices(){}

void MainMixer::RefreshInputDevices(){}

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
    //So the bus corresponds to the AudioInputDevice
    int busCount = getBusCount(true);
    for (int busIndex = 0; busIndex < busCount; busIndex++) {
        auto bus = getBus(true, busIndex);

        // ISSUE: ONLY THE FIRST LOADED BUS IS GETTING CHANNELS
        juce::AudioBuffer<float> busBuffer = bus->getBusBuffer(buffer);

        for (int channel = 0; channel < busBuffer.getNumChannels(); ++channel)
        {
            // And the channel is relative to that. SO let's auto AudioInputDevice and then access the channel by index based on that
            int numSamples = busBuffer.getNumSamples();
            float* data = busBuffer.getWritePointer(getChannelIndexInProcessBlockBuffer(true, busIndex, channel));
            inputDevices[busIndex]->channels[channel]->process(data, busBuffer.getNumSamples());
            float rmsLevel = juce::Decibels::gainToDecibels(busBuffer.getRMSLevel(getChannelIndexInProcessBlockBuffer(true, busIndex, channel), 0, busBuffer.getNumSamples()));
            inputDevices[busIndex]->channels[channel]->setRMSLevel(rmsLevel);
        }
    }
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

