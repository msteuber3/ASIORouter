#include "MainMixer.h"

// Device manager -> audioIOdevice -> send data to mixer

MainMixer::MainMixer(juce::AudioDeviceManager& mixerManager)
	: mixerDeviceManager(mixerManager)
{
    mixerDeviceManager.addAudioCallback(this);
}   

MainMixer::~MainMixer()
{}

void MainMixer::audioDeviceIOCallbackWithContext(const float* const * inputChannelData, int numInputChannels,
    float* const * outputChannelData, int numOutputChannels,
    int numSamples, const juce::AudioIODeviceCallbackContext& context)
{
    // Clear outputs first
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);

    // Get audio from the mixer (files and other sources)
    juce::AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);
    juce::AudioSourceChannelInfo info(&buffer, 0, numSamples);
    mixer.getNextAudioBlock(info);

    // Mix in live input channels
    for (int inputChannel = 0; inputChannel < numInputChannels; ++inputChannel)
    {
        // Apply channel routing/gain here if needed

        // Mix input to all output channels
        for (int outputChannel = 0; outputChannel < numOutputChannels; ++outputChannel)
        {
            if (inputChannelData[inputChannel] != nullptr &&
                outputChannelData[outputChannel] != nullptr)
            {
                // Mix with a gain factor (adjust as needed)
                float gain = 1.0f;

                // Add input to output
                juce::FloatVectorOperations::addWithMultiply(
                    outputChannelData[outputChannel],
                    inputChannelData[inputChannel],
                    gain,
                    numSamples);
            }
        }
    }
}


void MainMixer::InitializeInputDevices()
{
	auto* currentDevice = mixerDeviceManager->getCurrentAudioDevice();
}

void MainMixer::RefreshInputDevices()
{

}
