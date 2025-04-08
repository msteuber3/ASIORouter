#pragma once
#include <PropertiesComponent.h>

PropertiesComponent::PropertiesComponent()
{
    addAndMakeVisible(label);
    label.setText("Properties go here!", juce::dontSendNotification);
}

void PropertiesComponent::resized()
{
    label.setBounds(10, 10, getWidth() - 20, 24);
}