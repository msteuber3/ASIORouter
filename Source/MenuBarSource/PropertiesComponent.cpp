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
    audioSettingsComp = std::make_unique<juce::AudioDeviceSelectorComponent>(*deviceManager, 2, 20, 2, 20, true, true, true, true);

    addAndMakeVisible(*audioSettingsComp);
    audioSettingsComp->setSize(getWidth(), getHeight() - 10);
}

void PropertiesComponent::resized()
{
    label.setBounds(10, 10, getWidth() - 20, 24);
    audioSettingsComp->setBounds(0, label.getBottom(), getWidth(), getHeight());
}
