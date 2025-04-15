#pragma once
#include <PropertiesComponent.h>

PropertiesComponent::PropertiesComponent() : Component()
{
    generateDriverDropdown();
}

PropertiesComponent::~PropertiesComponent(){
    removeAllChildren();
}

void PropertiesComponent::generateDriverDropdown(){
    juce::Font textFont{ 12.0f };
    label.setFont(textFont);
    addAndMakeVisible(label);
    audioSettingsComp = new juce::AudioDeviceSelectorComponent{ *deviceManager, 2, 16, 2, 2, true, true, true, false };

    addAndMakeVisible(audioSettingsComp);
    audioSettingsComp->setSize(getWidth(), getHeight() - 10);
}

void PropertiesComponent::resized()
{
    label.setBounds(10, 10, getWidth() - 20, 24);
    audioSettingsComp->setBounds(0, label.getBottom(), getWidth(), getHeight());
}
