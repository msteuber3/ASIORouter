#pragma once
#include <PropertiesComponent.h>

PropertiesComponent::PropertiesComponent()
{
    generateDriverDropdown();
}

PropertiesComponent::~PropertiesComponent()
{
    deviceTypes.clear();
    audioDrivers.clear();
}

void PropertiesComponent::generateDriverDropdown(){
    juce::Font textFont{ 12.0f };
    label.setFont(textFont);
    addAndMakeVisible(label);
    addAndMakeVisible(audioDrivers);
    deviceManager->createAudioDeviceTypes(deviceTypes);

    //mainFlexBox.items.add(juce::FlexItem(deviceTypeLabel).withMinWidth(20).withMinHeight(10));
    for (size_t i = 0; i < deviceTypes.size(); i++) {
        audioDrivers.addItem(deviceTypes[i]->getTypeName(), i + 1);
    }
    audioDrivers.onChange = [this] { changeAudioDriver(); };
    audioDrivers.setSelectedId(1);
}

void PropertiesComponent::changeAudioDriver() {
    int id = audioDrivers.getSelectedId() - 1;

    deviceManager->setCurrentAudioDeviceType(deviceTypes[id]->getTypeName(), true);
}

void PropertiesComponent::resized()
{
    label.setBounds(10, 10, getWidth() - 20, 24);
    audioDrivers.setBounds(0, label.getBottom(), 100, 20);
}
