//TODO: universal in&out min/max

#include "MainComponent.h"


// So MainComponent will inherit from component. 
//      Each input device will be its own AudioIODevice and will need its own AudioIODeviceCallback
//      Then, a custom AudioDeviceManager (Not that inherits from that, but replaces it) 
//      & a mixer class that recieves all input buffers

// AudioIODevice -> AudioProcessorPlayer:audioDeviceIOCallbackWithContext -> an AudioProcessor -> an audioProcessor graph -> some output
// Remember that integral to this plan is the 

// Required classes:
/*
*  MainComponent:Component
*  InputDevice:AudioIODevice
*       The device class should turn the input channels of the device into a list of AudioProcessors in a graph and then the final graph is a compiliation of all the graphs
*  InputChannel:AudioProcessor
*  InputDeviceManager
* 
*/


MainComponent::MainComponent() : juce::AudioAppComponent(uniqueDeviceManager)
{
    uniqueDeviceManager.initialise(20, 2, nullptr, true);
    audioSetting.reset(new juce::AudioDeviceSelectorComponent(uniqueDeviceManager, 0, 20, 0, 2, true, true, true, true));
    addAndMakeVisible(audioSetting.get());




    setAudioChannels(20, 2);

    createChannels();

    setSize(1000, 800);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int, double) {
    // Initialize channels here when a source is loaded
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    // Alter volume here
}

void MainComponent::releaseResources() {
    // Delete channels here
}



void MainComponent::resized() {
    int channelWidth = 75;
    for (int i = 0; i < channels.size(); i++) {
        channels[i]->setBounds(i * channelWidth, 0, channelWidth, 300);
    }
    audioSetting->setBounds(10, 300, 300, 400);
}

void MainComponent::initializeMenu()
{

}

void MainComponent::createChannels()
{

    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

    for (int i = 0; i < maxInputChannels; i++) {
        auto channel = std::make_unique<Channel>(i, device->getInputChannelNames()[i]);
        addAndMakeVisible(*channel);
        channels.push_back(std::move(channel));
    }


}

//void MainComponent::paint (juce::Graphics& g)
//{
//    //// (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//    
//    g.setFont (juce::FontOptions (16.0f));
//    g.setColour (juce::Colours::white);
//    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
//
//    
//}

