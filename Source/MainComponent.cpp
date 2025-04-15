//TODO: universal in&out min/max
#pragma once
#include <MainComponent.h>


// So MainComponent will inherit from component. 
//      Each input device will be its own AudioIODevice and will need its own AudioIODeviceCallback
//      Then, a custom AudioDeviceManager (Not that inherits from that, but replaces it) 
//      & a mixer class that recieves all input buffers

// AudioIODevice -> AudioProcessorPlayer:audioDeviceIOCallbackWithContext -> an AudioProcessor -> an audioProcessor graph -> some output
// Remember that integral to this plan is the 

// Required classes:
/*
*  MainComponent:Component
* Scan for device types and have the user select one, then pass that to v
*  InputDevice:AudioProcessorPlayer
*       The device class should turn the input channels of the device into a list of AudioProcessors in a graph and then the final graph is a compiliation of all the graphs
*  InputChannel:AudioProcessor
*       This is where the sliders come in
*  InputDeviceManager
* 
*/

/*
* MainComponent shouhld contain the following:
* GUI stuff, so for the IO portion of this addAndMakeVisible a mixer object
* 
* Mixer should take all the input & output info and position it
* So Mixer Component would be an apt description
* 
* each AudioInputDevice should place each individual slider in a row
* 
* I also need a lookandfeel class
* 
*/
std::unique_ptr<juce::AudioDeviceManager> deviceManager;

MainComponent::MainComponent() : Component()
{
    initializeDeviceManager();
    createMixer();
    createGuiElements();
    setSize(2000, 800); //TODO: Automatically reset size to match number of devices

    //deviceManager->setAudioChannels(20, 20);
   // createGuiElements();


}

MainComponent::~MainComponent() 
{
   deviceManager->removeAudioCallback(mixer.get());
   deviceManager->removeAllChangeListeners();
   deviceManager->closeAudioDevice();
   deviceManager.reset();
   menuBar.reset();
   removeAllChildren();
}

void MainComponent::initializeDeviceManager() {
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    
    deviceManager->initialise(20, 20, nullptr, true, juce::String(), nullptr);
    deviceManager->setCurrentAudioDeviceType(juce::String("ASIO"), true); //TODO: Add check here to make sure asio is installed
    auto setup = std::make_unique<juce::AudioDeviceManager::AudioDeviceSetup>();
    deviceManager->getAudioDeviceSetup(*setup);
    setup->inputDeviceName = "ASIO4ALL v2";
    setup->outputDeviceName = "ASIO4ALL v2";
    deviceManager->setAudioDeviceSetup(*setup, true);
}

void MainComponent::createMixer()
{
    mixer = std::make_unique<MainMixer>();
    addAndMakeVisible(*mixer);
    deviceManager->addAudioCallback(mixer.get());
}

void MainComponent::createGuiElements() {

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    menuModel.setListener(this);
    addAndMakeVisible(*menuBar);

}
void MainComponent::resetMixer()
{
   // deviceManager->removeAudioCallback(mixer.get());
   // removeChildComponent(*mixer);
   // createMixer();

}
void MainComponent::mainComponentEventTriggered()
{
   // resetMixer();
    //resized();
}

void MainComponent::resized() {
    addAndMakeVisible(*mixer);

    //textLabel.setBounds(10, 10, getWidth() - 20, 20);
    //audioDrivers.setBounds(10, 40, getWidth() - 20, 20);
    menuBar->setBounds(0, 0, getWidth(), 25);
    mainFlexBox.items.add(juce::FlexItem(*mixer).withMinWidth(getWidth()).withMinHeight(800));

    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 30, getWidth(), getHeight());

    mainFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlexBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mainFlexBox.performLayout(fbRect);
}
