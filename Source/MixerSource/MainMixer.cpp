#pragma once
#include <MainMixer.h>

// Device manager -> audioIOdevice -> send data to mixer

// Remove scanCurrentDriver from constructor

MainMixer::MainMixer() :
    juce::Component(),
    audioDeviceManager(std::make_unique<juce::AudioDeviceManager>())
{
        mixerBox.flexDirection = juce::FlexBox::Direction::row;
        mixerBox.flexWrap = juce::FlexBox::Wrap::wrap;
    if (!generateDeviceType()) {
        handleDeviceNotFound();
    }
    if (activateJackVirtualDevice)
        startJack();

}


MainMixer::~MainMixer()
{
    removeAllChildren();
}

bool MainMixer::generateDeviceType()
{
    switch (DEFAULT_DEVICE_TYPE) {
    case(1):
        deviceType = std::unique_ptr<juce::AudioIODeviceType>(juce::AudioIODeviceType::createAudioIODeviceType_WASAPI(juce::WASAPIDeviceMode::shared));
        createWASAPIDevices();
        return true;
    case(2):
        deviceType = std::unique_ptr<juce::AudioIODeviceType>(juce::AudioIODeviceType::createAudioIODeviceType_ASIO());
        createASIODevices();
        calculatePreferredSize();
        return true;
    default:
        DBG("Audio device type " + juce::String(DEFAULT_DEVICE_TYPE) + " not found");
        return false;
    }
}

void MainMixer::createWASAPIDevices()
{
    auto bounds = getLocalBounds();
    WASAPIContainer = std::make_unique<JuceWASAPIDeviceContainer>(std::move(deviceType));
    addAndMakeVisible(*WASAPIContainer);
    mixerBox.items.add(juce::FlexItem(*WASAPIContainer).withFlex(1).withMinWidth(bounds.getWidth()).withMinHeight(bounds.getHeight()));
}

void MainMixer::createASIODevices()
{
    audioDeviceManager->initialise(20, 20, nullptr, true);

    auto& deviceTypes = audioDeviceManager->getAvailableDeviceTypes();
    for (auto* type : deviceTypes)
    {
        if (type->getTypeName() == "ASIO")
        {
            type->scanForDevices();

            auto devices = type->getDeviceNames();
            for (const auto& name : devices)
            {
                if (name.containsIgnoreCase("focusrite usb"))
                {
                    auto* device = type->createDevice(name, name);
                    auto juceAsioDevice = std::make_unique<JuceAsioDevice>(device);
                    addAndMakeVisible(juceAsioDevice.get());
                    mixerBox.items.add(juce::FlexItem(*juceAsioDevice).withMinWidth(juceAsioDevice->getWidth()).withMinHeight(juceAsioDevice->getHeight()).withFlex(1.0f));
                    juceDevices.add(std::move(juceAsioDevice));
                }
            }
        }
    }
}

    /*
    deviceType->scanForDevices();

    auto deviceNames = deviceType->getDeviceNames();

    for (const juce::String& name : deviceNames) {
        auto newAudioDevice = deviceType->createDevice(name, name);
        if (!newAudioDevice->getInputChannelNames().isEmpty() || !newAudioDevice->getOutputChannelNames().isEmpty()) {
            auto device = std::make_unique<JuceAsioDevice>(newAudioDevice);
            addAndMakeVisible(device.get());
            mixerBox.items.add(juce::FlexItem(*device).withMinWidth(device->getWidth()).withMinHeight(device->getHeight()).withFlex(1.0f));
            juceDevices.add(std::move(device));
        }
        else {
            newAudioDevice->close();
        }
    }

}
*/

void MainMixer::calculatePreferredSize()
{
    float totalWidth = 0.0f;
    float maxHeight = 0.0f;

    for (auto& device : juceDevices)
    {
        auto deviceSize = device->getPreferredSize();
        totalWidth += deviceSize.getX();
        maxHeight = juce::jmax(maxHeight, deviceSize.getY());
    }

    preferredSize = juce::Point<float>(totalWidth, maxHeight);
}

void MainMixer::deleteDevice(int index)
{
    juceDevices[index]->getAudioDevice()->stop();
    juceDevices[index]->getAudioDevice()->close();
    juceDevices.remove(index);
}

void MainMixer::handleDeviceNotFound()
{
    deviceNotFoundLabel.setText("Default audio device type " + juce::String(DEFAULT_DEVICE_TYPE) + " not found. You can change the device in options->properties, but odds are this is a lower level application issue.", juce::dontSendNotification);
    deviceNotFoundLabel.setJustificationType(juce::Justification::centred);
    deviceNotFoundLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    addAndMakeVisible(deviceNotFoundLabel);
    mixerBox.items.add(juce::FlexItem(deviceNotFoundLabel).withHeight(30.0f).withMinWidth(100).withFlex(1));
    setSize(500, 300);
}

void MainMixer::startJack()
{
    jackWrapper = std::make_unique<JackWrapper>("VirtualJackDevice");
    addAndMakeVisible(*jackWrapper);
    mixerBox.items.add(juce::FlexItem(*jackWrapper).withFlex(1));
    jackWrapper->start();
    jackWrapper->addChannel(juceDevices[0]->getBuffer(true, 0), true, "testDevice");
}

juce::Point<float> MainMixer::getPreferredSize() const
{
    return preferredSize;
}

void MainMixer::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);
}

void MainMixer::resized() {
    DBG("MainMixer bounds: " + getLocalBounds().toString());

    mixerBox.performLayout(getLocalBounds().reduced(10));

    if (WASAPIContainer != nullptr) {
        DBG("WASAPIContainer bounds: " + WASAPIContainer->getBounds().toString());
    }
}

// Banished to the end of the source file until I feel like dealing with them

void MainMixer::createBridgeDevices() {

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

