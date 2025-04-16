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

MainComponent::MainComponent() : Component()
{
    jackClientName = "JuceJackRouter";
    startJackServer();
    initializeDeviceManager();
    createMixer();
    createGuiElements();
    setSize(2000, 800); //TODO: Automatically reset size to match number of devices

    //deviceManager->setAudioChannels(20, 20);
   // createGuiElements();


}

MainComponent::~MainComponent() 
{
   deviceManager->removeAllChangeListeners();
   deviceManager->closeAudioDevice();
   deviceManager.reset();
  // menuBar.reset();
   jack->stop();
   //removeAllChildren();
}

void MainComponent::startJackServer()
{
    jack = std::make_unique<JackWrapper>(jackClientName);
    bool result = jack->start();
    if (!result) { DBG("Jack client failed to start"); }
}

void MainComponent::initializeDeviceManager() {
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    
    deviceManager->initialise(20, 20, nullptr, true, juce::String(), nullptr);

    deviceManager->createAudioDeviceTypes(deviceTypes);

    for (auto& type : deviceTypes)
    {
        type->scanForDevices();

        DBG("Device type: " + type->getTypeName());
        DBG("Devices: " + type->getDeviceNames().joinIntoString(", "));

        if (type->getTypeName().containsIgnoreCase("jack")) // Look for "JACK"
        {
            deviceManager->setCurrentAudioDeviceType(type->getTypeName(), true);

            juce::StringArray deviceNames = type->getDeviceNames(true); // true = input devices

            juce::String deviceName;

            if (deviceNames.size() > 0)
            {
                for (juce::String name : deviceNames)
                {
                    if (name.contains(jackClientName)) {
                        deviceName = name;
                    }
                }
            }
            else {
                deviceName = deviceNames[0];
            }

            juce::AudioDeviceManager::AudioDeviceSetup setup;
            deviceManager->getAudioDeviceSetup(setup);
            setup.inputDeviceName = deviceName;
            setup.outputDeviceName = deviceName;
            setup.useDefaultInputChannels = true;
            setup.useDefaultOutputChannels = true;
            DBG("Set device " + deviceName);
            auto result = deviceManager->setAudioDeviceSetup(setup, true);
            if (result != "")
                 DBG("Failed to set JACK device: " + result);
            

            break;
        }
    }
}

void MainComponent::createMixer()
{
    int jackIns = jack->getNumInputChannels();
    int jackOuts = jack->getNumOutputChannels();
    mixer = std::make_unique<MainMixer>(jackIns, jackOuts);
    jack->setAudioCallback(mixer.get());
    addAndMakeVisible(*mixer);
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
