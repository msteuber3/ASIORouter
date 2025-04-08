#pragma once
#include <PropertiesComponent.h>

PropertiesComponent::PropertiesComponent()
{
    addAndMakeVisible(label);
    label.setText("Properties go here!", juce::dontSendNotification);
    generateDriverDropdown();
}

void PropertiesComponent::generateDriverDropdown(){
    juce::Label deviceTypeLabel{ {}, "Select audio driver" };
    juce::Font textFont{ 12.0f };
    deviceTypeLabel.setFont(textFont);
    addAndMakeVisible(deviceTypeLabel);
    addAndMakeVisible(audioDrivers);
    //mainFlexBox.items.add(juce::FlexItem(deviceTypeLabel).withMinWidth(20).withMinHeight(10));
    for (size_t i = 0; i < deviceTypes.size(); i++) {
        audioDrivers.addItem(deviceTypes[i]->getTypeName(), i + 1);
    }
    audioDrivers.onChange = [this] { changeAudioDriver(); };
    audioDrivers.setSelectedId(1);
    mainFlexBox.items.add(juce::FlexItem(audioDrivers).withMinWidth(100).withMinHeight(40));
    auto* deviceType = mainDeviceManager->getCurrentDeviceTypeObject();
}

void PropertiesComponent::changeAudioDriver() {
    int id = audioDrivers.getSelectedId() - 1;
    this->getParentComponent()->getParentComponent()
    mainDeviceManager->setCurrentAudioDeviceType(deviceTypes[id]->getTypeName(), true);
}

void PropertiesComponent::resized()
{
    label.setBounds(10, 10, getWidth() - 20, 24);
}
