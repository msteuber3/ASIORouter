#pragma once
#define NOMINMAX

#include <MainMixer.h>
#include <RouterHeader.h>
#include <MenuBar.h>
#include <JuceHeader.h>

class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    bool initializeDeviceManager();

    void createMixer();

    void resized() override;

    void createGuiElements();

    void paint(juce::Graphics& g) override;

    
private:
    juce::String deviceTypeToRetrieve;

    juce::FlexBox mainFlexBox;

    MenuBar menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;

    std::unique_ptr<MainMixer> mixer;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
