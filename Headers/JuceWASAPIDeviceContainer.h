/*
  ==============================================================================

    JuceWASPIDeviceContainer.h
    Created: 17 Apr 2025 10:24:44pm
    Author:  Michael

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <RouterHeader.h>
#include <JuceWASAPIDevice.h>

class JuceWASAPIDeviceContainer : public juce::Component {
public:
    JuceWASAPIDeviceContainer(std::unique_ptr<juce::AudioIODeviceType> deviceType);
    ~JuceWASAPIDeviceContainer() override;

    void createGUI();

    void createInputDevices();

    void createOutputDevices();

    void findDeviceName();
    
    void sortDevices();

    void createSortedDevices();

    void resized() override;

    void paint(juce::Graphics& g) override;

    class SortedWASPIDevice : public juce::GroupComponent {
    public:
        SortedWASPIDevice(juce::String name);
        SortedWASPIDevice(juce::String name, bool isSolo);
        ~SortedWASPIDevice() override;
        
        void insertNewDevice(JuceWASAPIDevice* newDevice, bool isInput);

        int getDeviceSizeInPixels(int index, bool isInput);

        int getFullWidth();

        int getLargestDeviceNumInChannels();

        int getLargestDeviceNumOutChannels();

        int getNumInChannels();

        int getNumOutChannels();

        juce::String getName();

        void createGUI();

        void resized() override;

        void soloResize();

        void paint(juce::Graphics& g) override;
    
        private:
            juce::String name;

            juce::OwnedArray<JuceWASAPIDevice> inputDevices;
            juce::OwnedArray<JuceWASAPIDevice> outputDevices;
            
            juce::Label deviceLabel;
            juce::Label inputSectionLabel;
            juce::Label outputSectionLabel;

            bool isInput = false;
            bool solo = false;
    };

private:

    std::unique_ptr<juce::AudioIODeviceType> deviceType;

    juce::StringArray inputDeviceNames;
    juce::StringArray outputDeviceNames;
    juce::StringArray allDeviceNames;

    int numActualDevices = 0;

    juce::OwnedArray<JuceWASAPIDevice> inputDevices;
    juce::OwnedArray<JuceWASAPIDevice> outputDevices;

    juce::StringArray foundHardwareDeviceNames;

    juce::OwnedArray<juce::OwnedArray<JuceWASAPIDevice>> idontrememberwhatthisisfor;

    const juce::StringArray knownRoles = {
        "microphone", 
        "speakers", 
        "line in", 
        "line out", 
        "headphones", 
        "stereo mix"
    };

    const juce::StringArray knownHardware = {
        "focusrite"
     };

    juce::FlexBox containerBox;

    std::map<juce::String, std::unique_ptr<SortedWASPIDevice>> sortedDevicesMap;

};