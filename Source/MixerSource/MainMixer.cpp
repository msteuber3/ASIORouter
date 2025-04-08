#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

MainMixer::MainMixer(juce::AudioIODeviceType* deviceType) : juce::Component(), deviceType(deviceType)
{
    outputGraph = std::make_unique<juce::AudioProcessorGraph>();
    ScanCurrentDriver();
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
    int x;
    int y;
    deviceType->scanForDevices();
    inDeviceNames = deviceType->getDeviceNames(true);
    outDeviceNames = deviceType->getDeviceNames(false);
    int ind = 0;
    for (const juce::String& name : inDeviceNames) {
        // Create input device
        auto inDevice = std::make_unique<AudioInputDevice>(name, name, deviceType, ind);
        int maxIn = inDevice->createChannels(this);

        // Move unique_ptr into vector
        inputDevices.push_back(std::move(inDevice));
        ind++;

        // Add channels to the graph
        for (int inChannel = 0; inChannel < maxIn; inChannel++) {
            if (inputDevices.back()->channels[inChannel]) {  // Ensure valid pointer
                outputGraph->addNode(std::move(inputDevices.back()->channels[inChannel]));

            }
        }
        addAndMakeVisible(*inputDevices.back());
        mixerBox.items.add(juce::FlexItem(*inputDevices.back()).withMinHeight(200.0f).withMinWidth(DEVICE_CONTAINER_WIDTH));


    }
}

void MainMixer::resized() {
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 75, DEVICE_CONTAINER_WIDTH, 200);

    mixerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mixerBox.flexDirection = juce::FlexBox::Direction::row;
    mixerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mixerBox.alignContent = juce::FlexBox::AlignContent::flexStart;
    
    mixerBox.performLayout(fbRect);
}

//void MainMixer::audioDeviceIOCallbackWithContext(const float* const * inputChannelData, int numInputChannels,
//    float* const * outputChannelData, int numOutputChannels,
//    int numSamples, const juce::AudioIODeviceCallbackContext& context)
//{
//    // Clear outputs first
//    for (int channel = 0; channel < numOutputChannels; ++channel)
//        if (outputChannelData[channel] != nullptr)
//            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
//
//    // Get audio from the mixer (files and other sources)
//    juce::AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);
//    juce::AudioSourceChannelInfo info(&buffer, 0, numSamples);
//    mixer.getNextAudioBlock(info);
//
//    // Mix in live input channels
//    for (int inputChannel = 0; inputChannel < numInputChannels; ++inputChannel)
//    {
//        // Apply channel routing/gain here if needed
//
//        // Mix input to all output channels
//        for (int outputChannel = 0; outputChannel < numOutputChannels; ++outputChannel)
//        {
//            if (inputChannelData[inputChannel] != nullptr &&
//                outputChannelData[outputChannel] != nullptr)
//            {
//                // Mix with a gain factor (adjust as needed)
//                float gain = 1.0f;
//
//                // Add input to output
//                juce::FloatVectorOperations::addWithMultiply(
//                    outputChannelData[outputChannel],
//                    inputChannelData[inputChannel],
//                    gain,
//                    numSamples);
//            }
//        }
//    }
//}


void MainMixer::InitializeInputDevices()
{
	//auto* currentDevice = mixerDeviceManager->getCurrentAudioDevice();
}

void MainMixer::RefreshInputDevices()
{

}
