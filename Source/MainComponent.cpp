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

/*
* MainComponent shouhld contain the following:
* GUI stuff, so for the IO portion of this addAndMakeVisible a mixer object
* 
* Mixer should take all the input & output info and position it
* So Mixer Component would be an apt description
* 
* each AudioInputDevice should place each individual slider in a row
* 
* I also need a lookandfeel class
* 
*/
MainComponent::MainComponent()  : Component()
{
    mainDeviceManager = std::make_unique<juce::AudioDeviceManager>();
    mainDeviceManager->initialise(20, 2, nullptr, true);
    mainDeviceManager->createAudioDeviceTypes(deviceTypes);

    outputGraph = std::make_unique<juce::AudioProcessorGraph>();

    createGuiElements();


    setSize(1000, 800);
}

MainComponent::~MainComponent() 
{
    outputGraph->clear();
    inputDevices.clear();
    mainDeviceManager->removeAllChangeListeners();
    mainDeviceManager->closeAudioDevice();
    mainDeviceManager.reset();
    removeAllChildren();
}

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
    mainDeviceManager->setCurrentAudioDeviceType(deviceTypes[id]->getTypeName(), true);
}

void MainComponent::ScanCurrentDriver() {
    auto* deviceType = mainDeviceManager->getCurrentDeviceTypeObject();

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
                // inputDevices.back()->channels[inChannel]->setBounds(x, y, 100, 200);

                outputGraph->addNode(std::move(inputDevices.back()->channels[inChannel]));

            }
        }
        addAndMakeVisible(*inputDevices.back());
        mainFlexBox.items.add(juce::FlexItem(*inputDevices.back()).withMinHeight(200.0f).withMinWidth(600.0f));

        
    }
}

void MainComponent::resized() {
    //textLabel.setBounds(10, 10, getWidth() - 20, 20);
    audioDrivers.setBounds(10, 40, getWidth() - 20, 20);

    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 50, 1000, 600);

    mainFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlexBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mainFlexBox.performLayout(fbRect);

   

    //int channelWidth = 75;
    //for (int i = 0; i < channels.size(); i++) {
    //    channels[i]->setBounds(i * channelWidth, 0, channelWidth, 300);
   // }
//    audioSetting->setBounds(10, 300, 300, 400);

}

void MainComponent::initializeMenu() {}

