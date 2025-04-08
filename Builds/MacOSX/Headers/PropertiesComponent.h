#pragma once
#include <JuceHeader.h>

class PropertiesComponent : public juce::Component
{
public:
    PropertiesComponent();
    
    void resized() override;
    
    void generateDriverDropdown();
    
    void changeAudioDriver();
    
private:
    juce::Label label;

    juce::ComboBox audioDrivers;
    juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;

    
};

