/*
  ==============================================================================

    JuceWASAPIDeviceContainer.cpp
    Created: 17 Apr 2025 10:25:37pm
    Author:  Michael

  ==============================================================================
*/

#include "JuceWASAPIDeviceContainer.h"

JuceWASAPIDeviceContainer::JuceWASAPIDeviceContainer(std::unique_ptr<juce::AudioIODeviceType> deviceType) : juce::Component(), deviceType(std::move(deviceType))
{
    createGUI();
    createInputDevices();
    createOutputDevices();
    sortDevices();
    createSortedDevices();

    DBG("Input devices created: " + juce::String(inputDevices.size()));
    DBG("Output devices created: " + juce::String(outputDevices.size()));
    DBG("Sorted devices created: " + juce::String(sortedDevicesMap.size()));
}

JuceWASAPIDeviceContainer::~JuceWASAPIDeviceContainer()
{
}

void JuceWASAPIDeviceContainer::createGUI()
{
    containerBox.flexDirection = juce::FlexBox::Direction::row;
    containerBox.flexWrap = juce::FlexBox::Wrap::wrap;
}

void JuceWASAPIDeviceContainer::createInputDevices()
{
    deviceType->scanForDevices();

    inputDeviceNames = deviceType->getDeviceNames(true);

    for (const juce::String& name : inputDeviceNames) {
        auto newAudioDevice = deviceType->createDevice(name, name);
        if (!newAudioDevice->getInputChannelNames().isEmpty()) {
            auto device = std::make_unique<JuceWASAPIDevice>(newAudioDevice, true);            
            inputDevices.add(std::move(device));
        }
        else {
            newAudioDevice->close();
        }
    }
}

void JuceWASAPIDeviceContainer::createOutputDevices()
{
    deviceType->scanForDevices();

    outputDeviceNames = deviceType->getDeviceNames(false);

    for (const juce::String& name : outputDeviceNames) {
        auto newAudioDevice = deviceType->createDevice(name, name);
        if (!newAudioDevice->getOutputChannelNames().isEmpty() && !name.containsIgnoreCase("realtek digital")) {
            auto device = std::make_unique<JuceWASAPIDevice>(newAudioDevice, false);
            outputDevices.add(std::move(device));
        }
        else {
            newAudioDevice->close();
        }
    }
}
void JuceWASAPIDeviceContainer::findDeviceName()
{
    // Get full list of device names
    // Identify common words
    // Map devices that contain that name to an array, so
    // juce::OwnedArray<std::map<juce::String, juce::OwnedArray<JuceWASAPIDevice>>> <- holy shit
    //                   |                 |
    //                   V                 V
    //              Device name         array of devices, when writing the grid code,  check for in or out to determine where to put it
    // I might another class inside this one, this one containing the logic and that one handling the GUI. So this creates the devices, that one stores them
    // so that class contains a name member, an OwnedArray of ins, and an OwnedArray of outs
    // This one contains an OwnedArray of that one, we'll call it SortedWASAPIDevices
}

void JuceWASAPIDeviceContainer::sortDevices()
{
    for (juce::String hardware : knownHardware) {
        foundHardwareDeviceNames.add(hardware);
    }
}


void JuceWASAPIDeviceContainer::createSortedDevices()
{
    for (juce::String devName : foundHardwareDeviceNames) {
        sortedDevicesMap.emplace(devName, std::make_unique<SortedWASPIDevice>(devName));
        addAndMakeVisible(*sortedDevicesMap.at(devName));
        containerBox.items.add(juce::FlexItem(*sortedDevicesMap.at(devName)).withFlex(1));
    }

    // This is fucking stupid and needs to be cleaned
    juce::String deviceToSearchFor;
    juce::String unsortedName;

    for (int i = 0; i < inputDevices.size(); i++) {
        unsortedName = inputDevices[i]->getShortDeviceName();
        DBG("lOOKING FOR " + unsortedName + "(input)");

        for (int j = 0; j < foundHardwareDeviceNames.size(); j++) { // Dummy loop rn for testing, feel free to rewrite later
            deviceToSearchFor = foundHardwareDeviceNames[j];
            if (unsortedName.containsIgnoreCase(deviceToSearchFor)) {

                sortedDevicesMap.at(deviceToSearchFor)->insertNewDevice((inputDevices.removeAndReturn(i)), true); //holy shit that was painful to write
                
                DBG("Sorry me in like half an hour, I'm not gonna gice you the name of the device, but one onf them moved somewhere. Ill give you a hint: " + unsortedName + " -> " + deviceToSearchFor + " and it's an input device");
            }
            deviceToSearchFor.clear();
        }
        unsortedName.clear();
    }

    for (int i = 0; i < outputDevices.size(); i++) {
        unsortedName = outputDevices[i]->getShortDeviceName();
        DBG("lOOKING FOR " + unsortedName + "(output)");

        for (int j = 0; j < foundHardwareDeviceNames.size(); j++) { // Dummy loop rn for testing, feel free to rewrite later
            deviceToSearchFor = foundHardwareDeviceNames[j];

            if (unsortedName.containsIgnoreCase(deviceToSearchFor)) {
                if (unsortedName.indexOf(deviceToSearchFor) != -1)
                    outputDevices[i]->renameDevice(unsortedName.replaceSection(unsortedName.indexOf(deviceToSearchFor), deviceToSearchFor.length(), ""));
                sortedDevicesMap.at(deviceToSearchFor)->insertNewDevice(outputDevices.removeAndReturn(i), false); //holy shit that was painful to write
                
                DBG("Sorry me in like half an hour, I'm not gonna gice you the name of the device, but one onf them moved somewhere. Ill give you a hint: " + unsortedName + " -> " + deviceToSearchFor + " and it's an output device");
            }
            deviceToSearchFor.clear();
        }
        unsortedName.clear();
    }

    if (!inputDevices.isEmpty()) {

        for (int i = 0; i < inputDevices.size(); i++) {
            juce::String devName = inputDevices[i]->getShortDeviceName();

            sortedDevicesMap.emplace(devName, std::make_unique<SortedWASPIDevice>(devName));

            sortedDevicesMap.at(devName)->insertNewDevice(inputDevices.removeAndReturn(i), true);

            addAndMakeVisible(*sortedDevicesMap.at(devName));
            containerBox.items.add(juce::FlexItem(*sortedDevicesMap.at(devName)).withFlex(1));
        }
    }
    if (!outputDevices.isEmpty()) {

        for (int i = 0; i < outputDevices.size(); i++) {
            juce::String devName = outputDevices[i]->getShortDeviceName();

            sortedDevicesMap.emplace(devName, std::make_unique<SortedWASPIDevice>(devName));

            sortedDevicesMap.at(devName)->insertNewDevice(outputDevices.removeAndReturn(i), false);

            addAndMakeVisible(*sortedDevicesMap.at(devName));
            containerBox.items.add(juce::FlexItem(*sortedDevicesMap.at(devName)).withFlex(1));
        }
    }

}
void JuceWASAPIDeviceContainer::resized()
{
    DBG("JuceWASAPIDeviceContainer bounds: " + getLocalBounds().toString());

    containerBox.performLayout(getLocalBounds().reduced(10));

    int count = 0;
    for (auto& [name, device] : sortedDevicesMap) {
        if (count++ < 3) { // Just check first 3 to avoid log spam
            DBG(name + " bounds: " + device->getBounds().toString());
        }
    }
}

void JuceWASAPIDeviceContainer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.withAlpha(0.2f));
    g.setColour(juce::Colours::yellow);
    g.drawRect(getLocalBounds(), 2);

}



//========================================================================================================================

//                                    SORTED DEVICES CLASS

// this needs to handle standalone devices too

JuceWASAPIDeviceContainer::SortedWASPIDevice::SortedWASPIDevice(juce::String name) : juce::GroupComponent(), name(name)
{
    createGUI();
}

JuceWASAPIDeviceContainer::SortedWASPIDevice::SortedWASPIDevice(juce::String name, bool isSolo) : juce::GroupComponent(), name(name)
{
    solo = true;
    createGUI();
}

JuceWASAPIDeviceContainer::SortedWASPIDevice::~SortedWASPIDevice(){}

void JuceWASAPIDeviceContainer::SortedWASPIDevice::insertNewDevice(JuceWASAPIDevice *newDevice, bool isInput)
{
    if (isInput) {
        inputDevices.add(std::unique_ptr<JuceWASAPIDevice>(newDevice));
        addAndMakeVisible(inputDevices.getLast());

    }
    else {
        outputDevices.add(std::unique_ptr<JuceWASAPIDevice>(newDevice));
        addAndMakeVisible(outputDevices.getLast());
    }

}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getDeviceSizeInPixels(int index, bool isInput)
{
    int numChannels = isInput ? inputDevices[index]->getNumChannels() : outputDevices[index]->getNumChannels();
    return numChannels * SLIDER_WIDTH; // + whatever the device spacing is, come back later for that
}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getFullWidth()
{
    int inWidth = 0;
    int outWidth = 0;
    for (int i = 0; i < inputDevices.size(); i++) {
        inWidth += getDeviceSizeInPixels(i, true);
    }
    for (int j = 0; j < outputDevices.size(); j++) {
        outWidth += getDeviceSizeInPixels(j, false);
    }

    return juce::jmax(inWidth, outWidth); 
}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getLargestDeviceNumInChannels()
{
    int largest = 0;
    for (int i = 0; i < inputDevices.size(); i++)
        largest = inputDevices[i]->getNumChannels() > largest ? inputDevices[i]->getNumChannels() : largest;

    return largest;
}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getLargestDeviceNumOutChannels()
{
    int largest = 0;
    for (int i = 0; i < outputDevices.size(); i++)
        largest = outputDevices[i]->getNumChannels() > largest ? outputDevices[i]->getNumChannels() : largest;

    return largest;
}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getNumInChannels()
{
    int largest = 0;
    for (int i = 0; i < inputDevices.size(); i++)
        largest += inputDevices[i]->getNumChannels() + 1; // Add an additional channel's worth of space between jucedevices

    return largest; //& and remove the last 
}

int JuceWASAPIDeviceContainer::SortedWASPIDevice::getNumOutChannels()
{
    int largest = 0;
    for (int i = 0; i < outputDevices.size(); i++)
        largest += outputDevices[i]->getNumChannels() + 1; // Same dealio here, see above

    return largest; //yur
}


juce::String JuceWASAPIDeviceContainer::SortedWASPIDevice::getName()
{
    return name;
}

void JuceWASAPIDeviceContainer::SortedWASPIDevice::createGUI()
{
    deviceLabel.setText(name, juce::dontSendNotification);
    deviceLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(deviceLabel);

    inputSectionLabel.setText("Inputs", juce::dontSendNotification);
    inputSectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    inputSectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inputSectionLabel);

    outputSectionLabel.setText("Outputs", juce::dontSendNotification);
    outputSectionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    outputSectionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputSectionLabel);
}

void JuceWASAPIDeviceContainer::SortedWASPIDevice::resized()
{
    DBG("SortedWASPIDevice " + name + " resizing. Input devices: " + juce::String(inputDevices.size()) +
        ", Output devices: " + juce::String(outputDevices.size()));

    if (!solo) {
        auto bounds = getLocalBounds().reduced(10);

        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;
        using Px = juce::Grid::Px;

        juce::Grid grid;

        // Define grid settings
        grid.rowGap = Px(10);
        grid.columnGap = Px(5);

        // Calculate number of columns needed: max of input and output channel counts
        const int numColumns = juce::jmax(getNumInChannels(), getNumOutChannels());

        const int largestDevice = juce::jmax(getLargestDeviceNumInChannels(), getLargestDeviceNumOutChannels());

        // Setup rows (device name, inputs label, input channels, outputs label, output channels)
        grid.templateRows = {
            Track(Px(30)), // device name 
            Track(Px(20)), // Inputs label
            Track(Px(250)), //input channels
            Track(Px(20)), //outputs label
            Track(Px(250)) }; //output channels

        // Setup columns - one for each channel
        juce::Array<Track> columnTracks;
        for (int i = 0; i < numColumns; ++i)
            columnTracks.add(Track(Px((largestDevice * SLIDER_WIDTH) + 20)));

        grid.templateColumns = columnTracks;
        // Create a Grid Item for each component

        // Device label (spans all columns)
        auto deviceLabelItem = juce::GridItem(deviceLabel).withArea(1, 1).withJustifySelf(juce::GridItem::JustifySelf::center);
        grid.items.add(deviceLabelItem);

        // Input section label (spans all columns)
        auto inputLabelItem = juce::GridItem(inputSectionLabel).withArea(2, 1).withJustifySelf(juce::GridItem::JustifySelf::center);
        grid.items.add(inputLabelItem);

        // Input channels
        for (int i = 0; i < inputDevices.size(); i++) {
            auto channelItem = juce::GridItem(*inputDevices[i]).withArea(3, i + 1, 4, i + 2);
            grid.items.add(channelItem);
        }

        // Output section label (spans all columns)
        auto outputLabelItem = juce::GridItem(outputSectionLabel).withArea(4, 1).withJustifySelf(juce::GridItem::JustifySelf::center);
        grid.items.add(outputLabelItem);

        // Output channels
        for (int i = 0; i < outputDevices.size(); i++) {
            auto channelItem = juce::GridItem(*outputDevices[i]).withArea(5, i + 1, 6, i + 2);
            grid.items.add(channelItem);
        }

        // Perform layout
        grid.performLayout(bounds);
    }
    else { soloResize(); }
}

void JuceWASAPIDeviceContainer::SortedWASPIDevice::soloResize()
{
    auto bounds = getLocalBounds().reduced(10);

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    juce::Grid grid;

    // Define grid settings
    grid.rowGap = Px(10);
    grid.columnGap = Px(5);

    // Calculate number of columns needed: max of input and output channel counts
    const int numColumns = juce::jmax(getNumInChannels(), getNumOutChannels());

    const int largestDevice = juce::jmax(getLargestDeviceNumInChannels(), getLargestDeviceNumOutChannels());

    // Setup rows (device name, inputs label, input channels, outputs label, output channels)
    grid.templateRows = {
        Track(Px(30)), // device name 
        Track(Px(20)), // type label
        Track(Px(250)), //type channels
    };

    // Setup columns - one for each channel
    juce::Array<Track> columnTracks;
    for (int i = 0; i < numColumns; ++i)
        columnTracks.add(Track(Px(largestDevice * SLIDER_WIDTH)));

    grid.templateColumns = columnTracks;

    // Create a Grid Item for each component

    // Device label (spans all columns)
    auto deviceLabelItem = juce::GridItem(deviceLabel).withArea(1, 1).withJustifySelf(juce::GridItem::JustifySelf::center);
    grid.items.add(deviceLabelItem);

    if (isInput) {
        // Input section label (spans all columns)
        auto inputLabelItem = juce::GridItem(inputSectionLabel).withArea(2, 1).withJustifySelf(juce::GridItem::JustifySelf::center);
        grid.items.add(inputLabelItem);

        // Input channels
        for (int i = 0; i < inputDevices.size(); i++) {
            auto channelItem = juce::GridItem(*inputDevices[i]).withArea(3, i + 1, 4, i + 2);
            grid.items.add(channelItem);
        }
    }
    else if (!isInput) {
        // Output section label (spans all columns)
        auto outputLabelItem = juce::GridItem(outputSectionLabel).withArea(2, 1, 3, numColumns + 1);
        grid.items.add(outputLabelItem);

        // Output channels
        for (int i = 0; i < outputDevices.size(); i++) {
            auto channelItem = juce::GridItem(*outputDevices[i]).withArea(3, i + 1, 4, i + 2);
            grid.items.add(channelItem);
        }
    }
    // Perform layout
    grid.performLayout(bounds);
}

void JuceWASAPIDeviceContainer::SortedWASPIDevice::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.1f));
    g.setColour(juce::Colours::green);
    g.drawRect(getLocalBounds(), 2);
}
