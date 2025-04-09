//TODO: universal in&out min/max

#include <MainComponent.h>


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
std::unique_ptr<juce::AudioDeviceManager> deviceManager;

MainComponent::MainComponent() : Component()
{
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    deviceManager->initialise(20, 20, nullptr, true);
    deviceManager->createAudioDeviceTypes(deviceTypes);
    outputGraph = std::make_unique<juce::AudioProcessorGraph>();

    createGuiElements();


    setSize(2000, 800);
}

MainComponent::~MainComponent() 
{
   outputGraph->clear();
   inputDevices.clear();
   deviceManager->removeAudioCallback(&processorPlayer);
   processorPlayer.setProcessor(nullptr);
   deviceManager->removeAllChangeListeners();
   deviceManager->closeAudioDevice();
   deviceManager.reset();
   removeAllChildren();
}

std::vector<std::tuple<juce::AudioChannelSet, juce::String>> MainComponent::setBusesProperties(bool input) {
    std::vector <std::tuple<juce::AudioChannelSet, juce::String>> channelVector;
    auto* deviceType = deviceManager->getCurrentDeviceTypeObject();
    deviceType->scanForDevices();
    juce::StringArray devices = deviceType->getDeviceNames(input);
    for (juce::String& name : devices) {
        if (name.contains("Realtek Digital Output")) {
            DBG("REALTEK NAME:" + name);
            break;
        }
        auto deviceObject = deviceType->createDevice(name, name);

        const juce::BigInteger& numInputs = deviceObject->getInputChannelNames().size();
        const juce::BigInteger& numOutputs = deviceObject->getOutputChannelNames().size();

        deviceObject->open(numInputs, numOutputs, 44100, 512);

        auto activeInputChannels = input ? deviceObject->getActiveInputChannels() : deviceObject->getActiveOutputChannels();
        int maxIO = activeInputChannels.getHighestBit() + 1;
        if (maxIO == 1) { channelVector.push_back(std::tuple<juce::AudioChannelSet, juce::String>(juce::AudioChannelSet::mono(), name)); }
        else if (maxIO == 2) { channelVector.push_back(std::tuple<juce::AudioChannelSet, juce::String>(juce::AudioChannelSet::stereo(), name)); }
        else { channelVector.push_back(std::tuple<juce::AudioChannelSet, juce::String>(juce::AudioChannelSet::discreteChannels(maxIO), name)); }
        deviceObject->close();
    }
    return channelVector;
}

void MainComponent::createGuiElements() {

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);

    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> inputBuses = setBusesProperties(true);
    std::vector<std::tuple<juce::AudioChannelSet, juce::String>> outputBuses = setBusesProperties(false);

    auto* deviceType = deviceManager->getCurrentDeviceTypeObject();
    mixer = std::make_unique<MainMixer>(deviceType, inputBuses, outputBuses);
    processorPlayer.setProcessor(mixer.get());
    deviceManager->addAudioCallback(&processorPlayer);

}
void MainComponent::resized() {
    //textLabel.setBounds(10, 10, getWidth() - 20, 20);
    //audioDrivers.setBounds(10, 40, getWidth() - 20, 20);
    menuBar->setBounds(0, 0, getWidth(), 25);
    addAndMakeVisible(*mixer);
    mainFlexBox.items.add(juce::FlexItem(*mixer).withMinWidth(getWidth()).withMinHeight(800));

    juce::Rectangle<int> fbRect = juce::Rectangle<int>(0, 30, getWidth(), getHeight());

    mainFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlexBox.alignContent = juce::FlexBox::AlignContent::flexStart;

    mainFlexBox.performLayout(fbRect);
}

void MainComponent::initializeMenu() {}

