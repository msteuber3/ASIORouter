#pragma once
#include <JuceHeader.h>
#include <RouterHeader.h>

class PropertiesComponent : public juce::Component
{
public:
    PropertiesComponent();
    ~PropertiesComponent() override;
    
    void resized() override;
    
    void generateDriverDropdown();
        
private:
    juce::Label label{ {}, "Select audio driver" };

    juce::AudioDeviceSelectorComponent *audioSettingsComp;    
};

