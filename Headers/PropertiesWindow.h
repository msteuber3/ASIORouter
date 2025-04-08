#pragma once
#include <JuceHeader.h>
#include <PropertiesComponent.h>

class PropertiesWindow : public juce::DocumentWindow
{
public:
    PropertiesWindow();

    void closeButtonPressed() override;
};
