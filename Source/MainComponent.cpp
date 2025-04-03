//TODO: universal in&out min/max

#include "MainComponent.h"


// So MainComponent will inherit from component. 
//      Each input device will be its own AudioIODevice and will need its own AudioIODeviceCallback
//      Then, a custom AudioDeviceManager (Not that inherits from that, but replaces it) 
//      & a mixer class that recieves all input buffers

// AudioIODevice -> AudioProcessorPlayer:audioDeviceIOCallbackWithContext -> an AudioProcessor -> an audioProcessor graph -> some output
// Remember that integral to this plan is the 

// Required classes:
/*
*  MainComponent:Component
* Scan for device types and have the user select one, then pass that to v
*  InputDevice:AudioProcessorPlayer
*       The device class should turn the input channels of the device into a list of AudioProcessors in a graph and then the final graph is a compiliation of all the graphs
*  InputChannel:AudioProcessor
*       This is where the sliders come in
*  InputDeviceManager
* 
*/


MainComponent::MainComponent()  : Component()
{
    mainDeviceManager.initialise(20, 2, nullptr, true);
    mainDeviceManager.createAudioDeviceTypes(deviceTypes);

    createGuiElements();


    setPaintingIsUnclipped(true);
    setSize(1000, 800);
}

MainComponent::~MainComponent() {}

void MainComponent::createGuiElements() {
    juce::Label deviceTypeLabel{ {}, "Select audio driver" };
    juce::Font textFont{ 12.0f };
    deviceTypeLabel.setFont(textFont);
    addAndMakeVisible(deviceTypeLabel);
    addAndMakeVisible(audioDrivers);
    for (size_t i = 0; i < deviceTypes.size(); i++) {
        audioDrivers.addItem(deviceTypes[i]->getTypeName(), i + 1);
    }
    audioDrivers.onChange = [this] { changeAudioDriver(); };
    audioDrivers.setSelectedId(1);
    ScanCurrentDriver();

}

void MainComponent::changeAudioDriver() {
    int id = audioDrivers.getSelectedId() - 1;
    mainDeviceManager.setCurrentAudioDeviceType(deviceTypes[id]->getTypeName(), true);
}

void MainComponent::ScanCurrentDriver() {
    auto* deviceType = mainDeviceManager.getCurrentDeviceTypeObject();

    if (!deviceType) {
        DBG("Error: No valid device type found.");
        return;
    }
    int x;
    int y;
    deviceType->scanForDevices();
    inDeviceNames = deviceType->getDeviceNames(true);
    outDeviceNames = deviceType->getDeviceNames(false);
	int ind = 0;
    for (const juce::String& name : inDeviceNames) {
        // Create input device
        auto inDevice = std::make_unique<AudioInputDevice>(name, name, deviceType, ind);
        int maxIn = inDevice->createChannels(this);

        // Move unique_ptr into vector
        inputDevices.push_back(std::move(inDevice));
        ind++;

        // Add channels to the graph
        for (int inChannel = 0; inChannel < maxIn; inChannel++) {
            if (inputDevices.back()->channels[inChannel]) {  // Ensure valid pointer
                x = inputDevices.back()->channels[inChannel]->getXCoord();
                y = inputDevices.back()->channels[inChannel]->getYCoord();
                inputDevices.back()->channels[inChannel]->setBounds(x, y, 100, 200);
                outputGraph.addNode(std::move(inputDevices.back()->channels[inChannel]));
                
            }
        }
    }
}

void MainComponent::resized() {
    //textLabel.setBounds(10, 10, getWidth() - 20, 20);
    audioDrivers.setBounds(10, 40, getWidth() - 20, 20);

    for (int ind = 0; ind < channels.size(); ind++) {
        int x = channels[ind]->getXCoord();
        int y = channels[ind]->getYCoord();
        channels[ind]->setBounds(x, y, 200, 100);
    }

    //int channelWidth = 75;
    //for (int i = 0; i < channels.size(); i++) {
    //    channels[i]->setBounds(i * channelWidth, 0, channelWidth, 300);
   // }
//    audioSetting->setBounds(10, 300, 300, 400);

}

void MainComponent::initializeMenu() {}

