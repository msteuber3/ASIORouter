//TODO: universal in&out min/max
#pragma once
#include <MainComponent.h>
/*
*TODO:
* OUTPUTS
* routing
* figure out how to handle read only input buffers
* extract channel name info from asio4all or figure out a workaround
* figure out how to handle stereo ins
*  more in depth audio io channel comparriosn testing - I think this only worked cause I turned on the headphones which were already enabled in a4a
* Documentation
* bug checking
* 
* 
* 
* With JACK, I want to get all ports and THEN worry about connections. Right now the concern is how to get ports. The JackRouter.ini file is probably the answer, but
* how do I make a port? there's jack_port_register(client, 
*/
std::unique_ptr<juce::AudioDeviceManager> deviceManager;

MainComponent::MainComponent() : juce::Component()
{
    initializeDeviceManager();
    setSize(2000, 800); //TODO: Automatically reset size to match number of devices
}

MainComponent::~MainComponent() 
{
   deviceManager->removeAllChangeListeners();
   if (deviceManager->getCurrentAudioDevice()) {
       deviceManager->closeAudioDevice();
   }
   deviceManager.reset();
   menuBar.reset();
   removeAllChildren();
}

void MainComponent::initializeDeviceManager() {
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    
    deviceManager->initialise(20, 20, nullptr, true, juce::String(), nullptr);
    deviceManager->createAudioDeviceTypes(deviceTypes);

    for (auto type : deviceTypes)
    {       
        DBG("Device type: " + type->getTypeName());

        if (type->getTypeName().containsIgnoreCase("asio")) 
        {
            type->scanForDevices();
            deviceManager->setCurrentAudioDeviceType(type->getTypeName(), true);
            createMixer(type);
            break;
        }
    }
}

void MainComponent::createMixer(juce::AudioIODeviceType* deviceType)
{
    mixer = std::make_unique<MainMixer>();
    mixer->createJuceDevices(deviceType);
}

void MainComponent::createGuiElements() {

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);
    mixer->createGUI();
    addAndMakeVisible(*mixer);
}

void MainComponent::resized() {
    createGuiElements();

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
