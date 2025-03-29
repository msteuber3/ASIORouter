#include "Channel.h"
#include "MainAudioComponent.h"

MainAudioComponent::MainAudioComponent()
{
    setSize(600, 100);
    setAudioChannels(2, 2);
}

MainAudioComponent::~MainAudioComponent()
{
}

void MainAudioComponent::prepareToPlay(int, double)
{
}
void MainAudioComponent::releaseResources()
{

}


void MainAudioComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

    std::vector<Channel*> channelList;

    for (int i = 0; i < maxInputChannels; i++) {
        channelList.push_back(new Channel(i));
    }
}
