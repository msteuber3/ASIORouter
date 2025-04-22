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

MainComponent::MainComponent() : juce::Component()
{   
    createMixer();
    createGuiElements();
    setSize(2000, 800); //TODO: Automatically reset size to match number of devices
}

MainComponent::~MainComponent() 
{
   menuBar.reset();
   removeAllChildren();
}

void MainComponent::createMixer()
{
    mixer = std::make_unique<MainMixer>();
}

void MainComponent::createGuiElements() {

    menuBar.reset(new juce::MenuBarComponent(&menuModel));

    addAndMakeVisible(menuBar.get());
    addAndMakeVisible(mixer.get());

    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.items.add(juce::FlexItem(*menuBar).withHeight(30.0f).withFlex(0));
    mainFlexBox.items.add(juce::FlexItem(*mixer).withFlex(1.0f));
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized() {
    auto bounds = getLocalBounds();
    mainFlexBox.performLayout(bounds);
}
