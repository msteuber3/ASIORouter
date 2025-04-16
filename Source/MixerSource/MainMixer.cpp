#pragma once
#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

// Remove scanCurrentDriver from constructor

MainMixer::MainMixer() : 
    juce::Component()
{ }


MainMixer::~MainMixer()
{
    inChannels.clear();
    outChannels.clear();

    removeAllChildren();
}

void MainMixer::createJuceDevices(juce::AudioIODeviceType* deviceType)
{
    deviceType->scanForDevices();

    auto deviceNames = deviceType->getDeviceNames();

    for (const juce::String& name : deviceNames) {
        juceDevices.add(std::make_unique<JuceAsioDevice>(deviceType->createDevice(name, name)));
    }
}

void MainMixer::createBridgeDevices(juce::AudioIODeviceType* deviceType) {

    auto inputDeviceNames = deviceType->getDeviceNames(true);
    auto outputDeviceNames = deviceType->getDeviceNames(false);

    for (const juce::String& name : inputDeviceNames) {
        auto device = deviceType->createDevice(name, name);
        jack_client_t* jackClient = jack_client_open(name.toRawUTF8(), JackNullOption, NULL);
        if (jackClient) {
            DBG("JACK client " + name + " opened successfully");
            inputDevices.add(std::make_unique<JackDeviceBridge>(device, jackClient, name.toRawUTF8()));
        }
        else {
            DBG("Failed to create JACK client " + name);
        }
    }

    for (const juce::String& name : outputDeviceNames) {
        auto device = deviceType->createDevice(name, name);
           jack_client_t* jackClient = jack_client_open(name.toRawUTF8(), JackNullOption, NULL);
           if (jackClient) {
               DBG("JACK client " + name + " opened successfully");
               outputDevices.add(std::make_unique<JackDeviceBridge>(device, jackClient, name.toRawUTF8()));
           }
           else {
               DBG("Failed to create JACK client " + name);
           }
    }
}

void MainMixer::createGUI()
{
    if (!juceDevices.isEmpty()) {
        for (auto device : juceDevices) {
            device->createGUI();
            addAndMakeVisible(device);
            mixerBox.items.add(juce::FlexItem(*device).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        }
    }
}

void MainMixer::createBridgeGUI()
{
    if (!inputDevices.isEmpty()) {
        for (JackDeviceBridge* bridge : inputDevices) {
            bridge->createGUI();
            addAndMakeVisible(bridge);
            mixerBox.items.add(juce::FlexItem(*bridge).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        }
    }
    if (!outputDevices.isEmpty()) {
        for (JackDeviceBridge* bridge : outputDevices) {
            bridge->createGUI();
            addAndMakeVisible(bridge);
            mixerBox.items.add(juce::FlexItem(*bridge).withMinWidth(SLIDER_WIDTH).withMinHeight(200.0f));
        }
    }
}

void MainMixer::resized() {
    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 10, getLocalBounds().getWidth(), 500);

    mixerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mixerBox.flexDirection = juce::FlexBox::Direction::column;
    mixerBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mixerBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mixerBox.performLayout(fbRect);
}