#pragma once
#include <JuceHeader.h>

class PropertiesComponent : public juce::Component
{
public:
    PropertiesComponent();

    void resized() override;

private:
    juce::Label label;
};

